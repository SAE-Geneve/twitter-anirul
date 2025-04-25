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
		std::map<int64_t, int64_t> last_seen;  // token -> last‐seen tweet time

		while (running_)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			
			// Make a local copy of the writers so we don’t hold the lock while writing
			std::vector<std::pair<int64_t, grpc::ServerWriter<proto::UpdateOut>*>> tasks;
			{
				std::scoped_lock lock(writers_mutex_);
				for (auto& [token, writer] : writers_)
				{
					tasks.emplace_back(token, writer);
				}
			}

			// Process each client’s stream independently
			for (auto& [token, writer] : tasks)
			{
				// 1) Figure out the cutoff
				int64_t cutoff = 0;
				if (auto it = last_seen.find(token); it != last_seen.end())
				{
					cutoff = it->second;
				}

				// 2) Gather any new tweets from *whom this user follows*
				proto::UpdateOut response;
				std::multimap<int64_t, proto::TweetIn> sorted;
				std::string me = storage_->GetNameFromTocken(token);
				// make sure this returns *who I follow*
				auto following = storage_->GetSubscriptions(me);
				std::cout 
					<< "[proc] token=" 
					<< token 
					<< " follows=" 
					<< following.size() 
					<< " users\n";
				for (auto& followed_user : following)
				{
					std::cout << "   -> " << followed_user << "\n";
					auto fresh = storage_->GetTweetsFromNameTime(followed_user, cutoff);
					std::cout 
						<< "       tweets from " 
						<< followed_user 
						<< ": " 
						<< fresh.size()
						<< "\n";
					for (auto& tw : fresh)
					{
						proto::TweetIn ti;
						ti.set_user(tw.name);
						ti.set_content(tw.text);
						ti.set_time(tw.time);
						sorted.insert({ tw.time, ti });
					}
				}

				// 3) Keep only strictly newer tweets and bump the cutoff
				std::cout << "[proc] token=" << token
					<< " last_seen before=" << cutoff << "\n";
				std::cout << "[proc] token=" << token
					<< " total gathered tweets=" << sorted.size() << "\n";
				for (auto& [ts, ti] : sorted) 
				{
					if (ts > cutoff)
					{
						*response.add_tweets() = ti;
						cutoff = ts;
					}
				}

				// 4) If there was anything new, push it to _this_ client
				if (response.tweets_size() > 0)
				{
					if (!writer->Write(response))
					{
						// client probably disconnected; clean up
						std::scoped_lock lock(writers_mutex_);
						writers_.erase(token);
						last_seen.erase(token);
					}
					else
					{
						last_seen[token] = cutoff;
					}
				}
			}
		}
	}

} // End namespace tweet.
