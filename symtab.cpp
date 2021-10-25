#include "symtab.h"
#include <iostream>

void SymbolTable::get_depth(int& counter) {
	if (parent) {
		counter++;
		parent->get_depth(counter);
	}
}

// Constructor for nested scopes
SymbolTable::SymbolTable(SymbolTable* symtab): parent(symtab), print_symbols(symtab->print_symbols) {}

SymbolTable::SymbolTable(bool print_symbols): print_symbols(print_symbols) {
	// Define INTEGER primitive
	const auto int_type = new PrimitiveType("INTEGER");
	const auto int_sym = new Symbol("INTEGER", int_type, TYPE);
	syms.push_back(int_sym);
	// Define CHAR primitive
	const auto char_type = new PrimitiveType("CHAR");
	const auto char_sym = new Symbol("CHAR", char_type, TYPE);
	syms.push_back(char_sym);
}

SymbolTable::~SymbolTable() = default;

Symbol* SymbolTable::lookup(const std::string& name) {
	Symbol* sym_ptr = nullptr;
	for (const auto sym : syms) {
		if (sym->get_name() == name) sym_ptr = sym;
	}
	if (!sym_ptr && parent) sym_ptr = parent->lookup(name);
	return sym_ptr;
}

bool SymbolTable::define(Symbol* sym) {
	for (const auto defined_sym : syms) {
		if (sym->get_name() == defined_sym->get_name()) return false; // symbol is already defined in this scope
	}
	syms.push_back(sym);
	if (print_symbols) {
		int depth = 0;
		get_depth(depth);
		std::cout << std::to_string(depth) + "," + sym->get_kind_name() + "," + sym->get_name() + "," + sym->get_type()
			->to_string() << '\n';
	}
	return true;
}

std::vector<Symbol*> SymbolTable::get_syms() const {
	return syms;
}
