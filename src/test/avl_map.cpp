#include "test_common.h"

#include <string>
#include <tuple>
#include <vector>
#include <algorithm>

#include "dsa/avl_map.hpp"

TEST_ENTRYPOINT int test_avl_map_insert_find(int argc, char** argv) {
	using pair_type = avl_map<int, std::string>::pair_type;
	std::vector<pair_type> pairs = {
		{ 30, "thirty" },
		{ 20, "twenty" },
		{ 25, "twenty-five" },
		{ 22, "twenty-two" },
		{ 40, "fourty" },
		{ 35, "thirty-five" },
	};

	avl_map<int, std::string> map;

	for (const pair_type& pair : pairs) {
		try {
			map.insert(pair);
		} catch (std::exception& e) {
			std::cerr << "Caught exception while inserting " << e.what() << std::endl;
			return -1;
		}
	}

	for (const pair_type& pair : pairs) {
		try {
			std::string found = map[pair.first];
			if (found != pair.second) {
				std::cerr << "Incorrect value found " << found << ", expected "
				          << pair.second << " for key " << pair.first << std::endl;
				return -2;
			}
		} catch (std::exception& e) {
			std::cerr << "Caught exception while searching map " << e.what() << std::endl;
			return -3;
		}
	}

	return 0;
}

TEST_ENTRYPOINT int test_avl_map_iteration(int argc, char** argv) {
	using pair_type = avl_map<int, std::string>::pair_type;

	using vpair_type = std::pair<int, std::string>; // volatile pair, needed to sort

	std::vector<vpair_type> pairs = {
		{ 30, "thirty" },
		{ 20, "twenty" },
		{ 25, "twenty-five" },
		{ 22, "twenty-two" },
		{ 40, "fourty" },
		{ 35, "thirty-five" },
	};

	avl_map<int, std::string> map;

	for (const vpair_type& pair : pairs) {
		try {
			map.insert(pair);
		} catch (std::exception& e) {
			std::cerr << "Caught exception while inserting " << e.what() << std::endl;
			return -1;
		}
	}

	std::sort(pairs.begin(), pairs.end());

	int i = 0;

	for (const pair_type& found_pair : map) {
		const pair_type& expected_pair = pairs[i++];

		if (found_pair != expected_pair) {
			std::cerr << "Incorrect next pair key. Got " << found_pair.first
			          << ". Expected " << expected_pair.second << std::endl;

			return -1;
		}
	}

	return 0;
}

TEST_ENTRYPOINT int test_avl_map_balance(int argc, char** argv) {
	using pair_type = std::pair<int, std::string>;
	std::vector<pair_type> pairs = {
		{ 30, "thirty" },
		{ 20, "twenty" },
		{ 25, "twenty-five" },
		{ 22, "twenty-two" },
		{ 40, "fourty" },
		{ 35, "thirty-five" },
	};

	avl_map<int, std::string> map;

	for (const pair_type& pair : pairs) {
		try {
			map.insert(pair);
		} catch (std::exception& e) {
			std::cerr << "Caught exception while inserting " << e.what() << std::endl;
			return -1;
		}
	}

	for (auto it = map.begin(); it != map.end(); ++it) {
		int balance = it.get_balance_factor();

		std::cout << "Balance of " << it->first << ": " << balance << std::endl;

		if (it.unbalanced()) {
			std::cerr << "Unbalanced node with key " << it->first
			          << ". Balance factor: " << balance << std::endl;
			return -1;
		}
	}

	return 0;
}

TEST_ENTRYPOINT int test_avl_map_erase(int argc, char** argv) {
	using pair_type = std::pair<int, std::string>;
	std::vector<pair_type> pairs = {
		{ 30, "thirty" },
		{ 20, "twenty" },
		{ 25, "twenty-five" },
		{ 22, "twenty-two" },
		{ 40, "fourty" },
		{ 35, "thirty-five" },
	};

	avl_map<int, std::string> map;

	for (const pair_type& pair : pairs) {
		try {
			map.insert(pair);
		} catch (std::exception& e) {
			std::cerr << "Caught exception while inserting " << e.what() << std::endl;
			return -1;
		}
	}

	if (!map.contains(30)) {
		std::cerr << "Does not contain 30" << std::endl;
		return -2;
	}

	auto itnext = map.erase(map.find(30));

	if (itnext->first != 35) {
		std::cerr << "35 is not the next iterator!" << std::endl;
		return -5;
	}

	if (map.contains(30)) {
		std::cerr << "Map still contains 30" << std::endl;
		return -3;
	}

	for (auto it = map.begin(); it != map.end(); ++it) {
		int balance = it.get_balance_factor();

		std::cout << "Balance of " << it->first << ": " << balance << std::endl;

		if (it.unbalanced()) {
			std::cerr << "Unbalanced node with key " << it->first
			          << ". Balance factor: " << balance << std::endl;
			return -4;
		}
	}

	return 0;
}


