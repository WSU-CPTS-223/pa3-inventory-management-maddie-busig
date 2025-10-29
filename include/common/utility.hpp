#pragma once

#include <memory>
#include <cmath>

/**
 * @brief Implementation of std::make_unique
 *
 * Targetting C++11, but make_unique was not added until C++14
 */
template <typename CLASS_T, typename... ARGS_T>
std::unique_ptr<CLASS_T> make_unique(ARGS_T&&... args) {
	return std::unique_ptr<CLASS_T>(new CLASS_T(std::forward<ARGS_T>(args)...));
}

/**
 * @brief Naive isprime
 */
inline bool is_prime(int x) {
	int range = sqrt(x);

	for (int i = 0; i <= range; ++i) {
		if (x % i == 0) { return false; }
	}

	return true;
}

/**
 * @brief Find next prime >= x
 */
inline int next_prime(int x) {
	while (!is_prime(x)) { ++x; }
	return x;
}

