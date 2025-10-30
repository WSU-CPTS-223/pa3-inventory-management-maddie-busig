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
		map.insert(pair);
	}

	for (const pair_type& pair : pairs) {
		std::string found = map[pair.first];

		if (found != pair.second) {
			std::cerr << "Incorrect value found " << found << ", expected "
			<< pair.second << " for key " << pair.first << std::endl;
			return -2;
		}
	}

	return 0;
}

