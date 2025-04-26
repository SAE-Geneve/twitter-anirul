#pragma once

#include <string>
#include <mutex>
#include <unordered_map>
#include <random>
#include "Status.h"

namespace tweet {

	struct TweetValue 
	{
		std::string name;
		std::string text;
		int64_t time;
	};

	class Storage 
	{
	public:
		Storage();
		Status<std::monostate> Tweet(
			std::int64_t token,
			const std::string& text);
		Status<std::monostate> Follow(
			std::int64_t token,
			const std::string& name);
		Status<std::vector<TweetValue>> Show(
			std::int64_t token,
			const std::string& name);
		Status<std::int64_t> Login(
			const std::string& name,
			const std::string& pass);
		Status<std::monostate> Logout(std::int64_t token);
		Status<std::int64_t> Register(
			const std::string& name,
			const std::string& pass);

	public:
		std::int64_t Generatetoken();
		std::string GetNameFromtoken(std::int64_t token) const;
		std::vector<std::string> GetSubscriptions(
			const std::string& name) const;
		std::vector<TweetValue> GetTweetsFromNameTime(
			const std::string& name,
			std::int64_t time_s) const;
		bool TokenContains(std::int64_t token) const;

	protected:
		std::mutex mutex_ = {}; 
		std::mt19937_64 engine_;
		std::uniform_int_distribution<std::int64_t> dist_;
		std::unordered_multimap<std::string, TweetValue> name_tweets_ = {};
		std::unordered_multimap<std::string, std::string> followers_ = {};
		std::unordered_map<std::int64_t, std::string> token_names_ = {};
		std::unordered_map<std::string, std::string> name_passes_ = {};
	};

} // End namespace tweet.