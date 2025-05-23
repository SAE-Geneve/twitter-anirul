#pragma once

#include "../GrpcLib/Tweet.pb.h"
#include "../GrpcLib/Tweet.grpc.pb.h"
#include "../TweetLib/StatusHelper.h"
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
		grpc::Status Update(
			grpc::ServerContext* context,
			const proto::UpdateIn* update_in,
			grpc::ServerWriter<proto::UpdateOut>* update_out_writer) override;
		void ProceedAsync();

	private:
		const std::shared_ptr<Storage> storage_;
		std::map<std::int64_t, grpc::ServerWriter<proto::UpdateOut>*> writers_;
		std::mutex writers_mutex_;
		bool running_ = true;
	};

}