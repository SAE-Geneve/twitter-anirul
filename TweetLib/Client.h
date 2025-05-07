#pragma once

#include <memory>
#include <grpcpp/channel.h>
#include "../GrpcLib/Tweet.pb.h"
#include "../GrpcLib/Tweet.grpc.pb.h"
#include "../TweetLib/StatusHelper.h"

namespace tweet {

	class Client
	{
	public:
		Client(std::unique_ptr<proto::TweetService::Stub> stub) :
			stub_(std::move(stub)) {}
		Client(std::shared_ptr<grpc::Channel> channel) :
			stub_(proto::TweetService::NewStub(channel)) {}
		Status<proto::TweetOut> Tweet(const proto::TweetIn in);
		Status<proto::FollowOut> Follow(const proto::FollowIn in);
		Status<proto::ShowOut> Show(const proto::ShowIn in);
		Status<proto::LoginOut> Login(const proto::LoginIn in);
		Status<proto::LogoutOut> Logout(const proto::LogoutIn in);
		Status<proto::RegisterOut> Register(const proto::RegisterIn in);
		std::unique_ptr<grpc::ClientReader<proto::UpdateOut>> Update(
			grpc::ClientContext* context, const proto::UpdateIn& in);

	protected:
		std::unique_ptr<proto::TweetService::Stub> stub_;
	};

} // End namespace tweet
