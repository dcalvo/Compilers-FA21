#include "type.h"
#include "string"
#ifndef SYMBOL_H
#define SYMBOL_H

enum SymbolKind {
	VAR,
	TYPE,
	CONST
};

class Symbol {
	std::string name;
	Type* type;
	SymbolKind kind;

public:
	Symbol(const std::string& name, Type* type, SymbolKind kind);
	~Symbol();
	std::string get_kind_name() const;
	Type* get_type() const;
};

#endif // SYMBOL_H
