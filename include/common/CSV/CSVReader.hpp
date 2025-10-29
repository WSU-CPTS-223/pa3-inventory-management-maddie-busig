#pragma once

#include <string>

#include "CSVData.hpp"
#include "CSVValueType.hpp"

template <typename T>
class List;

namespace CSV {

class CSVReader {
public:
	CSVReader(bool has_header = true) :
		has_header_(has_header),
		types_() {}

	CSVReader(bool has_header, const List<CSVValueType>& types) :
		has_header_(has_header),
		types_(types) {}

	virtual ~CSVReader() {}

	CSVData read();

private:
	virtual bool eof() const = 0;
	virtual std::string readline() = 0;

	bool has_header_;
	List<CSVValueType> types_;
};

} // namespace CSV

