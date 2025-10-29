#pragma once

#include <cstddef>
#include <memory>
#include <tuple>

#include "utility.hpp"

namespace dsa {

template <typename KEY_T, typename VAL_T, typename HASH_F = std::hash<KEY_T>>
class unordered_map {
private:
	class tagged_entry;

	template <typename IT_ENTRY_T, typename IT_PAIR_T>
	class iterator_base;

public:
	using pair_type = std::pair<const KEY_T, VAL_T>;
	using value_type = pair_type; // std::unordered_map has pair named value_type
	using size_type = size_t;

	using iterator = iterator_base<tagged_entry, pair_type>;
	using const_iterator = iterator_base<const tagged_entry, const pair_type>;

	unordered_map() :
		m_size(0),
		m_buckets(31),
		m_table(new tagged_entry[m_buckets]) {}

	unordered_map(size_t buckets) :
		m_size(0),
		m_buckets(next_prime(buckets)),
		m_table(new tagged_entry[m_buckets]) {}

	unordered_map(unordered_map<KEY_T, VAL_T, HASH_F>&& other) :
		m_size(other.m_size),
		m_buckets(other.m_buckets),
		m_table(std::move(other.m_table)) {}

	unordered_map(const unordered_map<KEY_T, VAL_T, HASH_F>& other);

	unordered_map& operator=(unordered_map<KEY_T, VAL_T, HASH_F> rhs);

	/**
	 * @brief Inserts key/value pair into the map
	 *
	 * @param value   The pair to insert, of type @c pair_type
	 * 
	 * @returns @c std::pair containing an iterator to the KVP inserted or
	 * the KVP blocking the insertion, and a boolean indicating whether the
	 * value was inserted
	 */
	std::pair<iterator, bool> insert(pair_type value);

	/**
	 * @brief Removes all elements from the map
	 */
	void clear();

	/**
	 * @brief Erase the pair referenced by the iterator
	 *
	 * @param pos   Iterator to the value to erase
	 *
	 * @returns Iterator to the next value after the erased value
	 */
	iterator erase(iterator pos);

	/**
	 * @brief Erase pairs with matching key, if any
	 *
	 * @param key   Key of value to erase
	 *
	 * @returns The number of nodes erased, 0 or 1
	 */
	size_type erase(const KEY_T& key);

	/**
	 * @brief Swap this map with another
	 *
	 * @param other   Map to swap with
	 */
	void swap(unordered_map<KEY_T, VAL_T, HASH_F>& other);

	/**
	 * @brief Gets the number of nodes matching a given key
	 *
	 * @param key   Key to count
	 *
	 * @returns 0 or 1, depending if there was a matching value
	 */
	size_type count(const KEY_T& key) const;
	/**
	 * @brief Checks if the given key exists in the map
	 *
	 * @param key   Key to check
	 *
	 * @returns true if a match was found, false otherwise
	 */
	bool contains(const KEY_T& key) const;

	/**
	 * @brief Find an iterator to the given key
	 *
	 * @param key   Key of pair to find
	 *
	 * @returns @c iterator or @c const_iterator to the matching pair,
	 * end if no match was found
	 */
	iterator find(const KEY_T& key);
	const_iterator find(const KEY_T& key) const;

	/**
	 * @brief Gets the corresponding value of a key
	 *
	 * @param key   Key of value to get
	 *
	 * @returns Reference to the found value
	 *
	 * @throws std::invalid_argument if no matching key was found
	 */
	VAL_T& operator[](const KEY_T& key);
	const VAL_T& operator[](const KEY_T& key) const;

	iterator begin();
	const_iterator begin() const;
	const_iterator cbegin() const;

	iterator end() { return m_table.get() + m_buckets; }
	const_iterator end() const { return m_table.get() + m_buckets; }
	const_iterator cend() const { return m_table.get() + m_buckets; }

	/**
	 * @brief Calculate the current load factor
	 */
	double load_factor() const { return static_cast<double>(m_size) / m_buckets; }
	double max_load_factor() const { return 0.5; }

	/**
	 * @brief Rehashes the table to at least count buckets
	 */
	void rehash(size_t count);

	/**
	 * @brief Reserve enough space for at least count entries
	 */
	void reserve(size_t count);

private:
	/**
	 * @brief Calculate the offset for collision on the given attempt
	 * 
	 * Currently uses quadratic probing
	 */
	size_t collision_offset(size_t attempt) const;

	/**
	 * @brief Calculates the hash of a key for an attempt
	 */
	size_t hash(const KEY_T& key, size_t attempt) const;

	/**
	 * @brief Determine if the load factor threshold would be crossed after one more insertion
	 *
	 * If this function returns true, the next insertion will result in a resize
	 * and a rehash.
	 */
	bool threshold_reached() const {
		double exp_load = static_cast<double>(m_size + 1) / m_buckets;
		return exp_load >= 0.5;
	}

	/**
	 * @brief Resizes the hashmap if the threshold has been reached
	 *
	 * New table size is the lowest prime number >= 2 * max_size
	 */
	void resize();

	std::unique_ptr<tagged_entry[]> m_table;
	size_type m_size;
	size_type m_buckets;

	/**
	 * Stores an entry in the hash table
	 *
	 * Each entry can be either, empty, a sentinel value set when the entry
	 * was previously filled, or a key-value pair. This class stores this
	 * entry and any associated data.
	 */
	class tagged_entry {
	public:
		tagged_entry() : m_empty(true), m_sentinel(false) {}
		tagged_entry(pair_type entry) : m_empty(false), m_sentinel(false), m_entry(entry) {}

		bool empty() const { return m_empty; }
		bool sentinel() const { return m_sentinel; }
		bool full() const { return !m_empty && !m_sentinel; }

		pair_type entry() { return m_entry; }

		KEY_T key() const { return m_entry.first; }
		VAL_T& value() { return m_entry.second; }
		const VAL_T& value() const { return m_entry.second; }

		void set_entry(pair_type entry) {
			m_empty = false;
			m_sentinel = false;
			m_entry = entry;
		}
		void remove_entry() {
			m_sentinel = true;
		}

	private:
		bool m_empty;
		bool m_sentinel;
		pair_type m_entry; // c++17 adds std::optional, would be useful here
	};

	template <typename IT_ENTRY_T, typename IT_PAIR_T>
	class iterator_base {
	public:
		iterator_base(IT_ENTRY_T* entry) : m_entry(entry) {}

		template <typename OTH_IT_ENTRY_T, typename OTH_IT_PAIR_T>
		iterator_base(iterator_base<OTH_IT_ENTRY_T, OTH_IT_PAIR_T> other) : m_entry(other.m_entry) {}

		using iterator_type = iterator_base<IT_ENTRY_T, IT_PAIR_T>;

		iterator_type& operator++() {
			while (*this != map->end() && !m_entry->full()){
				++m_entry;
			}

			return *this;
		}

		iterator_type operator++(int) const {
			iterator_type tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const iterator_type& other) const { return m_entry == other.m_entry; }
		bool operator!=(const iterator_type& other) const { return m_entry != other.m_entry; }

		IT_PAIR_T& operator*() const { return m_entry->m_pair; }
		IT_PAIR_T* operator->() const { return &m_entry->m_pair; }

	private:
		const unordered_map<KEY_T, VAL_T>* map; // Map this iterator belongs to
		IT_ENTRY_T* m_entry;
	};
};

} // namespace dsa

#include "unordered_map.inl.hpp"

