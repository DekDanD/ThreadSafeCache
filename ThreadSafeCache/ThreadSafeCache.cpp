#include "ThreadSafeCache.h"
#include <iostream>
#include <unordered_map>
#include <chrono>

template <typename Key, typename Value>
inline std::shared_ptr<Value> ThreadSafeCache<Key, Value>::get(const Key& key)
{
	auto finding = this->cache.find(key);
	if (finding != this->cache.end())
		if (finding->second.time <= std::chrono::steady_clock::now())
			this->cache.erase(finding);
		else
		{
			finding->second.time = std::chrono::steady_clock::now() + std::chrono::milliseconds(finding->second.ttl);
			return std::make_shared<Value>(finding->second.value);
		}
	return nullptr;
}

template <typename Key, typename Value>
inline void ThreadSafeCache<Key, Value>::remove(const Key& key)
{
	this->cache.erase(key);
}

template <typename Key, typename Value>
inline void ThreadSafeCache<Key, Value>::set(const Key& key, Value value, int ttl_ms)
{
	Item item{ value, ttl_ms, std::chrono::steady_clock::now() + std::chrono::milliseconds(ttl_ms) };
	this->cache[key] = item;
}

int main() 
{

}