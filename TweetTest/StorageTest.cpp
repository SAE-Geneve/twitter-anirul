#include "StorageTest.h"

namespace test {

	TEST_F(StorageTest, CreateStorageTest)
	{
		EXPECT_FALSE(storage_);
		storage_ = std::make_shared<tweet::Storage>();
		EXPECT_TRUE(storage_);
	}

	TEST_F(StorageTest, LoginStorageTest)
	{
		ASSERT_FALSE(storage_);
		storage_ = std::make_shared<tweet::Storage>();
		ASSERT_TRUE(storage_);
		{
			auto status_token = storage_->Login("name", "right");
			EXPECT_FALSE(status_token.Ok());
		}
		{
			auto status_token = storage_->Register("name", "right");
			EXPECT_TRUE(status_token.Ok());
			EXPECT_TRUE(storage_->Logout(status_token.value).Ok());
		}
		{
			auto status_token = storage_->Login("name", "wrong");
			EXPECT_FALSE(status_token.Ok());
		}
		{
			auto status_token = storage_->Login("name", "right");
			EXPECT_TRUE(status_token.Ok());
			EXPECT_TRUE(storage_->Logout(status_token.value).Ok());
		}
	}

	TEST_F(StorageTest, TweetStorageTest)
	{
		ASSERT_FALSE(storage_);
		storage_ = std::make_shared<tweet::Storage>();
		ASSERT_TRUE(storage_);
		auto status_token = storage_->Register("name", "right");
		EXPECT_TRUE(status_token.Ok());
		EXPECT_TRUE(storage_->Tweet(status_token.value, "Hello a tous!").Ok());
		auto status_values = storage_->Show(status_token.value, "name");
		EXPECT_TRUE(status_values.Ok());
		EXPECT_EQ("Hello a tous!", status_values.value.begin()->text);
		EXPECT_EQ("name", status_values.value.begin()->name);
		EXPECT_TRUE(storage_->Logout(status_token.value).Ok());
	}

	TEST_F(StorageTest, FollowStorageTest)
	{
		ASSERT_FALSE(storage_);
		storage_ = std::make_shared<tweet::Storage>();
		ASSERT_TRUE(storage_);
		{
			auto status_token = storage_->Register("name", "right");
			EXPECT_TRUE(status_token.Ok());
			EXPECT_TRUE(storage_->Tweet(status_token.value, "Hello a tous!").Ok());
			EXPECT_TRUE(storage_->Tweet(status_token.value, "Re hello all!").Ok());
			{
				auto status_values = storage_->Show(status_token.value, "name");
				EXPECT_TRUE(status_values.Ok());
				EXPECT_EQ(2, status_values.value.size());
			}
			EXPECT_TRUE(storage_->Logout(status_token.value).Ok());
		}
		{
			auto status_token = storage_->Register("name2", "right2");
			EXPECT_TRUE(status_token.Ok());
			EXPECT_TRUE(storage_->Follow(status_token.value, "name").Ok());
			{
				auto status_values = storage_->Show(status_token.value, "name");
				EXPECT_TRUE(status_values.Ok());
				EXPECT_EQ(2, status_values.value.size());
			}
		}
	}

} // End namespace test.
