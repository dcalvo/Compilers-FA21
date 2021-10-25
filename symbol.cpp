#include "symbol.h"

Symbol::Symbol(const std::string& name, Type* type, SymbolKind kind): name(name), type(type), kind(kind) {}

Symbol::~Symbol() = default;

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

Type* Symbol::get_type() const {
	return type;
}
