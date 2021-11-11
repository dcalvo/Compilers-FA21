#ifndef SYMTAB_H
#define SYMTAB_H
#include "symbol.h"
#include <vector>
#include <string>

class Symbol;

class SymbolTable {
	std::vector<Symbol*> syms;
	SymbolTable* parent = nullptr;
	bool print_symbols;
	int current_offset;

	void get_depth(int& counter);

public:
	SymbolTable(SymbolTable* symtab);
	SymbolTable(bool print_symbols);
	~SymbolTable();
	Symbol* lookup(const std::string& name) const;
	bool define(Symbol* sym);
	std::vector<Symbol*> get_syms() const;
	int get_offset() const;
};

#endif // SYMTAB_H
