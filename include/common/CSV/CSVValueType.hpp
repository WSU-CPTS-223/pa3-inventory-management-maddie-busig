#pragma once

#include <typeinfo>
#include <string>

namespace CSV {

enum struct CSVValueType {
	CSVInvalid,
	CSVString,
	CSVInt,
	CSVDouble,
	CSVBool
};

template <typename T>
CSVValueType makeValueType() {
	if (typeid(T) == typeid(std::string)) {
		return CSVValueType::CSVString;
	} else if (typeid(T) == typeid(int)) {
		return CSVValueType::CSVInt;
	} else if (typeid(T) == typeid(double)) {
		return CSVValueType::CSVDouble;
	} else if (typeid(T) == typeid(bool)) {
		return CSVValueType::CSVBool;
	} else {
		return CSVValueType::CSVInvalid;
	}
}

inline std::string valueTypeToString(CSVValueType type) {
	switch (type) {
	case CSVValueType::CSVString: return "std::string"; break;
	case CSVValueType::CSVInt: return "int"; break;
	case CSVValueType::CSVDouble: return "double"; break;
	case CSVValueType::CSVBool: return "bool"; break;
	default: return "invalid"; break;
	}
}

} // namespace CSV

