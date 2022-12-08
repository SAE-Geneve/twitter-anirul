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
		proto::TweetOut tweet_out = client_->Tweet(tweet_in);
		if (!tweet_out.error())
		{
			std::cout << "Successful!" << std::endl;
			return true;
		}
		std::cout << "Error!" << std::endl;
		return false;
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
		proto::FollowOut follow_out = client_->Follow(follow_in);
		if (!follow_out.error())
		{
			std::cout 
				<< "Successfully following: [" 
				<< follow 
				<< "]" 
				<< std::endl;
			return true;
		}
		std::cout << "Couldn't follow: [" << follow << "]" << std::endl;
		return false;
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
		proto::ShowOut show_out = client_->Show(show_in);
		if (!show_out.error())
		{
			if (!show_out.mutable_tweets()->empty())
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
		std::cout << "no tweet from user: [" << user << "]" << std::endl;
		return false;
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
		proto::LoginOut login_out = client_->Login(login_in);
		if (!login_out.error())
		{
			std::cout 
				<< "successfully log in as : [" 
				<< name 
				<< "]." 
				<< std::endl;
			return true;
		}
		std::cout << "couldn't login..." << std::endl;
		return false;
	}

	bool Command::LogOut()
	{
		std::cout << "LOGOUT" << std::endl;
		proto::LogoutIn logout_in{};
		proto::LogoutOut logout_out = client_->Logout(logout_in);
		if (!logout_out.error())
		{
			std::cout << "Successful!" << std::endl;
			return true;
		}
		std::cout << "Error!" << std::endl;
		return false;
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
		proto::RegisterOut register_out = client_->Register(register_in);
		if (!register_out.error())
		{
			std::cout << "Successful!" << std::endl;
			return true;
		}
		std::cout << "Error!" << std::endl;
		return false;
	}

} // End namespace client.
