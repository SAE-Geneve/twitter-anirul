#include <future>
#include <grpcpp/create_channel.h>
#include "../TweetLib/Client.h"
#include "Command.h"

int main(int ac, char** av) {
	try
	{
		std::string connection_str = "localhost:4533";
		if (ac == 2)
		{
			connection_str = av[1];
		}
		std::cout << "starting client..." << std::endl;
		auto client = std::make_shared<tweet::Client>(
			grpc::CreateChannel(
				connection_str,
				grpc::InsecureChannelCredentials()));
		client::Command command(client);
		auto future = std::async(std::launch::async, [&command]
			{
				command.ProceedAsync();
			});
		command.Run();
		future.wait();
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}