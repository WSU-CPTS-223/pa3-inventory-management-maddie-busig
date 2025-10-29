#pragma once

#include <memory>

template <typename KEY_T, typename VAL_T>
class avl_map {
private:
	class node;

	template <typename IT_NODE_T, typename IT_PAIR_T>
	class iterator_base;

public:
	using pair_type = std::pair<const KEY_T, VAL_T>;
	using value_type = pair_type; // std::map has pair named value_type
	using size_type = size_t;

	using iterator = iterator_base<node, pair_type>;
	using const_iterator = iterator_base<const node, const pair_type>;

	avl_map() : m_root(nullptr) {}
	avl_map(const avl_map& other);
	avl_map(avl_map&& other) : m_root(std::move(other.m_root)) {}

	avl_map& operator=(avl_map rhs);

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
	void swap(avl_map<KEY_T, VAL_T>& other);

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

	iterator end() { return nullptr; }
	const_iterator end() const { return nullptr; }
	const_iterator cend() const { return nullptr; }

	int height() const { return m_root->height(); }
	int balance() const { return m_root->get_balance_factor(); }

private:
	std::unique_ptr<node> m_root;

	/*
	 * @brief Rotate left about the given node
	 *
	 * @param node   Node to rotate about
	 *
	 * Example: Given the following tree, to balance the tree a left
	 * rotation should be performed about node A.
	 *
	 * A
	 *  \ 
	 *   B
	 *    \
	 *     C
	 *
	 * After node A is passed to rotate_left:
	 *
	 *   B
	 *  / \
	 * A   C
	 *
	 */
	void rotate_left(node* node);

	/**
	 * @brief Rotate right about the given node
	 *
	 * @param node   Node to rotate about
	 *
	 * Example: Given the following tree, to balance the tree a right
	 * rotation should be performed about node A.
	 *
	 *     A
	 *    / 
	 *   B
	 *  /
	 * C
	 *
	 * After node A is passed to rotate_right:
	 *
	 *   B
	 *  / \
	 * C   A
	 *
	 */
	void rotate_right(node* node);

	/**
	 * @brief Rebalances an unbalanced node by performing the needed rotations
	 */
	void rebalance_node(node* node);

	/**
	 * @brief Rebalances a node and all its predecessors as necessary
	 */
	void propagate_rebalance(node* bottom);

	class node {
		template <typename IT_NODE_T, typename IT_PAIR_T>
		friend class iterator_base;

	public:
		node(const pair_type& pair) : m_pair(pair) {}

		KEY_T key() const { return m_pair.first; }

		void set_value(const VAL_T& value) { m_pair.second = value; }
		VAL_T value() const { return m_pair.second; }

		pair_type pair() const { return m_pair; }

		/**
		 * @brief Get a non-owning pointer to the node's left subtree
		 */
		node* left() const { return m_left.get(); }
		/**
		 * @brief Takes ownership of the node's left subtree
		 *
		 * Does not simply return using \c std::move , this function
		 * also leaves this node's subtree in a valid state (\c null ).
		 *
		 * @returns A \c std::unique_ptr<node> owning the node's left subtree
		 */
		std::unique_ptr<node> take_left();
		/**
		 * @brief Sets the node's left subtree, taking ownership
		 */
		void set_left(std::unique_ptr<node>&& left);
		
		/**
		 * @brief Get a non-owning pointer to the node's right subtree
		 */
		node* right() const { return m_right.get(); }
		/**
		 * @brief Takes ownership of the node's right subtree
		 *
		 * Does not simply return using \c std::move , this function
		 * also leaves this node's subtree in a valid state (\c null ).
		 *
		 * @returns A \c std::unique_ptr<node> owning the node's right subtree
		 */
		std::unique_ptr<node> take_right();
		/**
		 * @brief Sets the node's right subtree, taking ownership
		 */
		void set_right(std::unique_ptr<node>&& right);

		/**
		 * @brief Gets the node's parent node
		 */
		node* parent() const { return m_parent; }

		/**
		 * @brief Get the node's height
		 */
		int height() const { return m_height; }

		/**
		 * @brief Get the node's balance factor
		 */
		int get_balance_factor() const;

		bool left_heavy() const { return get_balance_factor() < 0; }
		bool right_heavy() const { return get_balance_factor() > 0; }

		bool unbalanced() const { return get_balance_factor() < -1 || get_balance_factor() > 1; }

	private:

		/**
		 * @brief Recalculate this node's height from its children
		 */
		void update_height();

		/**
		 * @brief Propagates height updates up this node and its predecessors
		 *
		 * Propagates height iteratively instead of recursively. This implementation
		 * has O(1) instead of O(log n) required for the recursive implementation.
		 */
		void propagate_height();

		pair_type m_pair;
		int m_height = 0;

		std::unique_ptr<node> m_left = nullptr;
		std::unique_ptr<node> m_right = nullptr;

		// Non-owning, managed by the parent node.
		//
		// Justification:
		// When iterating over the tree, finding the next node in the
		// sequence would be O(log n) when searching from the tree's root.
		// This would be necessary when moving up the tree. Instead by
		// storing the node's parent we can just move up the tree,
		// instead of iterating all the way down from the top. I didn't
		// work out an entire proof that this is faster, but I would
		// imagine it is.
		//
		// Another benefit:
		// Searching for the next node would also require O(log n) space
		// complexity using an in-order traversal from the root node, but
		// storing the parent allows an algorithm requiring only O(1)
		// complexity (no need for a stack/recursion).
		//
		// Don't know if we're not allowed to store the parent (didn't
		// see anything in the document explicitly forbidding it). But
		// I hope this helps explain why I did \.o~o"_
		node* m_parent = nullptr;
	};

	template <typename IT_NODE_T, typename IT_PAIR_T>
	class iterator_base {
	private:
		IT_NODE_T* m_node;

	public:
		iterator_base(IT_NODE_T* node) : m_node(node) {}

		template <typename OTH_IT_NODE_T, typename OTH_IT_PAIR_T>
		iterator_base(iterator_base<OTH_IT_NODE_T, OTH_IT_PAIR_T> other) : m_node(other.m_node) {}

		using iterator_type = iterator_base<IT_NODE_T, IT_PAIR_T>;

		iterator_type& operator++();
		iterator_type operator++(int) const {
			iterator_type tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const iterator_type& other) const { return m_node == other.m_node; }
		bool operator!=(const iterator_type& other) const { return m_node != other.m_node; }

		IT_PAIR_T& operator*() const { return m_node->m_pair; }
		IT_PAIR_T* operator->() const { return &m_node->m_pair; }

		IT_NODE_T* node() const { return m_node; }

		int height() const { return m_node->height(); }
		int get_balance_factor() const { return m_node->get_balance_factor(); }
		bool unbalanced() const { return m_node->unbalanced(); }
	};
};

#include "avl_map.inl.hpp"

