#include "Client.h"

namespace tweet {

	proto::TweetOut Client::Tweet(const proto::TweetIn in)
	{
		proto::TweetOut out;
		grpc::ClientContext context;
		stub_->Tweet(&context, in, &out);
		return out;
	}

	proto::FollowOut Client::Follow(const proto::FollowIn in)
	{
		proto::FollowOut out;
		grpc::ClientContext context;
		stub_->Follow(&context, in, &out);
		return out;
	}

	proto::ShowOut Client::Show(const proto::ShowIn in)
	{
		proto::ShowOut out;
		grpc::ClientContext context;
		stub_->Show(&context, in, &out);
		return out;
	}

	proto::LoginOut Client::Login(const proto::LoginIn in)
	{
		proto::LoginOut out;
		grpc::ClientContext context;
		stub_->Login(&context, in, &out);
		return out;
	}

	proto::LogoutOut Client::Logout(const proto::LogoutIn in)
	{
		proto::LogoutOut out;
		grpc::ClientContext context;
		stub_->Logout(&context, in, &out);
		return out;
	}

	proto::RegisterOut Client::Register(const proto::RegisterIn in)
	{
		proto::RegisterOut out;
		grpc::ClientContext context;
		stub_->Register(&context, in, &out);
		return out;
	}

} // End namespace tweet.
