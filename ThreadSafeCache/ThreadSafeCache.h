#pragma once

#include <iostream>
#include <unordered_map>
#include <utility>
#include <mutex>
#include <chrono>
template <typename Key, typename Value>
class ThreadSafeCache
{
	struct Item {
		std::shared_ptr<Value> value;
		int ttl;
		std::chrono::steady_clock::time_point time;
	};
	std::list<std::pair<Key, Item>> cache_list;
	std::unordered_map<Key, typename std::list<std::pair<Key, Item>>::iterator> cache;
	size_t max_size;
	mutable std::mutex mtx;
public:
	ThreadSafeCache(size_t capacity) : max_size(capacity) {};
	void set(const Key& key, Value value, int ttl_ms);
	std::shared_ptr<Value> get(const Key& key);
	void remove(const Key& key);
};