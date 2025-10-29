#include "CSV/CSVFileReader.hpp"

namespace CSV {

bool CSVFileReader::eof() const {
	return file_.eof();
}

std::string CSVFileReader::readline() {
	std::string line;
	std::getline(file_, line);
	return line;
}

} // namespace CSV

