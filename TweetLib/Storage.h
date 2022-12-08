#pragma once

#include <string>
#include <mutex>
#include <unordered_map>

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
		bool Tweet(
			const std::string& context,
			const std::string& text);
		bool Follow(
			const std::string& context,
			const std::string& name);
		const std::vector<TweetValue> Show(
			const std::string& context, 
			const std::string& name);
		bool Login(
			const std::string& context,
			const std::string& name,
			const std::string& pass);
		bool Logout(const std::string& context);
		bool Register(
			const std::string& context,
			const std::string& name,
			const std::string& pass);

	protected:
		std::mutex mutex_ = {};
		std::unordered_multimap<std::string, TweetValue> name_tweets_ = {};
		std::unordered_multimap<std::string, std::string> followers_ = {};
		std::unordered_map<std::string, std::string> context_names_ = {};
		std::unordered_map<std::string, std::string> name_passes_ = {};
	};

} // End namespace tweet.