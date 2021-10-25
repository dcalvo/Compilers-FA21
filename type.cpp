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

Type* ArrayType::get_type() {
	return type;
}

std::string ArrayType::to_string() {
	return "ARRAY " + std::to_string(num_elements) + " OF " + type->to_string();
}

////////////////
// RecordType //
////////////////

RecordField::RecordField(Type* type, const std::string& name): type(type), name(name) {}

RecordType::RecordType(const std::vector<RecordField*>& fields): fields(fields) {}

RecordType::~RecordType() = default;

Type* RecordType::get_field(const std::string& name) {
	for (const auto field : fields) {
		if (name == field->name) return field->type;
	}
	return nullptr;
}

std::string RecordType::to_string() {
	std::string out = "RECORD (";
	for (const auto field : fields) {
		out += field->type->to_string() + " x ";
	}
	out.erase(out.length() - 3);
	out += ")";
	return out;
}
