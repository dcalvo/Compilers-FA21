#include "symbol.h"

Symbol::Symbol(const std::string& name, Type* type, SymbolKind kind): name(name), type(type), kind(kind) {}

Symbol::~Symbol() = default;

std::string Symbol::get_name() const {
	return name;
}

Type* Symbol::get_type() const {
	return type;
}

std::string Symbol::get_kind_name() const {
	switch (kind) {
	case 0:
		return "VAR";
	case 1:
		return "TYPE";
	case 2:
		return "CONST";
	default:
		return "<unknown SymbolKind>";
	}
}

SymbolKind Symbol::get_kind() const {
	return kind;
}

void Symbol::set_ival(int ival) {
	this->ival = ival;
}

int Symbol::get_ival() const {
	return ival;
}

void Symbol::set_vreg(int vreg) {
	this->vreg = vreg;
}

int Symbol::get_vreg() const {
	return vreg;
}

void Symbol::set_offset(int offset) {
	this->offset = offset;
}

int Symbol::get_offset() const {
	return offset;
}
