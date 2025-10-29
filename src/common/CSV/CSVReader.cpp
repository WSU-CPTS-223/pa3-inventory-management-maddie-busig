#include "CSV/CSVReader.hpp"

#include <iostream>

#include "CSV/CSVRow.hpp"
#include "CSV/CSVTuple.hpp"

namespace CSV {

CSVData CSVReader::read() {
	CSVData csv;

	if (has_header_) {
		CSVRow header(readline());
		csv.header() = header;
	}

	std::string line;

	// TODO: Clean up, comment, and optimize

	// Everything uses List currently, so random accesses are super slow.
	// Really should just be using vector or something similar.
	while ((line = readline()), !eof()) {
		CSVTuple tuple;

		CSVRow row(line);
		auto& tokens = row.tokens();

		for (size_t i = tokens.size(); i-- > 0; ) {
			std::string token = tokens[i];
			CSVValue val;

			// Hacky check for token boolean value
			bool upcase = (token == "TRUE");
			bool camelcase = (token == "True");
			bool lowercase = (token == "true");

			if (i < types_.size()) {
				// Do type conversion
				CSVValueType type = types_[i];
				switch (type) {
				case CSVValueType::CSVInt:
					val = CSVValue(token.empty() ? 0 : std::stoi(token));
					break;
				case CSVValueType::CSVDouble:
					val = CSVValue(token.empty() ? 0.0 : std::stod(token));
					break;
				case CSVValueType::CSVBool:
					val = CSVValue(upcase || camelcase || lowercase);
					break;
				case CSVValueType::CSVString:
					val = CSVValue(token);
					break;
				default:
					throw std::invalid_argument("Invalid type found");
				}
			} else {
				val = CSVValue(token);
			}

			tuple.values().insertFront(val);
		}

		csv.rows().insertBack(tuple);
	}

	return csv;
}

} // namespace CSV

