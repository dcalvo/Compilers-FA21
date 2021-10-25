#include "symbol.h"

Symbol::Symbol(const std::string& name, Type* type, SymbolKind kind): name(name), type(type), kind(kind) {}

Symbol::~Symbol() = default;

std::string Symbol::get_name() const {
	return name;
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

Type* Symbol::get_type() const {
	return type;
}
