#include "symtab.h"
#include <iostream>

SymbolTable::SymbolTable(bool print_symbols): print_symbols(print_symbols) {
	// Define INTEGER primitive
	const auto int_type = new PrimitiveType("INTEGER");
	const auto int_sym = new Symbol("INTEGER", int_type, TYPE);
	syms["INTEGER"] = int_sym;
	// Define CHAR primitive
	const auto char_type = new PrimitiveType("CHAR");
	const auto char_sym = new Symbol("CHAR", char_type, TYPE);
	syms["CHAR"] = char_sym;
}

SymbolTable::~SymbolTable() = default;

Symbol* SymbolTable::lookup(const std::string& name) {
	return syms.count(name) ? syms[name] : nullptr;
}

bool SymbolTable::define(const std::string& name, Symbol* sym) {
	if (syms.count(name)) return false; // symbol is already defined
	syms[name] = sym;
	if (print_symbols)
		std::cout << "x," + sym->get_kind_name() + "," + name + "," + sym->get_type()->to_string() << '\n';
	return true;
}
