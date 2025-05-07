#include "Client.h"

namespace tweet {

	Status<proto::TweetOut> Client::Tweet(const proto::TweetIn in)
	{
		proto::TweetOut out;
		grpc::ClientContext context;
		auto grpc_status = stub_->Tweet(&context, in, &out);
		Status<proto::TweetOut> tweet_status = 
			StatusGrpcToStatusStorage<proto::TweetOut>(grpc_status);
		tweet_status.value = std::move(out);
		return tweet_status;
	}

	Status<proto::FollowOut> Client::Follow(const proto::FollowIn in)
	{
		proto::FollowOut out;
		grpc::ClientContext context;
		auto grpc_status = stub_->Follow(&context, in, &out);
		Status<proto::FollowOut> follow_status =
			StatusGrpcToStatusStorage<proto::FollowOut>(grpc_status);
		follow_status.value = std::move(out);
		return follow_status;
	}

	Status<proto::ShowOut> Client::Show(const proto::ShowIn in)
	{
		proto::ShowOut out;
		grpc::ClientContext context;
		auto grpc_status = stub_->Show(&context, in, &out);
		Status<proto::ShowOut> show_status =
			StatusGrpcToStatusStorage<proto::ShowOut>(grpc_status);
		show_status.value = std::move(out);
		return show_status;
	}

	Status<proto::LoginOut> Client::Login(const proto::LoginIn in)
	{
		proto::LoginOut out;
		grpc::ClientContext context;
		auto grpc_status = stub_->Login(&context, in, &out);
		Status<proto::LoginOut> login_status =
			StatusGrpcToStatusStorage<proto::LoginOut>(grpc_status);
		login_status.value = std::move(out);
		return login_status;
	}

	Status<proto::LogoutOut> Client::Logout(const proto::LogoutIn in)
	{
		proto::LogoutOut out;
		grpc::ClientContext context;
		auto grpc_status = stub_->Logout(&context, in, &out);
		Status<proto::LogoutOut> logout_status =
			StatusGrpcToStatusStorage<proto::LogoutOut>(grpc_status);
		logout_status.value = std::move(out);
		return logout_status;
	}

	Status<proto::RegisterOut> Client::Register(const proto::RegisterIn in)
	{
		proto::RegisterOut out;
		grpc::ClientContext context;
		auto grpc_status = stub_->Register(&context, in, &out);
		Status<proto::RegisterOut> register_status =
			StatusGrpcToStatusStorage<proto::RegisterOut>(grpc_status);
		register_status.value = std::move(out);
		return register_status;
	}

	std::unique_ptr<grpc::ClientReader<proto::UpdateOut>>
	Client::Update(grpc::ClientContext* context, const proto::UpdateIn& in)
	{
		// stub_->Update returns a ClientReader<UpdateOut> for a stream of responses
		return stub_->Update(context, in);
	}

} // End namespace tweet.
