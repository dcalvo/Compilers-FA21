#include "symtab.h"
#include <iostream>

void SymbolTable::get_depth(int& counter) {
	if (parent) {
		counter++;
		parent->get_depth(counter);
	}
}

// Constructor for nested scopes
SymbolTable::SymbolTable(SymbolTable* symtab): parent(symtab), print_symbols(symtab->print_symbols),
                                               current_offset(0) {}

SymbolTable::SymbolTable(bool print_symbols): print_symbols(print_symbols), current_offset(0) {
	// Define INTEGER primitive
	const auto int_type = new PrimitiveType("INTEGER", 8);
	const auto int_sym = new Symbol("INTEGER", int_type, TYPE);
	syms.push_back(int_sym);
	// Define CHAR primitive
	const auto char_type = new PrimitiveType("CHAR", 1);
	const auto char_sym = new Symbol("CHAR", char_type, TYPE);
	syms.push_back(char_sym);
}

SymbolTable::~SymbolTable() = default;

Symbol* SymbolTable::lookup(const std::string& name) const {
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
	// calculate offset in memory for arrays and records
	const auto int_type = lookup("INTEGER")->get_type();
	const auto char_type = lookup("CHAR")->get_type();
	if (sym->get_kind() == VAR) {
		int depth = 0;
		get_depth(depth);
		// the var should either be nested (in a record) or a non-primitive local variable
		if (depth || (sym->get_type() != int_type && sym->get_type() != char_type)) {
			sym->set_offset(current_offset);
			current_offset += sym->get_type()->get_size();
		}

	}
	else if (sym->get_kind() == CONST) {
		// we'll store constants in memory
		sym->set_offset(current_offset);
		current_offset += sym->get_type()->get_size();
	}
	// add symbol
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

int SymbolTable::get_offset() const {
	return current_offset;
}
