#pragma once

#include <cstdint>
#include <utility>
#include <stdexcept>

/**
 * @brief Implementation of std::optional
 *
 * Because we are targetting c++11, we do not have access to std::optional
 * since it was introduced in c++17. Useful for unordered_map implementation.
 */
template <typename T>
class optional {
public:
	optional() : m_exists(false), m_val(nullptr) {}

	optional(const optional<T>& other) :
		m_exists(other.m_exists),
		m_val(nullptr) {

		if (m_exists) {
			m_val = new (m_buf) T(*other.m_val);
		}
	}

	optional(optional<T>&& other) :
		m_exists(other.m_exists),
		m_val(nullptr) {
		
		if (m_exists) {
			m_val = new (m_buf) T(std::move(*other.m_val));
		}
	}

	optional(const T& val) :
		m_exists(true),
		m_val(new (m_buf) T(val)) {}
	
	optional(T&& val) :
		m_exists(true),
		m_val(new (m_buf) T(val)) {}

	~optional() {
		reset();
	}

	optional<T>& operator=(optional<T> rhs) {
		swap(rhs);
		return *this;
	}

	/**
	 * @brief Swaps the contents of this with another optional<T>
	 *
	 * @param other   Value to swap with
	 *
	 * If both optionals contain values, they are swapped using std::swap.
	 * If only one optional contains a value, its destination is move constructed
	 * from its value, and the source destructed. Otherwise, both contain no
	 * value and nothing happens.
	 */
	void swap(optional<T>& other) {
		if (m_exists && other.m_exists) {
			std::swap(*m_val, *other.m_val);
		} else if (m_exists) {
			// If only one contains a value, move construct and destruct
			other.m_val = new (other.m_buf) T(std::move(*m_val));
			m_val->~T();
			m_val = nullptr;
		} else if (other.m_exists) {
			// If only one contains a value, move construct and destruct
			m_val = new (m_buf) T(std::move(*other.m_val));
			other.m_val->~T();
			other.m_val = nullptr;
		}

		std::swap(m_exists, other.m_exists);
	}

	/**
	 * @brief Unchcked access to contained value
	 *
	 * If the optional does not contain a value, undefined behaviour occurs.
	 *
	 * @returns The contained value
	 */
	T* operator->() noexcept { return m_val; }
	const T* operator->() const noexcept { return m_val; }

	/**
	 * @brief Unchcked access to contained value
	 *
	 * If the optional does not contain a value, undefined behaviour occurs.
	 *
	 * @returns The contained value
	 */
	T& operator*() noexcept { return *m_val; }
	const T& operator*() const noexcept { return *m_val; }

	/**
	 * @brief Checked access to contained value
	 *
	 * If the optional does not contain a value, throws out_of_range
	 *
	 * @returns The contained value
	 */
	T& value() {
		if (!m_exists) {
			throw std::out_of_range("Bad optional access");
		}

		return *m_val;
	}
	const T& value() const {
		if (!m_exists) {
			throw std::out_of_range("Bad optional access");
		}

		return *m_val;
	}

	bool has_value() const {
		return m_exists;
	}

	/**
	 * @brief Returns the contains value or the given value
	 */
	T value_or(T&& default_val) const {
		return m_exists ? *m_val : default_val;
	}

	void reset() {
		if (m_exists) {
			m_val->~T();
		}

		m_exists = false;
	}

private:
	bool m_exists;

	// Buffer for storing value, if it exists. Using placement new because
	// we only want to construct the object if it exists
	alignas(T) uint8_t m_buf[sizeof(T)];
	T* m_val;
};

