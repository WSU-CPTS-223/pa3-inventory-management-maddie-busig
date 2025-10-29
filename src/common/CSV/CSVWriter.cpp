#include "CSV/CSVWriter.hpp"

#include <sstream>

namespace CSV {

void CSVWriter::write(const CSVData& csv) {
	std::stringstream ss;
	size_t idx = 0;

	for (std::string tok : csv.header()) {
		ss << tok;
		++idx;

		if (idx != csv.header().tokens().size()) {
			ss << ",";
		}
	}

	writeline(ss.str());

	for (CSVTuple row : csv.rows()) {
		ss.str("");
		idx = 0;

		for (CSVValue val : row) {
			switch (val.type()) {
			case CSVValueType::CSVInt: ss << val.get<int>(); break;
			case CSVValueType::CSVDouble: ss << val.get<double>(); break;
			case CSVValueType::CSVBool: ss << val.get<bool>(); break;
			case CSVValueType::CSVString: ss << "\"" << val.get<std::string>() << "\""; break;
			default: break;
			}

			++idx;

			if (idx != row.values().size()) {
				ss << ",";
			}
		}

		writeline(ss.str());
	}
}

} // namespace CSV

