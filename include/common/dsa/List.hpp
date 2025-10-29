#pragma once

#include <cstddef>
#include <stdexcept>

template <typename T>
class List {
	struct Node {
		T data;
		Node* next = nullptr;

		Node(T data_) : data(data_) {}
	};

public:
	template <typename NODE_T, typename DATA_T>
	class IteratorBase {
	public:
		IteratorBase(NODE_T* node) : node_(node) {}
		IteratorBase<NODE_T, DATA_T>& operator++() { node_ = node_->next; return *this; }
		IteratorBase<NODE_T, DATA_T> operator++(int) { auto retval = *this; ++(*this); return retval; }
		bool operator==(const IteratorBase<NODE_T, DATA_T>& rhs) const { return node_ == rhs.node_; }
		bool operator!=(const IteratorBase<NODE_T, DATA_T>& rhs) const { return node_ != rhs.node_; }
		DATA_T& operator*() const { return node_->data; }

	private:
		friend List<T>;

		NODE_T* node_;
	};

	using Iterator = IteratorBase<Node, T>;
	using ConstIterator = IteratorBase<const Node, const T>;

	List() : head_(nullptr), size_(0) {}
	List(const List<T>& other);
	List(List<T>&& other);

	List(std::initializer_list<T> init_list);

	List<T>& operator=(List<T> other);
	void swap(List<T>& other);

	~List();

	void clear();

	void insertFront(T value);
	void insertBack(T value);

	void removeFront();
	void removeBack();

	template <typename IT_T>
	void removeAt(IT_T it);

	bool empty() const;
	size_t size() const { return size_; }

	T& operator[](size_t idx);
	const T& operator[](size_t idx) const;

	Iterator begin() { return Iterator(head_); }
	ConstIterator begin() const { return ConstIterator(head_); }
	ConstIterator cbegin() const { return ConstIterator(head_); }

	Iterator end() { return nullptr; }
	ConstIterator end() const { return nullptr; }
	ConstIterator cend() const { return nullptr; }

private:
	Node* head_;
	size_t size_;
};

template <typename T>
List<T>::List(const List<T>& other) :
	size_(other.size_) {
	Node** link_ptr = &head_;

	for (auto val : other) {
		Node* cur = new Node(val);

		*link_ptr = cur;
		link_ptr = &cur->next;
	}
}

template <typename T>
List<T>::List(std::initializer_list<T> init_list) : head_(nullptr), size_(0) {
	Node* prev = nullptr;

	for (const T& val : init_list) {
		Node* cur = new Node(val);

		if (prev == nullptr) {
			head_ = cur;
		} else {
			prev->next = cur;
		}

		prev = cur;

		++size_;
	}
}

template <typename T>
List<T>& List<T>::operator=(List<T> other) {
	swap(other);
	return *this;
}

template <typename T>
void List<T>::swap(List<T>& other) {
	std::swap(head_, other.head_);
	std::swap(size_, other.size_);
}

template <typename T>
List<T>::List(List<T>&& other) : head_(nullptr), size_(0) {
	swap(other);
}

template <typename T>
List<T>::~List() {
	clear();
}

template <typename T>
void List<T>::insertFront(T value) {
	Node* new_node = new Node(value);

	new_node->next = head_;
	head_ = new_node;
	++size_;
}

template <typename T>
void List<T>::insertBack(T value) {
	Node* new_node = new Node(value);

	Node** link_ptr = &head_;
	Node* cur = head_;

	while (cur != nullptr) {
		link_ptr = &cur->next;
		cur = cur->next;
	}

	*link_ptr = new_node;
	++size_;
}

template <typename T>
void List<T>::removeFront() {
	if (empty()) {
		return;
	}

	Node* old_head = head_;
	head_ = head_->next;
	
	delete old_head;
	--size_;
}

template <typename T>
void List<T>::removeBack() {
	if (empty()) {
		return;
	}

	Node** link_ptr = &head_;
	Node* cur = head_;

	while (cur->next != nullptr) {
		link_ptr = &cur->next;
		cur = cur->next;
	}

	delete cur;
	*link_ptr = nullptr;
	--size_;
}

template <typename T>
template <typename IT_T>
void List<T>::removeAt(IT_T it) {
	if (it.node_ == head_) {
		return removeFront();
	}

	Node* search = it.node_;
	Node* cur = head_;
	Node* prev = nullptr;

	while (cur != nullptr && cur != search) {
		prev = cur;
		cur = cur->next;
	}

	if (cur != nullptr) {
		prev->next = cur->next;
		delete cur;

		--size_;
	}
}

template <typename T>
void List<T>::clear() {
	Node* cur = head_;

	while (cur != nullptr) {
		Node* next = cur->next;
		delete cur;
		cur = next;
	}

	head_ = nullptr;
	size_ = 0;
}

template <typename T>
bool List<T>::empty() const {
	return head_ == nullptr;
}

template <typename T>
T& List<T>::operator[](size_t idx) {
	const List<T>& thisref = *this; // Using const cast to avoid code duplication
	return const_cast<T&>(thisref[idx]);
}

template <typename T>
const T& List<T>::operator[](size_t idx) const {
	Node* cur = head_;
	size_t requested_idx = idx;

	while (cur != nullptr && idx > 0) {
		cur = cur->next;
		--idx;
	}

	if (cur == nullptr) {
		throw std::out_of_range(std::string("List index out of range: ") + std::to_string(requested_idx));
	}

	return cur->data;
}

