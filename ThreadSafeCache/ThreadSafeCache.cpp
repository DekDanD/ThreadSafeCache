#include "ThreadSafeCache.h"
#include <iostream>
#include <unordered_map>
#include <gtest/gtest.h>
#include <chrono>

template <typename Key, typename Value>
inline std::shared_ptr<Value> ThreadSafeCache<Key, Value>::get(const Key& key)
{
	if (this->max_size == 0) return nullptr;
	std::lock_guard<std::mutex> lock(this->mtx);
	auto finding = this->cache.find(key);
	if (finding != this->cache.end())
	{
		auto iterator = finding->second;
		if (iterator->second.time <= std::chrono::steady_clock::now())
		{
			this->cache.erase(finding);
			this->cache_list.erase(iterator);
		}
		else
		{
			iterator->second.time = std::chrono::steady_clock::now() + std::chrono::milliseconds(iterator->second.ttl);
			this->cache_list.splice(this->cache_list.end(), this->cache_list, iterator);
			return iterator->second.value;
		}
	}
	return nullptr;
}

template <typename Key, typename Value>
inline void ThreadSafeCache<Key, Value>::remove(const Key& key)
{
	if (this->max_size == 0) return;
	std::lock_guard<std::mutex> lock(this->mtx);
	auto finding = this->cache.find(key);
	if (finding == this->cache.end())
		return;
	this->cache_list.erase(finding->second);
	this->cache.erase(finding);
}

template <typename Key, typename Value>
inline void ThreadSafeCache<Key, Value>::set(const Key& key, Value value, int ttl_ms)
{
	if (this->max_size == 0) return;
	std::lock_guard<std::mutex> lock(this->mtx);
	auto finding = this->cache.find(key);
	Item item{ std::make_shared<Value>(std::move(value)), ttl_ms, std::chrono::steady_clock::now() + std::chrono::milliseconds(ttl_ms) };
	if (finding != this->cache.end())
	{
		auto iterator = finding->second;
		iterator->second = std::move(item);
		this->cache_list.splice(this->cache_list.end(), this->cache_list, iterator);
	}
	else
	{
		if (this->cache_list.size() == this->max_size)
		{
			this->cache.erase(this->cache_list.front().first);
			this->cache_list.pop_front();
		}
		this->cache_list.push_back(std::make_pair(key, std::move(item)));
		this->cache[key] = std::prev(this->cache_list.end());
	}
}
int main()
{
	testing::InitGoogleTest();
	RUN_ALL_TESTS();
}
