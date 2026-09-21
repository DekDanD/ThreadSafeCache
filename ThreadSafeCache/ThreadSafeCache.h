#pragma once

#include <iostream>
#include <unordered_map>
#include <chrono>
template <typename Key, typename Value>
class ThreadSafeCache
{
	struct Item {
		Value value;
		int ttl;
		std::chrono::steady_clock::time_point time;
	};
	std::unordered_map<Key, Item> cache;
	int max_size;
public:
	void set(const Key& key, Value value, int ttl_ms);
	std::shared_ptr<Value> get(const Key& key);
	void remove(const Key& key);
};