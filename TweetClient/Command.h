#pragma once

#include <memory>
#include "../TweetLib/Client.h"

namespace client {

	class Command
	{
	public:
		// Create a command with a client.
		Command(std::shared_ptr<tweet::Client>& client) : client_(client) {}
		// Run until the end (this take the hand on the main program).
		void Run();
		void ProceedAsync();

	protected:
		bool LoginRegisterQuit();
		bool ShowFollowTweetQuit();

	protected:
		bool Tweet();
		bool Follow();
		bool Show();
		bool Login();
		bool LogOut();
		bool Register();

	protected:
		std::mutex client_mutex_;
		std::optional<std::int64_t> token_ = std::nullopt;
		std::shared_ptr<tweet::Client> client_;
	};

} // End namespace client.
