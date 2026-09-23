#include "ThreadSafeCache.h"
#include <iostream>
#include <unordered_map>
#include <gtest/gtest.h>
#include <chrono>

template <typename Key, typename Value>
inline std::shared_ptr<Value> ThreadSafeCache<Key, Value>::get(const Key& key)
{
	std::lock_guard<std::mutex> lock(this->mtx);
	auto finding = this->cache.find(key);
	if (finding != this->cache.end())
		if (finding->second->second.time <= std::chrono::steady_clock::now())
		{
			this->cache_list.erase(finding->second);
			this->cache.erase(finding);
		}
		else
		{
			finding->second->second.time = std::chrono::steady_clock::now() + std::chrono::milliseconds(finding->second->second.ttl);
			this->cache_list.splice(this->cache_list.end(), this->cache_list, finding->second);
			return finding->second->second.value;
		}
	return nullptr;
}

template <typename Key, typename Value>
inline void ThreadSafeCache<Key, Value>::remove(const Key& key)
{
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
	std::lock_guard<std::mutex> lock(this->mtx);
	auto finding = this->cache.find(key);
	Item item{ std::make_shared<Value>(value), ttl_ms, std::chrono::steady_clock::now() + std::chrono::milliseconds(ttl_ms) };
	if (finding != this->cache.end())
	{
		finding->second->second = item;
		this->cache_list.splice(this->cache_list.end(), this->cache_list, finding->second);
	}
	else
	{
		if (this->cache_list.size() == this->max_size)
		{
			this->cache.erase(this->cache_list.front().first);
			this->cache_list.pop_front();
		}
		this->cache_list.push_back(std::make_pair(key, item));
		this->cache[key] = std::prev(this->cache_list.end());
	}

}

int main()
{
	testing::InitGoogleTest();
	RUN_ALL_TESTS();
}