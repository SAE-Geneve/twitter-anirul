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
		EXPECT_FALSE(storage_->Login("context", "name", "right"));
		EXPECT_TRUE(storage_->Register("context", "name", "right"));
		EXPECT_TRUE(storage_->Logout("context"));
		EXPECT_FALSE(storage_->Login("context", "name", "wrong"));
		EXPECT_TRUE(storage_->Login("context", "name", "right"));
		EXPECT_TRUE(storage_->Logout("context"));
	}

	TEST_F(StorageTest, TweetStorageTest)
	{
		ASSERT_FALSE(storage_);
		storage_ = std::make_shared<tweet::Storage>();
		ASSERT_TRUE(storage_);
		EXPECT_TRUE(storage_->Register("context", "name", "right"));
		EXPECT_TRUE(storage_->Tweet("context", "Hello a tous!"));
		auto values = storage_->Show("context", "name");
		EXPECT_EQ("Hello a tous!", values.begin()->text);
		EXPECT_EQ("name", values.begin()->name);
		EXPECT_TRUE(storage_->Logout("context"));
	}

	TEST_F(StorageTest, FollowStorageTest)
	{
		ASSERT_FALSE(storage_);
		storage_ = std::make_shared<tweet::Storage>();
		ASSERT_TRUE(storage_);
		EXPECT_TRUE(storage_->Register("context", "name", "right"));
		EXPECT_TRUE(storage_->Tweet("context", "Hello a tous!"));
		EXPECT_TRUE(storage_->Tweet("context", "Re hello all!"));
		{
			auto values = storage_->Show("context", "name");
			EXPECT_EQ(2, values.size());
		}
		EXPECT_TRUE(storage_->Logout("context"));
		EXPECT_TRUE(storage_->Register("context2", "name2", "right2"));
		EXPECT_TRUE(storage_->Follow("context2", "name"));
		{
			auto values = storage_->Show("context2", "name");
			EXPECT_EQ(2, values.size());
		}
	}

} // End namespace test.
