#pragma once

#include "List.hpp"

#include "CSVRow.hpp"
#include "CSVTuple.hpp"

namespace CSV {

class CSVData {
public:
	CSVRow& header() { return header_; }
	const CSVRow& header() const { return header_; }

	List<CSVTuple>& rows() { return rows_; }
	const List<CSVTuple>& rows() const { return rows_; }

private:
	CSVRow header_;
	List<CSVTuple> rows_;
};

} // namespace CSV

