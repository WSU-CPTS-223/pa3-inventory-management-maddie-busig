#pragma once

#include "avl_map.hpp"

#include <stdexcept>
#include <iostream>

#include "utility.hpp"

// avl_map

template <typename KEY_T, typename VAL_T>
avl_map<KEY_T, VAL_T>::avl_map(const avl_map<KEY_T, VAL_T>& other) {
	for (const auto& pair : other) {
		insert(pair);
	}
}

template <typename KEY_T, typename VAL_T>
avl_map<KEY_T, VAL_T>& avl_map<KEY_T, VAL_T>::operator=(avl_map<KEY_T, VAL_T> rhs) {
	swap(rhs);
	return *this;
}

template <typename KEY_T, typename VAL_T>
void avl_map<KEY_T, VAL_T>::swap(avl_map<KEY_T, VAL_T>& other) {
	std::swap(m_root, other.m_root);
}

template <typename KEY_T, typename VAL_T>
std::pair<typename avl_map<KEY_T, VAL_T>::iterator, bool> avl_map<KEY_T, VAL_T>::insert(pair_type pair) {
	node* parent = nullptr;
	node* cur = m_root.get();

	bool position_left;

	// Searching for the position iteratively. Recursion is unnecessary due to
	// storing the node's parent.
	while (cur != nullptr) {
		parent = cur;

		// If we encountered a pair with the same key, return
		// its iterator and false. Stop early
		if (cur->key() == pair.first) {
			return { iterator(cur), false };
		}

		position_left = pair.first < cur->key();

		if (position_left) {
			cur = cur->left();
		} else {
			cur = cur->right();
		}
	}

	std::unique_ptr<node> new_node = make_unique<node>(std::move(pair));
	node* new_node_handle = new_node.get();

	iterator new_node_iterator(new_node_handle);

	if (parent == nullptr) {
		// Inserting first node, need to set root
		m_root = std::move(new_node);
	} else if (position_left) {
		parent->set_left(std::move(new_node));
	} else {
		parent->set_right(std::move(new_node));
	}

	propagate_rebalance(new_node_handle);

	return { new_node_iterator, true };
}

template <typename KEY_T, typename VAL_T>
typename avl_map<KEY_T, VAL_T>::iterator avl_map<KEY_T, VAL_T>::erase(iterator pos) {
	iterator nextit(pos);
	++nextit; // This iterator will still be valid because the address will same

	class node* node = pos.node();
	class node* parent = node->parent();

	bool has_parent = parent != nullptr;
	bool is_parent_left = false;

	// Will be automatically deleted at end of scope
	std::unique_ptr<class node> owned_node;

	// Get an owning pointer to the given node
	if (has_parent) {
		// If the node has a parent, take the node from it
		is_parent_left = (node == parent->left());
		owned_node = is_parent_left ? parent->take_left() : parent->take_right();
	} else {
		// Otherwise it is the root node
		owned_node = std::move(m_root);
	}

	std::unique_ptr<class node> replacement_node = nullptr;

	// Find a node to replace the erased node with
	if (node->left() != nullptr) {
		// If the node has a left subtree then we can replace it with
		// the rightmost node of the left subtree
		class node* cur = node->left();

		while (cur->right() != nullptr) {
			cur = cur->right();
		}

		// Take the replacement node
		class node* replacement_parent = cur->parent();
		replacement_node = replacement_parent->take_right();

		// Give its other subtree to its parent, taking its place
		std::unique_ptr<class node> left_subtree = replacement_node->take_left();
		replacement_parent->set_right(std::move(left_subtree));

	} else if (node->right() != nullptr) {
		// Otherwise, if right subtree, same but opposite direction
		class node* cur = node->right();

		while (cur->left() != nullptr) {
			cur = cur->left();
		}

		class node* replacement_parent = cur->parent();
		replacement_node = replacement_parent->take_left();

		std::unique_ptr<class node> right_subtree = replacement_node->take_right();
		replacement_parent->set_left(std::move(right_subtree));
	}

	// Transfer ownership of subtrees
	replacement_node->set_left(owned_node->take_left());
	replacement_node->set_right(owned_node->take_right());

	// Give the replacement node back to the parent or root
	if (has_parent) {
		if (is_parent_left) {
			parent->set_left(std::move(replacement_node));
		} else {
			parent->set_right(std::move(replacement_node));
		}
	} else {
		m_root = std::move(replacement_node);
	}

	return nextit;
}

template <typename KEY_T, typename VAL_T>
typename avl_map<KEY_T, VAL_T>::size_type avl_map<KEY_T, VAL_T>::erase(const KEY_T& key) {
	iterator pos = find(key);

	if (pos == end()) {
		return 0;
	} else {
		erase(pos);
		return 1;
	}
}

template <typename KEY_T, typename VAL_T>
void avl_map<KEY_T, VAL_T>::clear() {
	m_root = nullptr;
}

template <typename KEY_T, typename VAL_T>
typename avl_map<KEY_T, VAL_T>::iterator avl_map<KEY_T, VAL_T>::find(const KEY_T& key) {
	node* cur = m_root.get();

	// Searching for the position iteratively. Recursion is unnecessary due to
	// storing the node's parent.
	while (cur != nullptr) {
		// If we encountered a pari with the same key, return
		// its iterator and false. Stop early
		if (cur->key() == key) {
			return iterator(cur);
		}

		if (key < cur->key()) {
			cur = cur->left();
		} else {
			cur = cur->right();
		}
	}

	// No key found
	return iterator(nullptr);
}

template <typename KEY_T, typename VAL_T>
typename avl_map<KEY_T, VAL_T>::const_iterator avl_map<KEY_T, VAL_T>::find(const KEY_T& key) const {
	node* cur = m_root.get();

	// Searching for the position iteratively. Recursion is unnecessary due to
	// storing the node's parent.
	while (cur != nullptr) {
		// If we encountered a pari with the same key, return
		// its iterator and false. Stop early
		if (cur->key() == key) {
			return const_iterator(cur);
		}

		if (key < cur->key()) {
			cur = cur->left();
		} else {
			cur = cur->right();
		}
	}

	// No key found
	return const_iterator(nullptr);
}

template <typename KEY_T, typename VAL_T>
VAL_T& avl_map<KEY_T, VAL_T>::operator[](const KEY_T& key) {
	iterator it = find(key);

	if (it == end()) {
		throw std::invalid_argument("No matching key found");
	}

	return it->second;
}

template <typename KEY_T, typename VAL_T>
const VAL_T& avl_map<KEY_T, VAL_T>::operator[](const KEY_T& key) const {
	const_iterator it = find(key);

	if (it == end()) {
		throw std::invalid_argument("No matching key found");
	}

	return it->second;
}

template <typename KEY_T, typename VAL_T>
typename avl_map<KEY_T, VAL_T>::size_type avl_map<KEY_T, VAL_T>::count(const KEY_T& key) const {
	return contains(key);
}

template <typename KEY_T, typename VAL_T>
bool avl_map<KEY_T, VAL_T>::contains(const KEY_T& key) const {
	return find(key) != end();
}

template <typename KEY_T, typename VAL_T>
typename avl_map<KEY_T, VAL_T>::iterator avl_map<KEY_T, VAL_T>::begin() {
	if (m_root == nullptr) {
		return nullptr;
	}

	node* cur = m_root.get();

	// The smallest key in the map is in the leftmost node in the tree
	while (cur->left() != nullptr) {
		cur = cur->left();
	}

	return iterator(cur);
}

template <typename KEY_T, typename VAL_T>
typename avl_map<KEY_T, VAL_T>::const_iterator avl_map<KEY_T, VAL_T>::begin() const {
	return cbegin();
}

template <typename KEY_T, typename VAL_T>
typename avl_map<KEY_T, VAL_T>::const_iterator avl_map<KEY_T, VAL_T>::cbegin() const {
	if (m_root == nullptr) {
		return nullptr;
	}

	const node* cur = m_root.get();

	// The smallest key in the map is in the leftmost node in the tree
	while (cur->left() != nullptr) {
		cur = cur->left();
	}

	return const_iterator(cur);
}

template <typename KEY_T, typename VAL_T>
void avl_map<KEY_T, VAL_T>::rotate_right(node* node) {
	if (node->left() == nullptr) {
		throw std::invalid_argument("Node does not have a left subtree");
	}

	class node* parent = node->parent();
	bool in_parent_left = false;

	std::unique_ptr<class node> owned_node;

	// Need to own the node to move it
	if (parent == nullptr) {
		// If the node does not have a parent, then it is the root, so take it
		owned_node = std::move(m_root);
	} else {
		// Otherwise find and take it from the parent
		in_parent_left = parent->left() == node;
		owned_node = in_parent_left ? parent->take_left() : parent->take_right();
	}

	// Node's left child
	std::unique_ptr<class node> lchild(node->take_left());
	// Left child's right subtree
	std::unique_ptr<class node> lchild_rsubtree(lchild->take_right());

	// Reattach nodes
	lchild->set_right(std::move(owned_node));
	node->set_left(std::move(lchild_rsubtree));

	// Give the new root of the subtree back to the parent or m_root
	if (parent == nullptr) {
		m_root = std::move(lchild);
	} else {
		if (in_parent_left) {
			parent->set_left(std::move(lchild));
		} else {
			parent->set_right(std::move(lchild));
		}
	}
}

template <typename KEY_T, typename VAL_T>
void avl_map<KEY_T, VAL_T>::rotate_left(node* node) {
	if (node->right() == nullptr) {
		throw std::invalid_argument("Node does not have a left subtree");
	}

	class node* parent = node->parent();
	bool in_parent_left = false;

	std::unique_ptr<class node> owned_node;

	// Need to own the node to move it
	if (parent == nullptr) {
		// If the node does not have a parent, then it is the root, so take it
		owned_node = std::move(m_root);
	} else {
		// Otherwise find and take it from the parent
		in_parent_left = parent->left() == node;
		owned_node = in_parent_left ? parent->take_left() : parent->take_right();
	}

	// Node's right child
	std::unique_ptr<class node> rchild(node->take_right());
	// Right child's left subtree
	std::unique_ptr<class node> rchild_lsubtree(rchild->take_left());

	// Reattach nodes
	rchild->set_left(std::move(owned_node));
	node->set_right(std::move(rchild_lsubtree));

	// Give the new root of the subtree back to the parent or m_root
	if (parent == nullptr) {
		m_root = std::move(rchild);
	} else {
		if (in_parent_left) {
			parent->set_left(std::move(rchild));
		} else {
			parent->set_right(std::move(rchild));
		}
	}
}

template <typename KEY_T, typename VAL_T>
void avl_map<KEY_T, VAL_T>::rebalance_node(node* node) {
	if (!node->unbalanced()) {
		return;
	}

	bool node_left_heavy = node->left_heavy();

	class node* child = node_left_heavy ? node->left() : node->right();

	bool child_left_heavy = child->left_heavy();

	bool lr_or_rl = node_left_heavy != child_left_heavy;

	// Perform first of two rotations if necessary (LR or RL rotation)
	if (lr_or_rl && child_left_heavy) {
		rotate_right(child);
	} else if (lr_or_rl && !child_left_heavy) {
		rotate_left(child);
	}

	if (node_left_heavy) {
		rotate_right(node);
	} else {
		rotate_left(node);
	}
}

template <typename KEY_T, typename VAL_T>
void avl_map<KEY_T, VAL_T>::propagate_rebalance(node* bottom) {
	node* cur = bottom;

	while (cur != nullptr) {
		if (cur->unbalanced()) {
			rebalance_node(cur);
		}

		cur = cur->parent();
	}
}

// end avl_map

// avl_map::node

template <typename KEY_T, typename VAL_T>
std::unique_ptr<typename avl_map<KEY_T, VAL_T>::node> avl_map<KEY_T, VAL_T>::node::take_left() {
	if (m_left == nullptr) {
		return nullptr;
	}

	std::unique_ptr<node> tmp(std::move(m_left));
	m_left = nullptr;

	propagate_height();

	tmp->m_parent = nullptr;
	return tmp;
}

template <typename KEY_T, typename VAL_T>
void avl_map<KEY_T, VAL_T>::node::set_left(std::unique_ptr<typename avl_map<KEY_T, VAL_T>::node>&& left) {
	m_left = std::move(left);

	if (m_left != nullptr) {
		m_left->m_parent = this;
	}

	propagate_height();
}

template <typename KEY_T, typename VAL_T>
std::unique_ptr<typename avl_map<KEY_T, VAL_T>::node> avl_map<KEY_T, VAL_T>::node::take_right() {
	if (m_right == nullptr) {
		return nullptr;
	}

	std::unique_ptr<node> tmp(std::move(m_right));
	m_right = nullptr;

	propagate_height();

	tmp->m_parent = nullptr;
	return tmp;
}

template <typename KEY_T, typename VAL_T>
void avl_map<KEY_T, VAL_T>::node::set_right(std::unique_ptr<typename avl_map<KEY_T, VAL_T>::node>&& right) {
	m_right = std::move(right);

	if (m_right != nullptr) {
		m_right->m_parent = this;
	}

	propagate_height();
}

template <typename KEY_T, typename VAL_T>
int avl_map<KEY_T, VAL_T>::node::get_balance_factor() const {
	int bfactor = 0;

	if (m_left != nullptr) {
		bfactor -= m_left->m_height;
	}
	if (m_right != nullptr) {
		bfactor += m_right->m_height;
	}

	return bfactor;
}


template <typename KEY_T, typename VAL_T>
void avl_map<KEY_T, VAL_T>::node::update_height() {
	int lheight = 0;
	int rheight = 0;

	if (m_left != nullptr) {
		lheight = m_left->m_height + 1;
	}
	if (m_right != nullptr) {
		rheight = m_right->m_height + 1;
	}

	m_height = std::max(lheight, rheight);
}

template <typename KEY_T, typename VAL_T>
void avl_map<KEY_T, VAL_T>::node::propagate_height() {
	node* cur = this;

	while (cur != nullptr) {
		cur->update_height();
		cur = cur->m_parent;
	}
}

// end avl_map::node

// avl_map::iterator_base

template <typename KEY_T, typename VAL_T>
template <typename IT_NODE_T, typename IT_PAIR_T>
typename avl_map<KEY_T, VAL_T>::template iterator_base<IT_NODE_T, IT_PAIR_T>&
avl_map<KEY_T, VAL_T>::iterator_base<IT_NODE_T, IT_PAIR_T>::operator++() {
	// If the current node has a right subtree, we need to find the smallest
	// value from that subtree
	if (m_node->right() != nullptr) {
		IT_NODE_T* cur = m_node->right();

		// The smallest key will be in the leftmost node of the subtree
		while (cur->left() != nullptr) {
			cur = cur->left();
		}

		// That smallest value is the next node in the sequence
		m_node = cur;
		return *this;
	}
	
	// If the current node does not have a right subtree, we need to
	// traverse the predecessors until we come from the predecessor's
	// left subtree, or we reach the root node from the right
	IT_NODE_T* subtree = m_node;
	IT_NODE_T* parent = m_node->parent();

	while (parent != nullptr && subtree != parent->left()) {
		subtree = parent;
		parent = parent->parent();
	}

	if (parent == nullptr) {
		// If the parent is null, then we reached the root from the
		// right subtree, and we are done
		m_node = nullptr;
	} else {
		// Otherwise we found the next node (the parent)
		m_node = parent;
	}

	return *this;
}

// end avl_map::iterator_base

