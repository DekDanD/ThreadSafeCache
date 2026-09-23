#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <random>
#include "ThreadSafeCache.h"

TEST(ThreadSafeCacheTest, BasicSetAndGet) 
{
    ThreadSafeCache<int, std::string> cache(3);

    cache.set(1, "one", 1000);
    cache.set(2, "two", 1000);

    auto val1 = cache.get(1);
    ASSERT_NE(val1, nullptr);
    EXPECT_EQ(*val1, "one");

    auto val2 = cache.get(2);
    ASSERT_NE(val2, nullptr);
    EXPECT_EQ(*val2, "two");

    EXPECT_EQ(cache.get(3), nullptr);
}


TEST(ThreadSafeCacheTest, LruEviction) 
{
    ThreadSafeCache<int, std::string> cache(2);

    cache.set(1, "one", 5000);
    cache.set(2, "two", 5000);

    cache.get(1);

    cache.set(3, "three", 5000);

    EXPECT_NE(cache.get(1), nullptr); 
    EXPECT_EQ(cache.get(2), nullptr);  
    EXPECT_NE(cache.get(3), nullptr);
}


TEST(ThreadSafeCacheTest, RemoveElement) 
{
    ThreadSafeCache<int, std::string> cache(5);
    cache.set(1, "one", 5000);

    EXPECT_NE(cache.get(1), nullptr);
    cache.remove(1);
    EXPECT_EQ(cache.get(1), nullptr);
}


TEST(ThreadSafeCacheTest, TtlExpiration)
{
    ThreadSafeCache<int, std::string> cache(5);

    cache.set(1, "value", 50);

    EXPECT_NE(cache.get(1), nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(cache.get(1), nullptr);
}

TEST(ThreadSafeCacheTest, RaceConditionStressTest) {
    ThreadSafeCache<int, int> cache(100);

    const int threads_number = 12; // Должно быть кратно 3 для логики ниже
    const int calls_in_thread = 120;
    std::vector<std::thread> threads;
    threads.reserve(threads_number);

    for (int i = 0; i < threads_number; i += 3) {
        // Поток на запись (Set)
        threads.emplace_back([&cache, calls_in_thread]() {
            // У каждого потока должен быть свой локальный генератор
            std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<> dis_key(1, 100);
            std::uniform_int_distribution<> dis_ttl(100, 1000);

            for (int j = 0; j < calls_in_thread; j++) {
                int random_key = dis_key(gen);
                int random_ttl = dis_ttl(gen);
                cache.set(random_key, random_key, random_ttl);
            }
            });

        // Поток на чтение (Get)
        threads.emplace_back([&cache, calls_in_thread]() {
            for (int j = 0; j < calls_in_thread; j++) {
                auto value = cache.get((j % 100) + 1);
            }
            });

        // Поток на удаление (Remove)
        threads.emplace_back([&cache, calls_in_thread]() {
            for (int j = 0; j < calls_in_thread; j++) {
                cache.remove((j % 100) + 1);
            }
            });
    }

    // Обязательное ожидание завершения всех потоков
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    SUCCEED();
}