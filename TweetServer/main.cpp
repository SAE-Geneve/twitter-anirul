#include <iostream>
#include <future>
#include "../TweetLib/Server.h"
#include "../TweetLib/Storage.h"
#include <grpcpp/server_builder.h>
#include <grpcpp/security/credentials.h>

int main(int ac, char** av)
{
	std::cout << "starting server..." << std::endl;
	std::string server_address{ "0.0.0.0:4533" };
	auto storage = std::make_shared<tweet::Storage>();
	tweet::Server session{ storage };
	grpc::ServerBuilder builder{};
	builder.AddListeningPort(
		server_address,
		grpc::InsecureServerCredentials());
	builder.RegisterService(&session);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	auto future = std::async(std::launch::async, [&session]
		{
			session.ProceedAsync();
		});
	server->Wait();
	future.wait();
	return 0;
}