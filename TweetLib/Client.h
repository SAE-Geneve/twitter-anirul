#pragma once

#include <memory>
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#endif
#include <grpcpp/channel.h>
#include "../GrpcLib/Tweet.pb.h"
#include "../GrpcLib/Tweet.grpc.pb.h"
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif

namespace tweet {

	class Client
	{
	public:
		Client(std::unique_ptr<proto::TweetService::Stub> stub) :
			stub_(std::move(stub)) {}
		Client(std::shared_ptr<grpc::Channel> channel) :
			stub_(proto::TweetService::NewStub(channel)) {}
		proto::TweetOut Tweet(const proto::TweetIn in);
		proto::FollowOut Follow(const proto::FollowIn in);
		proto::ShowOut Show(const proto::ShowIn in);
		proto::LoginOut Login(const proto::LoginIn in);
		proto::LogoutOut Logout(const proto::LogoutIn in);
		proto::RegisterOut Register(const proto::RegisterIn in);

	protected:
		std::unique_ptr<proto::TweetService::Stub> stub_;
	};

} // End namespace tweet
