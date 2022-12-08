# Tweet Server server part of our project.

add_executable(TweetServer
    main.cpp
)

target_link_libraries(TweetServer
  PUBLIC
    GrpcLib
    TweetLib
)

set_property(TARGET TweetServer PROPERTY FOLDER "FinalProduct")