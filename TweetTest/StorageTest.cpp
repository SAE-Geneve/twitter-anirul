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
			auto maybe_tocken = storage_->Login("name", "right");
			EXPECT_FALSE(maybe_tocken.has_value());
		}
		{
			auto maybe_tocken = storage_->Register("name", "right");
			EXPECT_TRUE(maybe_tocken.has_value());
			EXPECT_TRUE(storage_->Logout(maybe_tocken.value()));
		}
		{
			auto maybe_tocken = storage_->Login("name", "wrong");
			EXPECT_FALSE(maybe_tocken.has_value());
		}
		{
			auto maybe_tocken = storage_->Login("name", "right");
			EXPECT_TRUE(maybe_tocken.has_value());
			EXPECT_TRUE(storage_->Logout(maybe_tocken.value()));
		}
	}

	TEST_F(StorageTest, TweetStorageTest)
	{
		ASSERT_FALSE(storage_);
		storage_ = std::make_shared<tweet::Storage>();
		ASSERT_TRUE(storage_);
		auto maybe_tocken = storage_->Register("name", "right");
		EXPECT_TRUE(maybe_tocken.has_value());
		EXPECT_TRUE(storage_->Tweet(maybe_tocken.value(), "Hello a tous!"));
		auto values = storage_->Show(maybe_tocken.value(), "name");
		EXPECT_EQ("Hello a tous!", values.begin()->text);
		EXPECT_EQ("name", values.begin()->name);
		EXPECT_TRUE(storage_->Logout(maybe_tocken.value()));
	}

	TEST_F(StorageTest, FollowStorageTest)
	{
		ASSERT_FALSE(storage_);
		storage_ = std::make_shared<tweet::Storage>();
		ASSERT_TRUE(storage_);
		{
			auto maybe_tocken = storage_->Register("name", "right");
			EXPECT_TRUE(maybe_tocken.has_value());
			EXPECT_TRUE(storage_->Tweet(maybe_tocken.value(), "Hello a tous!"));
			EXPECT_TRUE(storage_->Tweet(maybe_tocken.value(), "Re hello all!"));
			{
				auto values = storage_->Show(maybe_tocken.value(), "name");
				EXPECT_EQ(2, values.size());
			}
			EXPECT_TRUE(storage_->Logout(maybe_tocken.value()));
		}
		{
			auto maybe_tocken = storage_->Register("name2", "right2");
			EXPECT_TRUE(maybe_tocken.has_value());
			EXPECT_TRUE(storage_->Follow(maybe_tocken.value(), "name"));
			{
				auto values = storage_->Show(maybe_tocken.value(), "name");
				EXPECT_EQ(2, values.size());
			}
		}
	}

} // End namespace test.
