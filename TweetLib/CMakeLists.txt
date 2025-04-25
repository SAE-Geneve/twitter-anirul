# This is the library to make tweeting possible.

add_library(TweetLib
  STATIC
    Client.h
    Client.cpp
    Server.h
    Server.cpp
    Status.h
    StatusHelper.h
    Storage.h
    Storage.cpp
)

target_link_libraries(TweetLib
  PUBLIC
    GrpcLib
)

set_property(TARGET TweetLib PROPERTY FOLDER "Library")