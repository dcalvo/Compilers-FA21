#ifndef HIGHLEVELCODEGEN_H
#define HIGHLEVELCODEGEN_H
#include "cfg.h"
#include "symtab.h"

class HighLevelCodeGen {
	InstructionSequence* _iseq;
	SymbolTable* symtab;
	int _vreg_count = 0;
public:
	HighLevelCodeGen(SymbolTable* symtab);
	virtual ~HighLevelCodeGen();

	InstructionSequence* get_iseq();
	int next_vreg();
	void free_vreg();

	void visit(struct Node* ast);

	virtual void visit_program(struct Node* ast);
	virtual void visit_add(struct Node* ast);
	virtual void visit_subtract(struct Node* ast);
	virtual void visit_multiply(struct Node* ast);
	virtual void visit_divide(struct Node* ast);
	virtual void visit_modulus(struct Node* ast);
	virtual void visit_negate(struct Node* ast);
	virtual void visit_int_literal(struct Node* ast);
	virtual void visit_instructions(struct Node* ast);
	virtual void visit_assign(struct Node* ast);
	virtual void visit_if(struct Node* ast);
	virtual void visit_if_else(struct Node* ast);
	virtual void visit_repeat(struct Node* ast);
	virtual void visit_while(struct Node* ast);
	virtual void visit_compare_eq(struct Node* ast);
	virtual void visit_compare_neq(struct Node* ast);
	virtual void visit_compare_lt(struct Node* ast);
	virtual void visit_compare_lte(struct Node* ast);
	virtual void visit_compare_gt(struct Node* ast);
	virtual void visit_compare_gte(struct Node* ast);
	virtual void visit_write(struct Node* ast);
	virtual void visit_read(struct Node* ast);
	virtual void visit_var_ref(struct Node* ast);
	virtual void visit_array_element_ref(struct Node* ast);
	virtual void visit_field_ref(struct Node* ast);
	virtual void visit_identifier_list(struct Node* ast);
	virtual void visit_expression_list(struct Node* ast);
	virtual void visit_identifier(struct Node* ast);

	virtual void recur_on_children(struct Node* ast);
};

#endif // HIGHLEVELCODEGEN_H
