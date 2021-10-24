#include "symbol.h"
#include <vector>
#include <string>
#ifndef SYMTAB_H
#define SYMTAB_H

class SymbolTable {
	std::vector<Symbol*> syms;
	SymbolTable* parent;

public:
	Symbol* lookup(std::string name);
	void define(std::string name, Symbol* sym);
};

#endif // SYMTAB_H
