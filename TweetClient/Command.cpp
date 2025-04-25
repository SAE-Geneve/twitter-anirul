#include "Command.h"

namespace client {

	void Command::Run()
	{
		// Choose login or register or quit.
		if (LoginRegisterQuit())
		{
			while (ShowFollowTweetQuit()) {}
			LogOut();
		}
	}

	bool Command::LoginRegisterQuit()
	{
		std::cout << "\t(1)\tLOGIN" << std::endl;
		std::cout << "\t(2)\tREGISTER" << std::endl;
		std::cout << "\t(3)\tQUIT" << std::endl;
		std::cout << "Enter your choice: " << std::endl;
		std::cout << " > ";
		int value;
		std::cin >> value;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		switch (value)
		{
		case 1:
			return Login();
		case 2:
			return Register();
		case 3:
			return true;
		default:
			return false;
		}
	}

	bool Command::ShowFollowTweetQuit()
	{
		std::cout << "\t(1)\tSHOW" << std::endl;
		std::cout << "\t(2)\tFOLLOW" << std::endl;
		std::cout << "\t(3)\tTWEET" << std::endl;
		std::cout << "\t(4)\tQUIT" << std::endl;
		std::cout << "Enter your choice: " << std::endl;
		std::cout << " > ";
		int value;
		std::cin >> value;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		switch (value)
		{
		case 1:
			return Show();
		case 2:
			return Follow();
		case 3:
			return Tweet();
		case 4:
			return false;
		default:
			return false;
		}
	}

	bool Command::Tweet()
	{
		std::cout << "TWEET" << std::endl;
		std::cout << "I feel like tweeting: " << std::endl;
		std::cout << " > ";
		std::string text;
		std::getline(std::cin, text);
		proto::TweetIn tweet_in{};
		tweet_in.set_content(text);
		std::scoped_lock l(client_mutex_);
		auto tweet_status = client_->Tweet(tweet_in);
		return tweet_status.Ok();
	}

	bool Command::Follow()
	{
		std::cout << "FOLLOW" << std::endl;
		std::cout << "I want to follow: " << std::endl;
		std::cout << " > ";
		std::string follow;
		std::cin >> follow;
		proto::FollowIn follow_in{};
		follow_in.set_name(follow);
		std::scoped_lock l(client_mutex_);
		auto status = client_->Follow(follow_in);
		return status.Ok();
	}

	bool Command::Show()
	{
		std::cout << "SHOW" << std::endl;
		std::cout << "Show me the tweet from user: " << std::endl;
		std::cout << " > ";
		std::string user;
		std::cin >> user;
		proto::ShowIn show_in{};
		show_in.set_user(user);
		proto::ShowOut show_out;
		std::scoped_lock l(client_mutex_);
		auto show_status = client_->Show(show_in);
		if (!show_status.Ok())
		{
			return false;
		}
		if (!show_status.value.mutable_tweets()->empty())
		{
			auto it = show_out.tweets().cbegin();
			std::cout 
				<< "tweet from user: [" 
				<< it->user() 
				<< "]" 
				<< std::endl;
			for (const auto& tweet_in : show_out.tweets())
			{
				std::cout << "\t" << tweet_in.time() << "\t";
				std::cout << tweet_in.content() << std::endl;
			}
			std::cout << "done" << std::endl;
			return true;
		}
		std::cout << "no tweet from user: [" << user << "]" << std::endl;
		return true;
	}

	bool Command::Login()
	{
		std::cout << "LOGIN" << std::endl;
		std::cout << "Enter your name: " << std::endl;
		std::cout << " > ";
		std::string name;
		std::cin >> name;
		std::cout << "Enter your password: " << std::endl;
		std::cout << " > ";
		std::string pass;
		std::cin >> pass;
		proto::LoginIn login_in{};
		login_in.set_user(name);
		login_in.set_pass(pass);
		std::scoped_lock l(client_mutex_);
		auto login_status = client_->Login(login_in);
		if (!login_status.Ok())
		{
			std::cout << "couldn't login..." << std::endl;
			return false;
		}
		std::cout 
			<< "successfully log in as : [" 
			<< name 
			<< "]." 
			<< std::endl;
		return true;
	}

	bool Command::LogOut()
	{
		std::cout << "LOGOUT" << std::endl;
		proto::LogoutIn logout_in;
		std::scoped_lock l(client_mutex_);
		logout_in.set_token(token_.value());
		auto logout_status = client_->Logout(logout_in);
		return logout_status.Ok();
	}

	bool Command::Register()
	{
		std::cout << "REGISTER" << std::endl;
		std::cout << "Enter your name: " << std::endl;
		std::cout << " > ";
		std::string name;
		std::cin >> name;
		std::cout << "Enter you password: " << std::endl;
		std::cout << " > ";
		std::string pass;
		std::cin >> pass;
		proto::RegisterIn register_in{};
		register_in.set_name(name);
		register_in.set_pass(pass);
		std::scoped_lock l(client_mutex_);
		auto register_status = client_->Register(register_in);
		if (register_status.Ok())
		{
			token_ = register_status.value.token();
			std::cout << "Successful!" << std::endl;
			return true;
		}
		token_ = std::nullopt;
		std::cout << "Error!" << std::endl;
		return false;
	}

	void Command::ProceedAsync()
	{
		// Wait for the token to be set.
		bool token_valid = false;
		while (!token_valid)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			std::scoped_lock l(client_mutex_);
			token_valid = token_.has_value();
		}

		proto::UpdateIn req;
		{
			std::scoped_lock l(client_mutex_);
			req.set_token(token_.value());
		}

		grpc::ClientContext context;
		auto reader = client_->Update(&context, req);

		proto::UpdateOut resp;
		while (reader->Read(&resp)) {
			// Got one batch of new tweets!
			// Lock your console/UI mutex, then dump them:
			std::lock_guard<std::mutex> lock(client_mutex_);
			for (const auto& t : resp.tweets()) {
				std::cout
					<< "\r[" << t.time() << "]\t"
					<< t.user() << "\t:\t"
					<< t.content() << "\n";
			}
		}

		// The stream is closed; check whether it ended OK
		grpc::Status status = reader->Finish();
		if (!status.ok()) {
			std::lock_guard<std::mutex> lock(client_mutex_);
			std::cerr << "Update stream closed: "
				<< status.error_message() << "\n";
		}
	}

} // End namespace client.
