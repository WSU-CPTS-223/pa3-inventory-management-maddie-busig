#pragma once

#include <string>
#include <stdexcept>
#include <type_traits>

#include "CSVValueType.hpp"

namespace CSV {

/**
 * @brief Stores a single CSV value
 *
 * This was written targeting C++11, so unfortunately we don't have access to
 * std::variant. This class implements a poor man's variant for the types that
 * can be parsed from CSV. These are std::string, int, and double.
 *
 * Because unions with non-trivial types (std::string in this case) are
 * complicated to deal with, this class is not move-constructable. It is still
 * copy-constructable however.
 *
 * Constructed by passing a single value to the constructor with the variant's
 * value. This value can then be retrieved using the @c get method. 
 *
 * Extremely scuffed, std::variant really should be used instead.
 */
class CSVValue {
private:
	union CSVValueUnion {
		int val_int;
		double val_double;
		bool val_bool;
		std::string val_string;

		CSVValueUnion() {}
		~CSVValueUnion() {}
	};

public:
	CSVValue() : type_(CSVValueType::CSVInvalid) {}

	template <typename T,
		 typename = typename std::enable_if<
		                          std::is_same<T, int>::value ||
					  std::is_same<T, double>::value ||
					  std::is_same<T, bool>::value ||
					  std::is_same<T, std::string>::value
		                          >::type>
	CSVValue(const T& value) :
		type_(makeValueType<T>()) {

		// Union's default constructor does nothing. Need to manually
		// construct the union. Set the value afterwards.
		constructUnion();
		get<T>() = value;
	}

	CSVValue(const CSVValue& other) :
		type_(other.type_) {
		// Union's default constructor does nothing. Need to manually
		// construct the union.
		constructUnion();

		// Can't implement get without template because of different return types
		// Using switch instead.
		switch (type_) {
		case CSVValueType::CSVString:
			value_.val_string = other.value_.val_string; break;
		case CSVValueType::CSVInt:
			value_.val_int = other.value_.val_int; break;
		case CSVValueType::CSVDouble:
			value_.val_double = other.value_.val_double; break;
		case CSVValueType::CSVBool:
			value_.val_bool = other.value_.val_bool; break;
		default: break;
		}
	}

	~CSVValue() {
		// Union destructor must be explicitly called
		destructUnion();
	}

	CSVValue& operator=(const CSVValue& rhs) {
		if (this == &rhs) { return *this; }

		destructUnion();
		type_ = rhs.type_;
		constructUnion();

		// Can't implement get without template because of different return types
		// Using switch instead.
		switch (type_) {
		case CSVValueType::CSVString:
			value_.val_string = rhs.value_.val_string; break;
		case CSVValueType::CSVInt:
			value_.val_int = rhs.value_.val_int; break;
		case CSVValueType::CSVDouble:
			value_.val_double = rhs.value_.val_double; break;
		case CSVValueType::CSVBool:
			value_.val_bool = rhs.value_.val_bool; break;
		default: break;
		}

		return *this;
	}

	/**
	 * @brief Attempts to get the variant's value
	 *
	 * @tparam T Type of the variant to get. Must match the type of the variant's
	 * value. Must be a valid CSV value type: int, double, or std::string
	 *
	 * @throws std::invalid_argument If @p T does not match the value stored in the variant.
	 *
	 * @returns A reference to the value.
	 */
	template <typename T,
		 typename = typename std::enable_if<
		                          std::is_same<T, int>::value ||
					  std::is_same<T, double>::value ||
					  std::is_same<T, bool>::value ||
					  std::is_same<T, std::string>::value
		                          >::type>
	T& get() {
		const CSVValue& thisref = *this;
		return const_cast<T&>(thisref.get<T>()); // Using const cast to avoid code dupe.
	}

	template <typename T,
		 typename = typename std::enable_if<
		                          std::is_same<T, int>::value ||
					  std::is_same<T, double>::value ||
					  std::is_same<T, bool>::value ||
					  std::is_same<T, std::string>::value,
		 			  void
		                          >::type>
	const T& get() const; // Implemented as specializations

	CSVValueType type() const { return type_; }
	
private:

	/**
	 * @brief Constructs the value union with a default value
	 *
	 * Uses the type stored in member @c value_ to construct the union.
	 * String values are initialized to an empty string, numerical values
	 * are initialized to 0.
	 *
	 * The constructor and destructor of CSVValueUnion do nothing. Must be
	 * manually constructed or destructed, but this depends on the type
	 * that we are constructing, so provided this separate function to do
	 * that work. Value is not 
	 */
	void constructUnion() {
		switch (type_) {
		case CSVValueType::CSVString:
			new (&value_.val_string) std::string();
			break;
		case CSVValueType::CSVDouble:
			value_.val_double = 0.0;
			break;
		case CSVValueType::CSVInt:
			value_.val_int = 0;
			break;
		case CSVValueType::CSVBool:
			value_.val_bool = false;
			break;
		default:
			break;
		}
	}

	/**
	 * @brief Destructs the value union
	 *
	 * For string types calls the string's destructor, otherwise nothing.
	 *
	 * See constructUnion
	 */
	void destructUnion() {
		switch (type_) {
		case CSVValueType::CSVString:
			value_.val_string.~basic_string();
			break;
		default:
			break;
		}
	}

	/**
	 * @brief Changes the type of value stored inside the variant
	 *
	 * @tparam T Type to change the variant to. Must be a valid CSV value type.
	 *
	 * If the type matches the type currently stored in the variant, nothing
	 * is done. Otherwise, the current value is deconstructed and a value
	 * with the new type is constructed.
	 *
	 * Probably a bad idea to have, should just construct & assign a new variant
	 *
	 * @throws std::invalid_argument if the given type is an invalid CSV type.
	 */
	template <typename T>
	void changeType() {
		CSVValueType new_type = makeValueType<T>();

		// Value is not reinitialized if no change to type
		if (new_type == type_) { return; }

		if (new_type == CSVValueType::CSVInvalid) {
			throw std::invalid_argument(std::string("Invalid CSV variant type: ")
			                            + typeid(T).name()
			                            + ". Expected std::string, int, or double");
		}
		
		destructUnion();
		type_ = new_type;
		constructUnion();
	}


	CSVValueType type_;
	CSVValueUnion value_;
};

// Template specialization needed to avoid errors from incompatible return types
// with switch statement. Could avoid with c++17 if constexpr, but targeting c++11 :c
template <>
inline const std::string& CSVValue::get<std::string, void>() const {
	CSVValueType requested_type = makeValueType<std::string>();

	if (requested_type != type_) {
		throw std::invalid_argument(std::string("CSV value type mismatch: got ")
				+ typeid(std::string).name()
				+ ". Expected " + valueTypeToString(type_));
	}

	return value_.val_string;
}
template <>
inline const int& CSVValue::get<int, void>() const {
	CSVValueType requested_type = makeValueType<int>();

	if (requested_type != type_) {
		throw std::invalid_argument(std::string("CSV value type mismatch: got ")
				+ typeid(int).name()
				+ ". Expected " + valueTypeToString(type_));
	}

	return value_.val_int;
}
template <>
inline const double& CSVValue::get<double, void>() const {
	CSVValueType requested_type = makeValueType<double>();

	if (requested_type != type_) {
		throw std::invalid_argument(std::string("CSV value type mismatch: got ")
				+ typeid(double).name()
				+ ". Expected " + valueTypeToString(type_));
	}

	return value_.val_double;
}
template <>
inline const bool& CSVValue::get<bool, void>() const {
	CSVValueType requested_type = makeValueType<bool>();

	if (requested_type != type_) {
		throw std::invalid_argument(std::string("CSV value type mismatch: got ")
				+ typeid(bool).name()
				+ ". Expected " + valueTypeToString(type_));
	}

	return value_.val_bool;
}

} // namespace CSV

