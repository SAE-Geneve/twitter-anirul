#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#endif
#include <grpcpp/create_channel.h>
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif
#include "../TweetLib/Client.h"
#include "Command.h"

int main(int ac, char** av) {
	try
	{
		std::string connection_str = "localhost:4242";
		if (ac == 2)
		{
			connection_str = av[1];
		}
		std::cout << "starting client..." << std::endl;
		auto c = std::make_shared<tweet::Client>(
			grpc::CreateChannel(
				connection_str,
				grpc::InsecureChannelCredentials()));
		client::Command command(c);
		command.Run();
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}