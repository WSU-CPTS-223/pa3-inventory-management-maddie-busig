#include "CSV/CSVRow.hpp"

#include "CSV/Parsing.hpp"

namespace CSV {

void CSVRow::parseLine(const std::string& line) {
	Parsing::SplitToken stok = {
		.last = false,
		.token = "",
		.residue = line,
	};

	while (!stok.last) {
		stok = Parsing::splitFirstToken(stok.residue);
		values_.insertBack(stok.token);
	}
}

} // namespace CSV

