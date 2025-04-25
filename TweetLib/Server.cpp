#include "Server.h"

#include <format>

namespace tweet {

	grpc::Status Server::Tweet(
		grpc::ServerContext* context, 
		const proto::TweetIn* request, 
		proto::TweetOut* response)
	{
		std::cout << std::format("Tweet({})\n", context->peer());
		response->set_error(
			storage_->Tweet(
				request->tocken(),
				request->content()));
		return grpc::Status::OK;
	}

	grpc::Status Server::Follow(
		grpc::ServerContext* context, 
		const proto::FollowIn* request, 
		proto::FollowOut* response)
	{
		std::cout << std::format("Follow({})\n", context->peer());
		response->set_error(
			storage_->Follow(
				request->tocken(),
				request->name()));
		return grpc::Status::OK;
	}

	grpc::Status Server::Show(
		grpc::ServerContext* context, 
		const proto::ShowIn* request, 
		proto::ShowOut* response)
	{
		std::cout << std::format("Show({})\n", context->peer());
		auto tweets = storage_->Show(
			request->tocken(),
			request->user());
		for (const auto& tweet : tweets)
		{
			proto::TweetIn tweet_in{};
			tweet_in.set_user(tweet.name);
			tweet_in.set_content(tweet.text);
			tweet_in.set_time(tweet.time);
			*response->add_tweets() = tweet_in;
		}
		response->set_error(true);
		return grpc::Status::OK;
	}

	grpc::Status Server::Login(
		grpc::ServerContext* context, 
		const proto::LoginIn* request, 
		proto::LoginOut* response)
	{
		std::cout << std::format("Login({})\n", context->peer());
		auto maybe_tocken = 
			storage_->Login(
				request->user(),
				request->pass());
		response->set_error(maybe_tocken.has_value());
		response->set_tocken(maybe_tocken.value());
		return grpc::Status::OK;
	}

	grpc::Status Server::Logout(
		grpc::ServerContext* context, 
		const proto::LogoutIn* request, 
		proto::LogoutOut* response)
	{
		std::cout << std::format("Logout({})\n", context->peer());
		storage_->Logout(request->tocken());
		response->set_error(true);
		return grpc::Status::OK;
	}

	grpc::Status Server::Register(
		grpc::ServerContext* context, 
		const proto::RegisterIn* request, 
		proto::RegisterOut* response)
	{
		std::cout << std::format("Register({})\n", context->peer());
		auto maybe_tocken =
			storage_->Register(
				request->name(),
				request->pass());
		response->set_error(maybe_tocken.has_value());
		response->set_tocken(maybe_tocken.value());
		return grpc::Status::OK;
	}

	grpc::Status Server::Update(
		grpc::ServerContext* context,
		const proto::UpdateIn* update_in,
		grpc::ServerWriter<proto::UpdateOut>* update_out_writer)
	{
		{
			std::lock_guard<std::mutex> lock(writers_mutex_);
			writers_.insert({ update_in->tocken(), update_out_writer });
		}
		while (!context->IsCancelled())
		{
			Sleep(100);
		}
		std::lock_guard<std::mutex> lock(writers_mutex_);
		writers_.erase(update_in->tocken());
		return grpc::Status::OK;
	}

	void Server::ProceedAsync()
	{
		bool proceed = true;
		std::map<std::int64_t, std::int64_t> user_start_time;
		while (proceed)
		{
			Sleep(10);
			std::scoped_lock lock(writers_mutex_);
			for (const auto& [key, value] : writers_)
			{
				if (!user_start_time.contains(key))
				{
					std::cout << "created the first value." << std::endl;
					user_start_time.insert({ key, 0 });
				}
				proto::UpdateOut update_out;
				std::multimap<std::int64_t, proto::TweetIn> tweets_map;
				std::string name = storage_->GetNameFromTocken(key);
				auto follows = storage_->GetFollowerList(name);
				for (const auto& follow : follows)
				{
					auto tweets = 
						storage_->GetTweetsFromNameTime(
							follow, user_start_time.at(key));
					for (const auto& tweet : tweets)
					{
						proto::TweetIn tweet_in;
						tweet_in.set_user(tweet.name);
						tweet_in.set_content(tweet.text);
						tweet_in.set_time(tweet.time);
						tweets_map.insert({ tweet.time, tweet_in });
					}
				}
				auto time = user_start_time.at(key);
				for (const auto& [time, value] : tweets_map)
				{
					std::cout << std::format(
						"go throught all the elements: [{}, {}]\n",
						time,
						value.user());
					if (value.time() >= time)
					{
						*update_out.add_tweets() = value;
						user_start_time.at(key) = 
							std::max(user_start_time.at(key), value.time());
					}
				}
				if (update_out.tweets_size())
				{
					for (auto& [key, writer] : writers_)
					{
						if (writer->Write(update_out))
						{
							std::cout << 
								std::format(
									"Send to user: {}.\n", key);
						}
						else
						{
							std::cerr << 
								std::format(
									"Couldn't send to user: {}!\n", key);
						}
					}
				}
			}
		}
	}

} // End namespace tweet.
