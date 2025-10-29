#pragma once

#include <string>
#include <tuple>

namespace CSV {
namespace Parsing {

static const std::string whitespace = " \n\r\t";
static const char escapeChar = '\\';
static const char quoteChar = '"';
static const char sepChar = ',';

struct SplitToken {
	bool last;
	std::string token;
	std::string residue;
};

SplitToken splitFirstToken(std::string string);
SplitToken splitLastToken(std::string string);

} // namespace Parsing
} // namespace CSV

