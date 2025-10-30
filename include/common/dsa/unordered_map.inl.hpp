#pragma once

#include "unordered_map.hpp"

#include <cmath>
#include "utility.hpp"

namespace dsa {

template <typename KEY_T, typename VAL_T, typename HASH_F>
unordered_map<KEY_T, VAL_T, HASH_F>::unordered_map(const unordered_map<KEY_T, VAL_T, HASH_F>& other) :
	m_size(other.m_size),
	m_buckets(other.m_buckets),
	m_table(new tagged_entry[m_buckets]) {

	for (size_t i = 0; i < m_buckets; ++i) {
		m_table[i] = other.m_table[i];
	}
}

template <typename KEY_T, typename VAL_T, typename HASH_F>
unordered_map<KEY_T, VAL_T, HASH_F>& unordered_map<KEY_T, VAL_T, HASH_F>::operator=(unordered_map<KEY_T, VAL_T, HASH_F> rhs) {
	swap(rhs);
	return *this;
}


template <typename KEY_T, typename VAL_T, typename HASH_F>
size_t unordered_map<KEY_T, VAL_T, HASH_F>::collision_offset(size_t attempt) const {
	return attempt * attempt;
}

template <typename KEY_T, typename VAL_T, typename HASH_F>
size_t unordered_map<KEY_T, VAL_T, HASH_F>::hash(const KEY_T& key, size_t attempt) const {
	return (HASH_F{}(key) + collision_offset(attempt)) % m_buckets;
}

template <typename KEY_T, typename VAL_T, typename HASH_F>
void unordered_map<KEY_T, VAL_T, HASH_F>::rehash(size_t count) {
	size_t min_size = std::ceil(m_size / max_load_factor());

	size_t requested_buckets = std::max(count, min_size);

	unordered_map<KEY_T, VAL_T, HASH_F> new_map(requested_buckets);

	for (const pair_type& pair : *this) {
		new_map.insert(pair);
	}
	
	swap(new_map);
}

template <typename KEY_T, typename VAL_T, typename HASH_F>
void unordered_map<KEY_T, VAL_T, HASH_F>::reserve(size_t count) {
	size_t min_buckets = std::ceil(count / max_load_factor());

	if (m_buckets < min_buckets) {
		rehash(min_buckets);
	}
}

template <typename KEY_T, typename VAL_T, typename HASH_F>
std::pair<typename unordered_map<KEY_T, VAL_T, HASH_F>::iterator, bool> unordered_map<KEY_T, VAL_T, HASH_F>::insert(pair_type pair) {
	reserve(m_size + 1);

	const KEY_T& key = pair.first;

	tagged_entry* pos;
	size_t attempt = 0;

	do {
		pos = &m_table[hash(key, attempt++)];
	} while (pos->full() && pos->key() != key);

	// If the key was a duplicate, do not replace
	if (pos->full() && pos->key() == key) {
		return { pos, false };
	} else {
		pos->set_entry(pair);
		return { pos, true };
	}
}

template <typename KEY_T, typename VAL_T, typename HASH_F>
typename unordered_map<KEY_T, VAL_T, HASH_F>::iterator unordered_map<KEY_T, VAL_T, HASH_F>::find(const KEY_T& key) {
	size_t attempt = 0;
	tagged_entry* entry = nullptr;

	do {
		size_t idx = hash(key, attempt);
		entry = &m_table[idx];
		++attempt;
		// Keep looking until we find an empty bucket, or the key matches cur attempt
	} while (!entry->empty() && entry->key() != key);

	bool match = !entry->empty() && entry->key() == key;
	return match ? entry : end();
}

template <typename KEY_T, typename VAL_T, typename HASH_F>
typename unordered_map<KEY_T, VAL_T, HASH_F>::const_iterator unordered_map<KEY_T, VAL_T, HASH_F>::find(const KEY_T& key) const {
	size_t attempt = 0;
	tagged_entry* entry = nullptr;

	do {
		size_t idx = hash(key, attempt);
		entry = &m_table[idx];
		++attempt;
		// Keep looking until we find an empty bucket, or the key matches cur attempt
	} while (!entry->empty() && entry->key() != key);

	bool match = !entry->empty() && entry->key() == key;
	return match ? entry : end();
}

template <typename KEY_T, typename VAL_T, typename HASH_F>
VAL_T& unordered_map<KEY_T, VAL_T, HASH_F>::operator[](const KEY_T& key) {
	iterator it = find(key);

	if (it == end()) {
		throw std::invalid_argument("Key not found in map");
	}

	return it->second;
}

template <typename KEY_T, typename VAL_T, typename HASH_F>
const VAL_T& unordered_map<KEY_T, VAL_T, HASH_F>::operator[](const KEY_T& key) const {
	iterator it = find(key);

	if (it == end()) {
		throw std::invalid_argument("Key not found in map");
	}

	return it->second;
}

template <typename KEY_T, typename VAL_T, typename HASH_F>
typename unordered_map<KEY_T, VAL_T, HASH_F>::iterator unordered_map<KEY_T, VAL_T, HASH_F>::begin() {
	tagged_entry* entry = m_table.get();
	tagged_entry* end = m_table.get() + m_buckets;

	while (entry != end && !entry->full()) {
		++entry;
	}

	return entry;
}
template <typename KEY_T, typename VAL_T, typename HASH_F>
typename unordered_map<KEY_T, VAL_T, HASH_F>::const_iterator unordered_map<KEY_T, VAL_T, HASH_F>::begin() const {
	tagged_entry* entry = m_table.get();
	tagged_entry* end = m_table.get() + m_buckets;

	while (entry != end && !entry->full()) {
		++entry;
	}

	return entry;
}
template <typename KEY_T, typename VAL_T, typename HASH_F>
typename unordered_map<KEY_T, VAL_T, HASH_F>::const_iterator unordered_map<KEY_T, VAL_T, HASH_F>::cbegin() const {
	return begin();
}

} // namespace dsa

