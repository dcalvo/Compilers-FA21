#include "type.h"

#include <iostream>

///////////////////
// PrimitiveType //
///////////////////
PrimitiveType::PrimitiveType(const std::string& name): name(name) {}

PrimitiveType::~PrimitiveType() = default;

std::string PrimitiveType::to_string() {
	return name;
}

///////////////
// ArrayType //
///////////////
ArrayType::ArrayType(Type* type, int num_elements): type(type), num_elements(num_elements) {}

ArrayType::~ArrayType() = default;

std::string ArrayType::to_string() {
	return "ARRAY " + std::to_string(num_elements) + " OF " + type->to_string();
}

////////////////
// RecordType //
////////////////
