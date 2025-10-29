#include "CSV/CSVFileWriter.hpp"

namespace CSV {

void CSVFileWriter::writeline(std::string line) {
	file_.write(line.c_str(), line.size());
	file_ << std::endl;
}

}

