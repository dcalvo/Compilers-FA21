#include "symbol.h"
#include <unordered_map>
#include <string>
#ifndef SYMTAB_H
#define SYMTAB_H

class SymbolTable {
	std::unordered_map<std::string, Symbol*> syms;
	SymbolTable* parent = nullptr;
	bool print_symbols;

public:
	SymbolTable(bool print_symbols);
	~SymbolTable();
	Symbol* lookup(const std::string& name);
	bool define(const std::string& name, Symbol* sym);
};

#endif // SYMTAB_H
