# Tweet Client part.

add_executable(TweetClient
    main.cpp
    Command.h
    Command.cpp
)

target_link_libraries(TweetClient
  PUBLIC
    GrpcLib
    TweetLib
)

set_property(TARGET TweetClient PROPERTY FOLDER "FinalProduct")