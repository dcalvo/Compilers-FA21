#include "type.h"
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
};

#endif // SYMBOL_H
