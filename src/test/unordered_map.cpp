#include "test_common.h"

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <algorithm>

#include "dsa/unordered_map.hpp"

using namespace dsa;

TEST_ENTRYPOINT int test_unordered_map_insert_find(int argc, char** argv) {
	using pair_type = unordered_map<int, std::string>::pair_type;
	std::vector<pair_type> pairs = {
		{ 30, "thirty" },
		{ 20, "twenty" },
		{ 25, "twenty-five" },
		{ 22, "twenty-two" },
		{ 40, "fourty" },
		{ 35, "thirty-five" },
	};

	unordered_map<int, std::string> map;

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

