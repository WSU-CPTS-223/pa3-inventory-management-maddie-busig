#pragma once

#include "CSVData.hpp"

namespace CSV {

class CSVWriter {
public:
	CSVWriter(bool write_header = true) :
		write_header_(write_header) {}

	virtual ~CSVWriter() {}

	void write(const CSVData& csv);

private:
	virtual void writeline(std::string line) = 0;

	bool write_header_;
};

}

