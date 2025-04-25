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

	bool Storage::Tweet(std::int64_t tocken, const std::string& text)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = tocken_names_.find(tocken);
		if (it == tocken_names_.end())
		{
			return false;
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
		return true;
	}

	bool Storage::Follow(std::int64_t tocken, const std::string& name)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = tocken_names_.find(tocken);
		if (it == tocken_names_.end())
		{
			return false;
		}
		// Check if user name is not current one.
		if (name == it->second)
		{
			return false;
		}
		// Check if user name is existing.
		auto it_user = name_passes_.find(name);
		if (it_user == name_passes_.end())
		{
			return false;
		}
		// Check if this user is already registered in the DB.
		auto range = followers_.equal_range(it->second);
		if (std::find_if(range.first, range.second, [name](const auto& value)
		{
			return value.second == name;
		}) == range.second)
		{
			followers_.insert({ it->second, name });
			return true;
		}
		return false;
	}

	const std::vector<TweetValue> Storage::Show(
		std::int64_t tocken,
		const std::string& name)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		if (!tocken_names_.contains(tocken))
		{
			return {};
		}
		bool found = false;
		// Check if current user.
		auto it = tocken_names_.find(tocken);
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
			return ret;
		}
		// Not found so return empty vector.
		return {};
	}

	std::optional<std::int64_t> Storage::Login(
		const std::string& name, 
		const std::string& pass)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = name_passes_.find(name);
		if (it == name_passes_.end())
		{
			return std::nullopt;
		}
		// Check the password.
		if (pass == it->second)
		{
			auto tocken = GenerateTocken();
			tocken_names_.insert({ tocken, name });
			return tocken;
		}
		return std::nullopt;
	}

	bool Storage::Logout(std::int64_t tocken)
	{
		std::scoped_lock l(mutex_);
		auto it = tocken_names_.find(tocken);
		if (it == tocken_names_.end())
		{
			return false;
		}
		tocken_names_.erase(it);
		return true;
	}

	std::optional<std::int64_t> Storage::Register(
		const std::string& name, 
		const std::string& pass)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = name_passes_.find(name);
		if (it != name_passes_.end())
		{
			return std::nullopt;
		}
		name_passes_.insert({ name, pass });
		auto tocken = GenerateTocken();
		tocken_names_.insert({ tocken, name });
		return tocken;
	}

	std::int64_t Storage::GenerateTocken()
	{
		return dist_(engine_);
	}

	std::string Storage::GetNameFromTocken(std::int64_t tocken) const
	{
		return tocken_names_.at(tocken);
	}

	std::vector<std::string> Storage::GetFollowerList(
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

} // End namespace tweet.
