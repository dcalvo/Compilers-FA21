#ifndef TYPE_H
#define TYPE_H

struct Type {};

struct PrimitiveType : Type {
	std::string name;
};

struct ArrayType : Type {
	Type* type;
	int num_elements;
};

struct RecordField {
	Type* type;
	std::string name;
};

struct RecordType : Type {
	std::vector<RecordField*> fields;
};

#endif // TYPE_H
