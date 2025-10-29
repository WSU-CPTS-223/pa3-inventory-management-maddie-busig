#pragma once

#include <string>

#include "List.hpp"

namespace CSV {

class CSVRow {
public:
	CSVRow() {}

	CSVRow(const std::string& line) {
		parseLine(line);
	}

	List<std::string>& tokens() { return values_; }
	const List<std::string>& tokens() const { return values_; }

	std::string& operator[](size_t idx) { return values_[idx]; }
	const std::string& operator[](size_t idx) const { return values_[idx]; }

	List<std::string>::Iterator begin() { return values_.begin(); }
	List<std::string>::ConstIterator begin() const { return values_.begin(); }
	List<std::string>::ConstIterator cbegin() const { return values_.begin(); }

	List<std::string>::Iterator end() { return values_.end(); }
	List<std::string>::ConstIterator end() const { return values_.end(); }
	List<std::string>::ConstIterator cend() const { return values_.end(); }

private:
	void parseLine(const std::string& line);

	// Assuming we're not allowed to use std::vector at all, so using List
	List<std::string> values_;
};

} // namespace CSV

