#ifndef TYPE_H
#define TYPE_H
#include <vector>
#include <string>
#include "symtab.h"

class SymbolTable;

class Type {
public:
	virtual std::string to_string() = 0;
	virtual int get_size() = 0;
};

///////////////////
// PrimitiveType //
///////////////////

class PrimitiveType : public Type {
	std::string name;
	int size; // in bytes

public:
	PrimitiveType(const std::string& name, int size);
	~PrimitiveType();
	std::string to_string() override;
	int get_size() override;
};

///////////////
// ArrayType //
///////////////

class ArrayType : public Type {
	Type* type;
	int num_elements;
	int size; // in bytes

public:
	ArrayType(Type* type, int num_elements);
	~ArrayType();
	Type* get_type();
	std::string to_string() override;
	int get_size() override;
};

////////////////
// RecordType //
////////////////

struct RecordField {
	Type* type;
	std::string name;

	RecordField(Type* type, const std::string& name);
};

class RecordType : public Type {
	std::vector<RecordField*> fields;
	int size; // in bytes
	SymbolTable* symtab;

public:
	RecordType(const std::vector<RecordField*>& fields, SymbolTable* symtab);
	~RecordType();
	Type* get_field(const std::string& name);
	std::string to_string() override;
	int get_size() override;
	SymbolTable* get_symtab();
};

#endif // TYPE_H
