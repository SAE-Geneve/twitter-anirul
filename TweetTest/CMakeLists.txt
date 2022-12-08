# Tweet test this is the test for the twitter project.

add_executable(TweetTest
    main.cpp
    StorageTest.h
    StorageTest.cpp
)

target_link_libraries(TweetTest
  PUBLIC
    GrpcLib
    TweetLib
    GTest::gtest
    GTest::gtest_main
    GTest::gmock 
    GTest::gmock_main
)

set_property(TARGET TweetTest PROPERTY FOLDER "Test")