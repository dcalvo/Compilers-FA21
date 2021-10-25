#ifndef SYMBOL_H
#define SYMBOL_H
#include "type.h"
#include "string"

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
	std::string get_name() const;
	Type* get_type() const;
	std::string get_kind_name() const;
	SymbolKind get_kind() const;
};

#endif // SYMBOL_H
