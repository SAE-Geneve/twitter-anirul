#pragma once

#include <gtest/gtest.h>
#include "../TweetLib/Storage.h"

namespace test {

	class StorageTest : public testing::Test
	{
	public:
		StorageTest() = default;

	protected:
		std::shared_ptr<tweet::Storage> storage_ = nullptr;
	};

} // End namespace test.