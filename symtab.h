#ifndef SYMTAB_H
#define SYMTAB_H
#include "symbol.h"
#include <vector>
#include <string>

class SymbolTable {
	std::vector<Symbol*> syms;
	SymbolTable* parent = nullptr;
	bool print_symbols;
	void get_depth(int& counter);

public:
	SymbolTable(SymbolTable* symtab);
	SymbolTable(bool print_symbols);
	~SymbolTable();
	Symbol* lookup(const std::string& name);
	bool define(Symbol* sym);
	std::vector<Symbol*> get_syms() const;
};

#endif // SYMTAB_H
