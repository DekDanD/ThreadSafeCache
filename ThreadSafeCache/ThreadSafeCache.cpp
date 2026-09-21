#include "ThreadSafeCache.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
using namespace std;

template <typename Key, typename Value>
inline shared_ptr<Value> ThreadSafeCache<Key, Value>::get(const Key& key)
{
	auto finding = this->cache.find(key);
	if (finding != this->cache.end())
		if (finding->second.time <= std::chrono::steady_clock::now())
			this->cache.erase(finding);
		else
		{
			finding->second.time = std::chrono::steady_clock::now() + finding->second.ttl;
			return std::make_shared<Value>(finding->second.value);
		}
	return nullptr;
}

template <typename Key, typename Value>
inline void ThreadSafeCache<Key, Value>::remove(const Key& key)
{

}

template <typename Key, typename Value>
inline void ThreadSafeCache<Key, Value>::set(const Key& key, Value value, int ttl_ms)
{

}

int main() 
{

}