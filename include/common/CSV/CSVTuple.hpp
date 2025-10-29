#pragma once

#include "List.hpp"

#include "CSVValue.hpp"

namespace CSV {

class CSVTuple {
public:
	using Iterator = List<CSVValue>::Iterator;
	using ConstIterator = List<CSVValue>::ConstIterator;

	CSVTuple() : values_() {}
	CSVTuple(const List<CSVValue>& values) :
		values_(values) {}

	CSVValue& operator[](size_t idx) { return values_[idx]; }
	const CSVValue& operator[](size_t idx) const { return values_[idx]; }

	CSVValue& operator[](std::string idx) { }
	const CSVValue& operator[](std::string idx) const { }

	List<CSVValue>& values() { return values_; }
	const List<CSVValue>& values() const { return values_; }

	Iterator begin() { return values_.begin(); }
	ConstIterator begin() const { return values_.begin(); }
	ConstIterator cbegin() const { return values_.begin(); }

	Iterator end() { return values_.end(); }
	ConstIterator end() const { return values_.end(); }
	ConstIterator cend() const { return values_.end(); }

private:
	// Assuming we're not allowed to use std::vector or anything, so using List
	List<CSVValue> values_; // :c
};

} // namespace CSV

