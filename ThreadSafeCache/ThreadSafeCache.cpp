#include "ThreadSafeCache.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
using namespace std;

template <typename Key, typename Value>
inline shared_ptr<Value> ThreadSafeCache<Key, Value>::get(const Key& key)
{
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
