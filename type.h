#include <vector>
#include <string>
#ifndef TYPE_H
#define TYPE_H

class Type {
public:
	virtual std::string to_string() = 0;
};

///////////////////
// PrimitiveType //
///////////////////
class PrimitiveType : public Type {
	std::string name;

public:
	PrimitiveType(const std::string& name);
	~PrimitiveType();
	std::string to_string() override;
};

///////////////
// ArrayType //
///////////////
class ArrayType : public Type {
	Type* type;
	int num_elements;
public:
	ArrayType(Type* type, int num_elements);
	~ArrayType();
	std::string to_string() override;
};

////////////////
// RecordType //
////////////////
struct RecordField {
	Type* type;
	std::string name;
};

class RecordType : public Type {
	std::vector<RecordField*> fields;
public:
	std::string to_string() override;
};

#endif // TYPE_H
