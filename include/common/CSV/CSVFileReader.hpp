#pragma once

#include <fstream>

#include "CSVReader.hpp"

namespace CSV {

class CSVFileReader : public CSVReader {
public:
	CSVFileReader(std::string filename, bool has_header = true) :
		CSVReader(has_header),
		filename_(filename),
		file_(filename) {

		if (!file_.is_open()) {
			throw std::invalid_argument("Failed to open file " + filename);
		}
	}

	CSVFileReader(std::string filename, bool has_header, const List<CSVValueType>& types) :
		CSVReader(has_header, types),
		filename_(filename),
		file_(filename) {

		if (!file_.is_open()) {
			throw std::invalid_argument("Failed to open file " + filename);
		}
	}

	~CSVFileReader() {
		file_.close();
	}

private:
	bool eof() const override;
	std::string readline() override;

	std::string filename_;
	std::ifstream file_;
};

} // namespace CSV

