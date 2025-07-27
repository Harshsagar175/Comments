add_library(cdn_common
    common.cpp
    utils.cpp
    logger.cpp
    config.cpp
)

target_link_libraries(cdn_common
    cpprestsdk::cpprest
    Threads::Threads
)

target_include_directories(cdn_common PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)