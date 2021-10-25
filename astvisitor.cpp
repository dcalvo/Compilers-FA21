#include <cassert>
#include "node.h"
#include "grammar_symbols.h"
#include "ast.h"
#include "astvisitor.h"
#include "type.h"
#include <iostream>

#include "util.h"

ASTVisitor::ASTVisitor(SymbolTable* symtab) {
	this->symtab = symtab;
}

ASTVisitor::~ASTVisitor() {}

void ASTVisitor::visit(struct Node* ast) {
	int tag = node_get_tag(ast);
	switch (tag) {
	case AST_PROGRAM:
		visit_program(ast);
		break;
	case AST_DECLARATIONS:
		visit_declarations(ast);
		break;
	case AST_CONSTANT_DECLARATIONS:
		visit_constant_declarations(ast);
		break;
	case AST_CONSTANT_DEF:
		visit_constant_def(ast);
		break;
	case AST_TYPE_DECLARATIONS:
		visit_type_declarations(ast);
		break;
	case AST_TYPE_DEF:
		visit_type_def(ast);
		break;
	case AST_NAMED_TYPE:
		visit_named_type(ast);
		break;
	case AST_ARRAY_TYPE:
		visit_array_type(ast);
		break;
	case AST_RECORD_TYPE:
		visit_record_type(ast);
		break;
	case AST_VAR_DECLARATIONS:
		visit_var_declarations(ast);
		break;
	case AST_VAR_DEF:
		visit_var_def(ast);
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
		std::cout << "<error> unidentified AST node: " << ast_get_tag_name(ast->get_tag()) << '\n';
		assert(false); // unknown AST node type
	}
}

void ASTVisitor::visit_program(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_declarations(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_constant_declarations(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_constant_def(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_type_declarations(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_type_def(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_named_type(struct Node* ast) {
	const struct Node* tok_identifier = ast->get_kid(0);
	const auto sym = symtab->lookup(tok_identifier->get_str());
	if (!sym) {
		const struct SourceInfo err = tok_identifier->get_source_info();
		err_fatal("%s:%d:%d: Error: Use of a named type that is not defined\n", err.filename, err.line, err.col);
		return;
	}
	ast->set_type(sym->get_type());
}

void ASTVisitor::visit_array_type(struct Node* ast) {
	recur_on_children(ast); // default behavior
	const auto size_ast = ast->get_kid(0);
	const auto size_ast_type = size_ast->get_type();
	if (size_ast_type != symtab->lookup("INTEGER")->get_type()) {
		const struct SourceInfo err = size_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Array size not an integer\n", err.filename, err.line, err.col);
		return;
	}
	const auto type_ast = ast->get_kid(1);
	const auto array_type = new ArrayType(type_ast->get_type(), size_ast->get_ival());
	ast->set_type(array_type);
}

void ASTVisitor::visit_record_type(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_var_declarations(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

// traverses a list of identifiers and stores them in the provided vector
void collate_identifiers(struct Node* ast, std::vector<std::string>& ids) {
	ids.push_back(ast->get_kid(0)->get_str());
	if (ast->get_num_kids() == 2) collate_identifiers(ast->get_kid(1), ids);
}

void ASTVisitor::visit_var_def(struct Node* ast) {
	recur_on_children(ast); // default behavior
	const auto identifiers_ast = ast->get_kid(0);
	const auto type_ast = ast->get_kid(1);
	std::vector<std::string> ids;
	// get all of the identifiers
	collate_identifiers(identifiers_ast, ids);
	// add each to the symbol table
	for (const auto& id : ids) {
		const auto sym = new Symbol(id, type_ast->get_type(), VAR);
		symtab->define(id, sym);
	}
}

void ASTVisitor::visit_add(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_subtract(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_multiply(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_divide(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_modulus(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_negate(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_int_literal(struct Node* ast) {
	const struct Node* tok_int_literal = ast->get_kid(0);
	const auto type = symtab->lookup("INTEGER")->get_type();
	ast->set_source_info(tok_int_literal->get_source_info());
	ast->set_ival(std::stoi(tok_int_literal->get_str()));
	ast->set_type(type);
}

void ASTVisitor::visit_instructions(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_assign(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_if(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_if_else(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_repeat(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_while(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_compare_eq(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_compare_neq(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_compare_lt(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_compare_lte(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_compare_gt(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_compare_gte(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_write(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_read(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_var_ref(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_array_element_ref(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_field_ref(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_identifier_list(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_expression_list(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_identifier(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::recur_on_children(struct Node* ast) {
	int num_kids = node_get_num_kids(ast);
	for (int i = 0; i < num_kids; i++) {
		visit(node_get_kid(ast, i));
	}
}
