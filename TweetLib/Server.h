#pragma once

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#endif
#include "../GrpcLib/Tweet.pb.h"
#include "../GrpcLib/Tweet.grpc.pb.h"
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif
#include "../TweetLib/Storage.h"

namespace tweet {

	class Server final : public proto::TweetService::Service
	{
	public:
		Server(const std::shared_ptr<Storage> storage) :
			storage_(storage) {}
		grpc::Status Tweet(
			grpc::ServerContext* context,
			const proto::TweetIn* request,
			proto::TweetOut* response) override;
		grpc::Status Follow(
			grpc::ServerContext* context,
			const proto::FollowIn* request,
			proto::FollowOut* response) override;
		grpc::Status Show(
			grpc::ServerContext* context,
			const proto::ShowIn* request,
			proto::ShowOut* response) override;
		grpc::Status Login(
			grpc::ServerContext* context,
			const proto::LoginIn* request,
			proto::LoginOut* response) override;
		grpc::Status Logout(
			grpc::ServerContext* context,
			const proto::LogoutIn* request,
			proto::LogoutOut* response) override;
		grpc::Status Register(
			grpc::ServerContext* context,
			const proto::RegisterIn* request,
			proto::RegisterOut* response) override;

	private:
		const std::shared_ptr<Storage> storage_;
	};

}