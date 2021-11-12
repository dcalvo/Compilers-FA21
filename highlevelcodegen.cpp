#include "highlevelcodegen.h"
#include <cassert>
#include <iostream>
#include "node.h"
#include "grammar_symbols.h"
#include "ast.h"

HighLevelCodeGen::HighLevelCodeGen(SymbolTable* symtab) {
	_iseq = new InstructionSequence();
	_printer = new PrintHighLevelInstructionSequence(_iseq);
	this->symtab = symtab;
	const auto int_type = symtab->lookup("INTEGER")->get_type();
	const auto char_type = symtab->lookup("CHAR")->get_type();
	for (const auto sym : symtab->get_syms()) {
		if (sym->get_kind() == VAR && (sym->get_type() == int_type || sym->get_type() == char_type))
			sym->set_vreg(next_vreg());
		else
			sym->set_vreg(-1);
	}
}

HighLevelCodeGen::~HighLevelCodeGen() {}

InstructionSequence* HighLevelCodeGen::get_iseq() {
	return _iseq;
}

int HighLevelCodeGen::next_vreg() {
	int vregs = _vreg_count++;
	_max_vreg_count = std::max(_vreg_count, _max_vreg_count);
	return vregs;
}

void HighLevelCodeGen::free_vreg() {
	_vreg_count--;
}

int HighLevelCodeGen::get_vreg_count() {
	return _max_vreg_count;
}

std::string HighLevelCodeGen::next_label() {
	return ".L" + std::to_string(_label_count++);
}

void HighLevelCodeGen::emit(Instruction* const ins) {
	ins->set_comment(_printer->format_instruction(ins));
	_iseq->add_instruction(ins);
}

void HighLevelCodeGen::visit(struct Node* ast) {
	int tag = node_get_tag(ast);
	switch (tag) {
	case AST_PROGRAM:
		visit_program(ast);
		break;
	case AST_ADD:
		visit_add(ast);
		break;
	case AST_SUBTRACT:
		visit_subtract(ast);
		break;
	case AST_MULTIPLY:
		visit_multiply(ast);
		break;
	case AST_DIVIDE:
		visit_divide(ast);
		break;
	case AST_MODULUS:
		visit_modulus(ast);
		break;
	case AST_NEGATE:
		visit_negate(ast);
		break;
	case AST_INT_LITERAL:
		visit_int_literal(ast);
		break;
	case AST_INSTRUCTIONS:
		visit_instructions(ast);
		break;
	case AST_ASSIGN:
		visit_assign(ast);
		break;
	case AST_IF:
		visit_if(ast);
		break;
	case AST_IF_ELSE:
		visit_if_else(ast);
		break;
	case AST_REPEAT:
		visit_repeat(ast);
		break;
	case AST_WHILE:
		visit_while(ast);
		break;
	case AST_COMPARE_EQ:
		visit_compare_eq(ast);
		break;
	case AST_COMPARE_NEQ:
		visit_compare_neq(ast);
		break;
	case AST_COMPARE_LT:
		visit_compare_lt(ast);
		break;
	case AST_COMPARE_LTE:
		visit_compare_lte(ast);
		break;
	case AST_COMPARE_GT:
		visit_compare_gt(ast);
		break;
	case AST_COMPARE_GTE:
		visit_compare_gte(ast);
		break;
	case AST_WRITE:
		visit_write(ast);
		break;
	case AST_READ:
		visit_read(ast);
		break;
	case AST_VAR_REF:
		visit_var_ref(ast);
		break;
	case AST_ARRAY_ELEMENT_REF:
		visit_array_element_ref(ast);
		break;
	case AST_FIELD_REF:
		visit_field_ref(ast);
		break;
	case AST_IDENTIFIER_LIST:
		visit_identifier_list(ast);
		break;
	case AST_EXPRESSION_LIST:
		visit_expression_list(ast);
		break;
	case NODE_TOK_IDENT:
		visit_identifier(ast);
		break;
	default:
		assert(false); // unknown AST node type
	}
}

// Transforms the given op if it a memory reference by adding a load instruction and returning a new op
Operand* HighLevelCodeGen::load_op(Operand* op) {
	if (!op->is_memref()) return op;
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto ins = new Instruction(HINS_LOAD_INT, *destreg, *op);
	emit(ins);
	return destreg;
}

void HighLevelCodeGen::visit_program(struct Node* ast) {
	visit(ast->get_kid(2)); // visit only the instructions
}

void HighLevelCodeGen::visit_add(struct Node* ast) {

	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	const auto ins = new Instruction(HINS_INT_ADD, *destreg, *leftop, *rightop);
	emit(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_subtract(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	const auto ins = new Instruction(HINS_INT_SUB, *destreg, *leftop, *rightop);
	emit(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_multiply(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	const auto ins = new Instruction(HINS_INT_MUL, *destreg, *leftop, *rightop);
	emit(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_divide(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	const auto ins = new Instruction(HINS_INT_DIV, *destreg, *leftop, *rightop);
	emit(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_modulus(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	const auto ins = new Instruction(HINS_INT_MOD, *destreg, *leftop, *rightop);
	emit(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_negate(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto op = load_op(ast->get_kid(0)->get_operand());
	const auto ins = new Instruction(HINS_INT_NEGATE, *destreg, *op);
	emit(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_int_literal(struct Node* ast) {
	// example from the assignment instructions
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const Operand immval(OPERAND_INT_LITERAL, ast->get_ival());
	const auto ins = new Instruction(HINS_LOAD_ICONST, *destreg, immval);
	emit(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_instructions(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void HighLevelCodeGen::visit_assign(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	Instruction* ins;
	if (leftop->is_memref()) {
		ins = new Instruction(HINS_STORE_INT, *leftop, *rightop);
		// free the vregs used to calculate the memref
		// also free the RHS vreg since it's in memory now
		for (int i = 0; i < ast->get_kid(0)->get_num_vregs_used() + 1; ++i) free_vreg();
	}
	else ins = new Instruction(HINS_MOV, *leftop, *rightop);
	emit(ins);
}

void HighLevelCodeGen::visit_if(struct Node* ast) {
	// example from the assignment instructions
	const auto condition_ast = ast->get_kid(0);
	const auto then_ast = ast->get_kid(1);
	const auto out_label = next_label();
	condition_ast->set_inverted(true);
	condition_ast->set_operand(new Operand(out_label));
	visit(condition_ast);
	visit(then_ast);
	_iseq->define_label(out_label);
}

void HighLevelCodeGen::visit_if_else(struct Node* ast) {
	// should generate a block of HL instructions like 
	// inverted conditional (jump <else_label>)
	// then statement
	// jump <out_label>
	// <else_label>
	// else statement
	// <out_label>
	const auto condition_ast = ast->get_kid(0);
	const auto then_ast = ast->get_kid(1);
	const auto else_ast = ast->get_kid(2);
	const auto else_label = next_label();
	const auto out_label = next_label();
	condition_ast->set_inverted(true);
	condition_ast->set_operand(new Operand(else_label));
	visit(condition_ast);
	visit(then_ast);
	const auto ins = new Instruction(HINS_JUMP, Operand(out_label));
	emit(ins);
	_iseq->define_label(else_label);
	visit(else_ast);
	// in case we're in a nested control block
	if (_iseq->has_label_at_end())
		emit(new Instruction(HINS_NOP));
	_iseq->define_label(out_label);
}

void HighLevelCodeGen::visit_repeat(struct Node* ast) {
	const auto instructions_ast = ast->get_kid(0);
	const auto condition_ast = ast->get_kid(1);
	const auto instructions_label = next_label();
	_iseq->define_label(instructions_label);
	visit(instructions_ast);
	condition_ast->set_operand(new Operand(instructions_label));
	condition_ast->set_inverted(true); // we want to jump when the comparison is false
	visit(condition_ast);
}

void HighLevelCodeGen::visit_while(struct Node* ast) {
	const auto condition_ast = ast->get_kid(0);
	const auto instructions_ast = ast->get_kid(1);
	const auto condition_label = next_label();
	const auto instructions_label = next_label();
	// condition needs to know where to jump to if successful
	condition_ast->set_operand(new Operand(instructions_label));
	const auto ins = new Instruction(HINS_JUMP, Operand(condition_label));
	emit(ins);
	_iseq->define_label(instructions_label);
	visit(instructions_ast);
	// in case we're in a nested control block
	if (_iseq->has_label_at_end())
		emit(new Instruction(HINS_NOP));
	_iseq->define_label(condition_label);
	visit(condition_ast);
}

void HighLevelCodeGen::visit_compare_eq(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	emit(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JNE, *ast->get_operand())
		      : new Instruction(HINS_JE, *ast->get_operand());
	emit(ins);
}

void HighLevelCodeGen::visit_compare_neq(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	emit(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JE, *ast->get_operand())
		      : new Instruction(HINS_JNE, *ast->get_operand());
	emit(ins);
}

void HighLevelCodeGen::visit_compare_lt(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	emit(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JGTE, *ast->get_operand())
		      : new Instruction(HINS_JLT, *ast->get_operand());
	emit(ins);
}

void HighLevelCodeGen::visit_compare_lte(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	emit(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JGT, *ast->get_operand())
		      : new Instruction(HINS_JLTE, *ast->get_operand());
	emit(ins);
}

void HighLevelCodeGen::visit_compare_gt(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	emit(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JLTE, *ast->get_operand())
		      : new Instruction(HINS_JGT, *ast->get_operand());
	emit(ins);
}

void HighLevelCodeGen::visit_compare_gte(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = load_op(ast->get_kid(0)->get_operand());
	const auto rightop = load_op(ast->get_kid(1)->get_operand());
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	emit(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JLT, *ast->get_operand())
		      : new Instruction(HINS_JGTE, *ast->get_operand());
	emit(ins);
}

void HighLevelCodeGen::visit_write(struct Node* ast) {
	recur_on_children(ast);
	auto op = ast->get_kid(0)->get_operand();
	Instruction* ins;
	if (op->is_memref()) {
		const auto writereg = new Operand(OPERAND_VREG, next_vreg());
		ins = new Instruction(HINS_LOAD_INT, *writereg, *op);
		emit(ins);
		// free the vregs used to calculate the memref
		// also free the temporary writereg
		for (int i = 0; i < ast->get_kid(0)->get_num_vregs_used() + 1; ++i) free_vreg();
		// change the write op to the new writereg
		op = writereg;
	}
	ins = new Instruction(HINS_WRITE_INT, *op);
	emit(ins);
}

void HighLevelCodeGen::visit_read(struct Node* ast) {
	recur_on_children(ast);
	auto op = ast->get_kid(0)->get_operand();
	const auto readreg = new Operand(OPERAND_VREG, next_vreg());
	auto ins = new Instruction(HINS_READ_INT, *readreg);
	emit(ins);
	if (op->is_memref()) {
		ins = new Instruction(HINS_STORE_INT, *op, *readreg);
		// free the vregs used to calculate the memref
		for (int i = 0; i < ast->get_kid(0)->get_num_vregs_used(); ++i) free_vreg();
	}
	else ins = new Instruction(HINS_MOV, *op, *readreg);
	emit(ins);
	free_vreg(); // no need to keep the temporary read register around
}

void HighLevelCodeGen::visit_var_ref(struct Node* ast) {
	const struct Node* tok_identifier = ast->get_kid(0);
	const auto sym = symtab->lookup(tok_identifier->get_str());
	// symbol is a local variable
	if (sym->get_vreg() >= 0) {
		const auto destreg = new Operand(OPERAND_VREG, sym->get_vreg());
		ast->set_operand(destreg);
		return;
	}
	// get the base address
	auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const int base_addr = sym->get_offset();
	auto ins = new Instruction(HINS_LOCALADDR, *destreg, Operand(OPERAND_INT_LITERAL, base_addr));
	emit(ins);
	if (sym->get_kind() == CONST) *destreg = destreg->to_memref();
	ast->set_operand(destreg);
	ast->set_vregs_used(1); // base addr
}

void HighLevelCodeGen::visit_array_element_ref(struct Node* ast) {
	recur_on_children(ast);
	const auto identifier_ast = ast->get_kid(0);
	const auto index_ast = ast->get_kid(1);
	// calculate the offset
	const int elem_size = dynamic_cast<ArrayType*>(identifier_ast->get_type())->get_type()->get_size();
	const auto offsetreg = Operand(OPERAND_VREG, next_vreg());
	auto ins = new Instruction(HINS_INT_MUL, offsetreg, *index_ast->get_operand(),
	                           Operand(OPERAND_INT_LITERAL, elem_size));
	emit(ins);
	// add the offset to base
	auto destreg = new Operand(OPERAND_VREG, next_vreg());
	ins = new Instruction(HINS_INT_ADD, *destreg, *identifier_ast->get_operand(), offsetreg);
	emit(ins);
	// set the memref for this node
	*destreg = destreg->to_memref();
	ast->set_operand(destreg);
	ast->set_vregs_used(identifier_ast->get_num_vregs_used() + 2); // offset and base+offset
}

void HighLevelCodeGen::visit_field_ref(struct Node* ast) {
	recur_on_children(ast);
	const auto identifier_ast = ast->get_kid(0);
	const auto field_ast = ast->get_kid(1);
	const auto record_symtab = dynamic_cast<RecordType*>(identifier_ast->get_type())->get_symtab();
	const int offset = record_symtab->lookup(field_ast->get_str())->get_offset();
	// add offset to base
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto ins = new Instruction(HINS_INT_ADD, *destreg, *identifier_ast->get_operand(),
	                                 Operand(OPERAND_INT_LITERAL, offset));
	emit(ins);
	// set the memref for this node
	*destreg = destreg->to_memref();
	ast->set_operand(destreg);
	ast->set_vregs_used(identifier_ast->get_num_vregs_used() + 1); // base+offset
}

void HighLevelCodeGen::visit_identifier_list(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void HighLevelCodeGen::visit_expression_list(struct Node* ast) {
	recur_on_children(ast);
	// pass info up the tree
	ast->set_operand(ast->get_kid(0)->get_operand());
}

void HighLevelCodeGen::visit_identifier(Node* ast) {
	recur_on_children(ast); // default behavior
}

void HighLevelCodeGen::recur_on_children(struct Node* ast) {
	int num_kids = node_get_num_kids(ast);
	for (int i = 0; i < num_kids; i++) {
		visit(node_get_kid(ast, i));
	}
}
