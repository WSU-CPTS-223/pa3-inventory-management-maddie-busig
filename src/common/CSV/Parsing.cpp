#include "CSV/Parsing.hpp"

namespace CSV {
namespace Parsing {

std::string::size_type findFirstUnescapedQuote(std::string str, std::string::size_type pos = 0) {
	std::string::size_type quotePos = pos;
	bool found = false;

	while (!found && quotePos < str.size()) {
		bool candidate = (str[quotePos] == quoteChar);
		bool escaped = (quotePos != 0 && str[quotePos-1] == escapeChar);

		found = candidate && !escaped;

		if (!found) {
			++quotePos;
		}
	}

	return found ? quotePos : std::string::npos;
}

SplitToken splitFirstToken(std::string line) {
	// Trim leading whitespace
	line.erase(0, line.find_first_not_of(whitespace));

	if (line.empty()) {
		return { true, "", "" }; 
	}

	bool quoted = (line[0] == quoteChar);
	std::string::size_type tokBegin, tokEnd, nextDelim;

	if (quoted) {
		tokBegin = 1; // Begin after the first quote
		tokEnd = findFirstUnescapedQuote(line, tokBegin);
		nextDelim = line.find_first_of(sepChar, tokEnd + 1);
	} else {
		tokBegin = 0;
		nextDelim = line.find_first_of(sepChar);

		tokEnd = nextDelim;
	}

	bool last = nextDelim == std::string::npos;
	std::string token = line.substr(tokBegin, tokEnd - tokBegin);
	token.erase(token.find_last_not_of(whitespace) + 1);

	std::string residue = last ? "" : line.substr(nextDelim + 1);

	return { last, token, residue };
}

} // namespace Parsing
} // namespace CSV

