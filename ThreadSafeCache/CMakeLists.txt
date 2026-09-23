add_executable(ThreadSafeCache 
    test.cpp
    ThreadSafeCache.h
)

target_include_directories(ThreadSafeCache PUBLIC .)

target_link_libraries(ThreadSafeCache PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ThreadSafeCache DISCOVERY_MODE PRE_TEST)