#include "highlevelcodegen.h"
#include <cassert>
#include <iostream>
#include "node.h"
#include "grammar_symbols.h"
#include "ast.h"
#include "highlevel.h"

HighLevelCodeGen::HighLevelCodeGen(SymbolTable* symtab) {
	_iseq = new InstructionSequence();
	this->symtab = symtab;
	const auto int_type = symtab->lookup("INTEGER")->get_type();
	const auto char_type = symtab->lookup("CHAR")->get_type();
	for (const auto sym : symtab->get_syms()) {
		if (sym->get_kind() == VAR) {
			if (sym->get_type() == int_type || sym->get_type() == char_type) sym->set_vreg(next_vreg());
		}
	}
}

HighLevelCodeGen::~HighLevelCodeGen() {}

InstructionSequence* HighLevelCodeGen::get_iseq() {
	return _iseq;
}

int HighLevelCodeGen::next_vreg() {
	return _vreg_count++;
}

void HighLevelCodeGen::free_vreg() {
	_vreg_count--;
}

std::string HighLevelCodeGen::next_label() {
	return ".L" + std::to_string(_label_count++);
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

void HighLevelCodeGen::visit_program(struct Node* ast) {
	visit(ast->get_kid(2)); // visit only the instructions
}

void HighLevelCodeGen::visit_add(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	const auto ins = new Instruction(HINS_INT_ADD, *destreg, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_subtract(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	const auto ins = new Instruction(HINS_INT_SUB, *destreg, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_multiply(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	const auto ins = new Instruction(HINS_INT_MUL, *destreg, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_divide(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	const auto ins = new Instruction(HINS_INT_DIV, *destreg, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_modulus(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	const auto ins = new Instruction(HINS_INT_MOD, *destreg, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_negate(struct Node* ast) {
	recur_on_children(ast);
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const auto op = ast->get_kid(0)->get_operand();
	const auto ins = new Instruction(HINS_INT_NEGATE, *destreg, *op);
	_iseq->add_instruction(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_int_literal(struct Node* ast) {
	// example from the assignment instructions
	const auto destreg = new Operand(OPERAND_VREG, next_vreg());
	const Operand immval(OPERAND_INT_LITERAL, ast->get_ival());
	const auto ins = new Instruction(HINS_LOAD_ICONST, *destreg, immval);
	_iseq->add_instruction(ins);
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_instructions(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void HighLevelCodeGen::visit_assign(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	const auto ins = new Instruction(HINS_MOV, *leftop, *rightop);
	_iseq->add_instruction(ins);
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
	_iseq->add_instruction(ins);
	_iseq->define_label(else_label);
	visit(else_ast);
	_iseq->define_label(out_label);
}

void HighLevelCodeGen::visit_repeat(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void HighLevelCodeGen::visit_while(struct Node* ast) {
	const auto condition_ast = ast->get_kid(0);
	const auto instructions_ast = ast->get_kid(1);
	const auto condition_label = next_label();
	const auto instructions_label = next_label();
	// condition needs to know where to jump to if successful
	condition_ast->set_operand(new Operand(instructions_label));
	const auto ins = new Instruction(HINS_JUMP, Operand(condition_label));
	_iseq->add_instruction(ins);
	_iseq->define_label(instructions_label);
	visit(instructions_ast);
	_iseq->define_label(condition_label);
	visit(condition_ast);
}

void HighLevelCodeGen::visit_compare_eq(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JNE, *ast->get_operand())
		      : new Instruction(HINS_JE, *ast->get_operand());
	_iseq->add_instruction(ins);
}

void HighLevelCodeGen::visit_compare_neq(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JE, *ast->get_operand())
		      : new Instruction(HINS_JNE, *ast->get_operand());
	_iseq->add_instruction(ins);
}

void HighLevelCodeGen::visit_compare_lt(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JGTE, *ast->get_operand())
		      : new Instruction(HINS_JLT, *ast->get_operand());
	_iseq->add_instruction(ins);
}

void HighLevelCodeGen::visit_compare_lte(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JGT, *ast->get_operand())
		      : new Instruction(HINS_JLTE, *ast->get_operand());
	_iseq->add_instruction(ins);
}

void HighLevelCodeGen::visit_compare_gt(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JLTE, *ast->get_operand())
		      : new Instruction(HINS_JGT, *ast->get_operand());
	_iseq->add_instruction(ins);
}

void HighLevelCodeGen::visit_compare_gte(struct Node* ast) {
	recur_on_children(ast);
	const auto leftop = ast->get_kid(0)->get_operand();
	const auto rightop = ast->get_kid(1)->get_operand();
	auto ins = new Instruction(HINS_INT_COMPARE, *leftop, *rightop);
	_iseq->add_instruction(ins);
	ins = ast->is_inverted()
		      ? new Instruction(HINS_JLT, *ast->get_operand())
		      : new Instruction(HINS_JGTE, *ast->get_operand());
	_iseq->add_instruction(ins);
}

void HighLevelCodeGen::visit_write(struct Node* ast) {
	recur_on_children(ast);
	const auto op = ast->get_kid(0)->get_operand();
	const auto ins = new Instruction(HINS_WRITE_INT, *op);
	_iseq->add_instruction(ins);
}

void HighLevelCodeGen::visit_read(struct Node* ast) {
	recur_on_children(ast);
	const auto readreg = new Operand(OPERAND_VREG, next_vreg());
	auto ins = new Instruction(HINS_READ_INT, *readreg);
	_iseq->add_instruction(ins);
	const auto op = ast->get_kid(0)->get_operand();
	ins = new Instruction(HINS_MOV, *op, *readreg);
	_iseq->add_instruction(ins);
	free_vreg(); // no need to keep the temporary read register around
}

void HighLevelCodeGen::visit_var_ref(struct Node* ast) {
	const struct Node* tok_identifier = ast->get_kid(0);
	const auto sym = symtab->lookup(tok_identifier->get_str());
	const auto destreg = new Operand(OPERAND_VREG, sym->get_vreg());
	ast->set_operand(destreg);
}

void HighLevelCodeGen::visit_array_element_ref(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void HighLevelCodeGen::visit_field_ref(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void HighLevelCodeGen::visit_identifier_list(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void HighLevelCodeGen::visit_expression_list(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void HighLevelCodeGen::visit_identifier(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void HighLevelCodeGen::recur_on_children(struct Node* ast) {
	int num_kids = node_get_num_kids(ast);
	for (int i = 0; i < num_kids; i++) {
		visit(node_get_kid(ast, i));
	}
}
