#ifndef SYMBOL_H
#define SYMBOL_H
#include "type.h"
#include "string"

class Type;

enum SymbolKind {
	VAR,
	TYPE,
	CONST
};

class Symbol {
	std::string name;
	Type* type;
	SymbolKind kind;
	int ival;
	int vreg; // virtual register for code generation
	int offset; // offset within the current symbol table

public:
	Symbol(const std::string& name, Type* type, SymbolKind kind);
	~Symbol();
	std::string get_name() const;
	Type* get_type() const;
	std::string get_kind_name() const;
	SymbolKind get_kind() const;
	void set_ival(int ival);
	int get_ival() const;
	void set_vreg(int vreg);
	int get_vreg() const;
	void set_offset(int offset);
	int get_offset() const;
};

#endif // SYMBOL_H
