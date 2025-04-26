#include "Storage.h"
#include <algorithm>
#include <iterator>
#include <chrono>
#include <map>

namespace tweet {

	Storage::Storage() : 
		engine_(std::random_device{}()),
		dist_(std::numeric_limits<std::int64_t>::min(),
			std::numeric_limits<std::int64_t>::max())
	{
	}

	Status<std::monostate> Storage::Tweet(
		std::int64_t token, const std::string& text)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = token_names_.find(token);
		if (it == token_names_.end())
		{
			return { StatusEnum::UNAUTHENTICATED, "Invalid token" };
		}
		// Create a tweet with user name current.
		TweetValue tv{};
		tv.name = it->second;
		auto now = std::chrono::system_clock::now();
		auto now_s = std::chrono::time_point_cast<std::chrono::seconds>(now);
		auto value = now_s.time_since_epoch();
		tv.time = value.count();
		tv.text = text;
		name_tweets_.insert({ it->second, tv });
		return { StatusEnum::OK };
	}

	Status<std::monostate> Storage::Follow(
		std::int64_t token, const std::string& name)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = token_names_.find(token);
		if (it == token_names_.end())
		{
			return { StatusEnum::UNAUTHENTICATED, "Invalid token" };
		}
		// Check if user name is not current one.
		if (name == it->second)
		{
			return { StatusEnum::INVALID_ARGUMENT, "Name doesn't correspond" };
		}
		// Check if user name is existing.
		auto it_user = name_passes_.find(name);
		if (it_user == name_passes_.end())
		{
			return { StatusEnum::INVALID_ARGUMENT, "Invalid name" };
		}
		// Check if this user is already registered in the DB.
		auto range = followers_.equal_range(it->second);
		if (std::find_if(range.first, range.second, [name](const auto& value)
		{
			return value.second == name;
		}) == range.second)
		{
			followers_.insert({ it->second, name });
			return { StatusEnum::OK };
		}
		return { StatusEnum::INTERNAL, "Internal error?" };
	}

	Status<std::vector<TweetValue>> Storage::Show(
		std::int64_t token,
		const std::string& name)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		if (!token_names_.contains(token))
		{
			return { 
				StatusEnum::UNAUTHENTICATED, "You are not authenticated?"};
		}
		bool found = false;
		// Check if current user.
		auto it = token_names_.find(token);
		if (it->second == name)
		{
			found = true;
		}
		else
		{
			// Check if the follower is in.
			auto range = followers_.equal_range(it->second);
			found = std::find_if(
				range.first,
				range.second,
				[name](const auto& value)
			{
				return value.second == name;
			}) != range.second;
		}
		// If the follower is in or this is current user.
		if (found)
		{
			// Transform the current range into a vector.
			std::vector<TweetValue> ret{};
			auto tweet_range = name_tweets_.equal_range(name);
			std::transform(
				tweet_range.first,
				tweet_range.second,
				std::back_inserter(ret),
				[](const auto& value)
				{
					return value.second;
				});
			return { StatusEnum::OK, "", ret };
		}
		// Not found so return empty vector.
		return { StatusEnum::OK, "", {} };
	}

	Status<std::int64_t> Storage::Login(
		const std::string& name, 
		const std::string& pass)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = name_passes_.find(name);
		if (it == name_passes_.end())
		{
			return { StatusEnum::ALREADY_EXISTS, "You are already login" };
		}
		// Check the password.
		if (pass == it->second)
		{
			auto token = Generatetoken();
			token_names_.insert({ token, name });
			return { StatusEnum::OK, "", token };
		}
		return { StatusEnum::UNAUTHENTICATED, "Invalid credentials" };
	}

	Status<std::monostate> Storage::Logout(std::int64_t token)
	{
		std::scoped_lock l(mutex_);
		auto it = token_names_.find(token);
		if (it == token_names_.end())
		{
			return { StatusEnum::INVALID_ARGUMENT, "Invalid token" };
		}
		token_names_.erase(it);
		return { StatusEnum::OK };
	}

	Status<std::int64_t> Storage::Register(
		const std::string& name, 
		const std::string& pass)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = name_passes_.find(name);
		if (it != name_passes_.end())
		{
			return { StatusEnum::ALREADY_EXISTS, "Name already taken" };
		}
		name_passes_.insert({ name, pass });
		auto token = Generatetoken();
		token_names_.insert({ token, name });
		return { StatusEnum::OK, "", token };
	}

	std::int64_t Storage::Generatetoken()
	{
		return dist_(engine_);
	}

	std::string Storage::GetNameFromtoken(std::int64_t token) const
	{
		return token_names_.at(token);
	}

	std::vector<std::string> Storage::GetSubscriptions(
		const std::string& name) const
	{
		std::vector<std::string> out;
		out.push_back(name);
		auto range = followers_.equal_range(name);
		for (auto it = range.first; it != range.second; ++it)
		{
			out.push_back(it->second);
		}
		return out;
	}

	std::vector<TweetValue> Storage::GetTweetsFromNameTime(
		const std::string& name,
		std::int64_t time_s) const
	{
		std::vector<TweetValue> tweets;
		auto range = name_tweets_.equal_range(name);
		for (auto it = range.first; it != range.second; ++it)
		{
			if (it->second.time > time_s)
			{
				tweets.push_back(it->second);
			}
		}
		return tweets;
	}

	bool Storage::TokenContains(std::int64_t token) const
	{
		return token_names_.contains(token);
	}

} // End namespace tweet.
