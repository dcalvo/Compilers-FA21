#include <cassert>
#include "node.h"
#include "grammar_symbols.h"
#include "ast.h"
#include "astvisitor.h"
#include "type.h"
#include <iostream>
#include "util.h"

// returns true if given operand_ast is integral (INTEGER or CHAR), false otherwise
bool check_integral(const SymbolTable* symtab, struct Node* operand_ast) {
	const auto operand_type = operand_ast->get_type();
	const auto int_type = symtab->lookup("INTEGER")->get_type();
	const auto char_type = symtab->lookup("CHAR")->get_type();
	return operand_type != int_type && operand_type != char_type ? false : true;
}

// returns Type* to either INTEGER or CHAR depending on operand types, errors otherwise
Type* check_operand_types(const SymbolTable* symtab, struct Node* left_ast, struct Node* right_ast) {
	if (!check_integral(symtab, left_ast)) {
		const struct SourceInfo err = left_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Using a non-integral value '%s' as an operand to a binary operator\n", err.filename,
		          err.line, err.col, left_ast->get_str().c_str());
		return nullptr;
	}
	if (!check_integral(symtab, right_ast)) {
		const struct SourceInfo err = right_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Using a non-integral value '%s' as an operand to a binary operator\n", err.filename,
		          err.line, err.col, right_ast->get_str().c_str());
		return nullptr;
	}
	const auto int_type = symtab->lookup("INTEGER")->get_type();
	return left_ast->get_type() == right_ast->get_type() ? left_ast->get_type() : int_type;
}

// returns true if the given node is an integer literal or defined as a constant expression, false otherwise
bool check_const(const SymbolTable* symtab, struct Node* operand_ast) {
	if (operand_ast->get_tag() == AST_INT_LITERAL) return true;
	return operand_ast->get_tag() == AST_VAR_REF && symtab->lookup(operand_ast->get_str())->get_kind() == CONST
		       ? true
		       : false;
}

// traverses a list of identifiers and stores them in the provided vector
void collate_identifiers(struct Node* ast, std::vector<std::string>& ids) {
	ids.push_back(ast->get_kid(0)->get_str());
	if (ast->get_num_kids() == 2) collate_identifiers(ast->get_kid(1), ids);
}

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
	recur_on_children(ast);
	const auto identifier_ast = ast->get_kid(0);
	const auto value_ast = ast->get_kid(1);
	// if the value_ast has a string, at least one of its children is a non-constant
	if (value_ast->get_str().length()) {
		const struct SourceInfo err = value_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Using a non-constant value '%s' in a constant expression\n", err.filename, err.line,
		          err.col, value_ast->get_str().c_str());
		return;
	}
	const auto sym = new Symbol(identifier_ast->get_str(), value_ast->get_type(), CONST);
	sym->set_ival(value_ast->get_ival());
	if (!symtab->define(sym)) {
		const struct SourceInfo err = identifier_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Name '%s' is already defined\n", err.filename, err.line, err.col,
		          sym->get_name().c_str());
	}
}

void ASTVisitor::visit_type_declarations(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_type_def(struct Node* ast) {
	recur_on_children(ast);
	const auto identifier_ast = ast->get_kid(0);
	const auto type_ast = ast->get_kid(1);
	const auto sym = new Symbol(identifier_ast->get_str(), type_ast->get_type(), TYPE);
	if (!symtab->define(sym)) {
		const struct SourceInfo err = identifier_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Name '%s' is already defined\n", err.filename, err.line, err.col,
		          sym->get_name().c_str());
	}
}

void ASTVisitor::visit_named_type(struct Node* ast) {
	const struct Node* tok_identifier = ast->get_kid(0);
	const auto sym = symtab->lookup(tok_identifier->get_str());
	if (!sym) {
		const struct SourceInfo err = tok_identifier->get_source_info();
		err_fatal("%s:%d:%d: Error: Use of a named type '%s' that is not defined\n", err.filename, err.line, err.col,
		          tok_identifier->get_str().c_str());
		return;
	}
	ast->set_source_info(tok_identifier->get_source_info());
	ast->set_str(tok_identifier->get_str());
	ast->set_type(sym->get_type());
}

void ASTVisitor::visit_array_type(struct Node* ast) {
	recur_on_children(ast);
	const auto size_ast = ast->get_kid(0);
	if (size_ast->get_type() != symtab->lookup("INTEGER")->get_type()) {
		const struct SourceInfo err = size_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Array size not an integer\n", err.filename, err.line, err.col);
		return;
	}
	const auto type_ast = ast->get_kid(1);
	const auto array_type = new ArrayType(type_ast->get_type(), size_ast->get_ival());
	ast->set_type(array_type);
}

void ASTVisitor::visit_record_type(struct Node* ast) {
	const auto outer_symtab = symtab;
	const auto record_symtab = new SymbolTable(symtab);
	symtab = record_symtab;
	// increase depth by 1 and continue
	recur_on_children(ast);
	// lift out and fill record type
	symtab = outer_symtab;
	std::vector<RecordField*> fields;
	for (const auto& sym : record_symtab->get_syms()) {
		const auto field = new RecordField(sym->get_type(), sym->get_name());
		fields.push_back(field);
	}
	const auto record_type = new RecordType(fields, record_symtab);
	ast->set_type(record_type);
}

void ASTVisitor::visit_var_declarations(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_var_def(struct Node* ast) {
	recur_on_children(ast);
	const auto identifiers_ast = ast->get_kid(0);
	const auto type_ast = ast->get_kid(1);
	if (type_ast->get_tag() == AST_NAMED_TYPE) {
		const auto sym = symtab->lookup(type_ast->get_str());
		if (sym->get_kind() != TYPE) {
			const struct SourceInfo err = type_ast->get_source_info();
			err_fatal("%s:%d:%d: Error: Identifier '%s' does not refer to a type\n", err.filename, err.line, err.col,
			          type_ast->get_str().c_str());
			return;
		}
	}
	std::vector<std::string> ids;
	// get all of the identifiers
	collate_identifiers(identifiers_ast, ids);
	// add each to the symbol table
	for (const auto& id : ids) {
		const auto sym = new Symbol(id, type_ast->get_type(), VAR);
		if (!symtab->define(sym)) {
			const struct SourceInfo err = identifiers_ast->get_source_info();
			err_fatal("%s:%d:%d: Error: Name '%s' is already defined\n", err.filename, err.line, err.col, id.c_str());
			return;
		}
	}
}

void ASTVisitor::visit_add(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	const auto result_type = check_operand_types(symtab, left_ast, right_ast);
	const int result = left_ast->get_ival() + right_ast->get_ival();
	ast->set_ival(result);
	ast->set_type(result_type);
	// if either operand is a non-constant, push that info up the tree
	if (!check_const(symtab, left_ast)) {
		ast->set_str(left_ast->get_str());
		ast->set_source_info(left_ast->get_source_info());
	}
	else if (!check_const(symtab, right_ast)) {
		ast->set_str(right_ast->get_str());
		ast->set_source_info(right_ast->get_source_info());

	}
}

void ASTVisitor::visit_subtract(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	const auto result_type = check_operand_types(symtab, left_ast, right_ast);
	const int result = left_ast->get_ival() - right_ast->get_ival();
	ast->set_ival(result);
	ast->set_type(result_type);
	// if either operand is a non-constant, push that info up the tree
	if (!check_const(symtab, left_ast)) {
		ast->set_str(left_ast->get_str());
		ast->set_source_info(left_ast->get_source_info());
	}
	else if (!check_const(symtab, right_ast)) {
		ast->set_str(right_ast->get_str());
		ast->set_source_info(right_ast->get_source_info());

	}
}

void ASTVisitor::visit_multiply(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	const auto result_type = check_operand_types(symtab, left_ast, right_ast);
	const int result = left_ast->get_ival() * right_ast->get_ival();
	ast->set_ival(result);
	ast->set_type(result_type);
	// if either operand is a non-constant, push that info up the tree
	if (!check_const(symtab, left_ast)) {
		ast->set_str(left_ast->get_str());
		ast->set_source_info(left_ast->get_source_info());
	}
	else if (!check_const(symtab, right_ast)) {
		ast->set_str(right_ast->get_str());
		ast->set_source_info(right_ast->get_source_info());

	}
}

void ASTVisitor::visit_divide(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	const auto result_type = check_operand_types(symtab, left_ast, right_ast);
	if (right_ast->get_ival() == 0 && check_const(symtab, right_ast)) {
		const struct SourceInfo err = right_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Illegal division by zero\n", err.filename, err.line, err.col);
		return;
	}
	if (check_const(symtab, left_ast) && check_const(symtab, right_ast)) {
		const int result = left_ast->get_ival() / right_ast->get_ival();
		ast->set_ival(result);
	}
	ast->set_type(result_type);
	// if either operand is a non-constant, push that info up the tree
	if (!check_const(symtab, left_ast)) {
		ast->set_str(left_ast->get_str());
		ast->set_source_info(left_ast->get_source_info());
	}
	else if (!check_const(symtab, right_ast)) {
		ast->set_str(right_ast->get_str());
		ast->set_source_info(right_ast->get_source_info());

	}
}

void ASTVisitor::visit_modulus(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	const auto result_type = check_operand_types(symtab, left_ast, right_ast);
	if (right_ast->get_ival() == 0) {
		const struct SourceInfo err = right_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Illegal mod by zero\n", err.filename, err.line, err.col);
		return;
	}
	const int result = left_ast->get_ival() % right_ast->get_ival();
	ast->set_ival(result);
	ast->set_type(result_type);
	// if either operand is a non-constant, push that info up the tree
	if (!check_const(symtab, left_ast)) {
		ast->set_str(left_ast->get_str());
		ast->set_source_info(left_ast->get_source_info());
	}
	else if (!check_const(symtab, right_ast)) {
		ast->set_str(right_ast->get_str());
		ast->set_source_info(right_ast->get_source_info());

	}
}

void ASTVisitor::visit_negate(struct Node* ast) {
	recur_on_children(ast);
	const auto operand_ast = ast->get_kid(0);
	if (!check_integral(symtab, operand_ast)) {
		const struct SourceInfo err = operand_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Using a non-integral value '%s' as an operand to a unary operator\n", err.filename,
		          err.line, err.col, operand_ast->get_str().c_str());
		return;
	}
	const int result = -operand_ast->get_ival();
	ast->set_ival(result);
	ast->set_type(operand_ast->get_type());
	// if the operand is a non-constant, push that info up the tree
	if (!check_const(symtab, operand_ast)) {
		ast->set_str(operand_ast->get_str());
		ast->set_source_info(operand_ast->get_source_info());
	}
}

void ASTVisitor::visit_int_literal(struct Node* ast) {
	const auto tok_int_literal = ast->get_kid(0);
	assert(symtab->lookup("INTEGER")); // we should always have INTEGER type
	const auto type = symtab->lookup("INTEGER")->get_type();
	ast->set_source_info(tok_int_literal->get_source_info());
	ast->set_ival(std::stoi(tok_int_literal->get_str()));
	ast->set_type(type);
}

void ASTVisitor::visit_instructions(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_assign(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	if (!check_integral(symtab, right_ast)) {
		const struct SourceInfo err = right_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Using a non-integral value '%s' as an assigned value\n", err.filename, err.line,
		          err.col, right_ast->get_str().c_str());
	}
	if (left_ast->get_type() != right_ast->get_type()) {
		const struct SourceInfo err = left_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: LHS of assignment is type '%s' while RHS of assignment is type '%s'\n",
		          err.filename, err.line, err.col, left_ast->get_type()->to_string().c_str(),
		          right_ast->get_type()->to_string().c_str());
	}
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
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	check_operand_types(symtab, left_ast, right_ast);
}

void ASTVisitor::visit_compare_neq(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	check_operand_types(symtab, left_ast, right_ast);
}

void ASTVisitor::visit_compare_lt(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	check_operand_types(symtab, left_ast, right_ast);
}

void ASTVisitor::visit_compare_lte(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	check_operand_types(symtab, left_ast, right_ast);
}

void ASTVisitor::visit_compare_gt(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	check_operand_types(symtab, left_ast, right_ast);
}

void ASTVisitor::visit_compare_gte(struct Node* ast) {
	recur_on_children(ast);
	const auto left_ast = ast->get_kid(0);
	const auto right_ast = ast->get_kid(1);
	check_operand_types(symtab, left_ast, right_ast);
}

void ASTVisitor::visit_write(struct Node* ast) {
	recur_on_children(ast);
	const auto var_ast = ast->get_kid(0);
	if (!check_integral(symtab, var_ast)) {
		// not an integer or char, maybe an array of char
		const auto char_type = symtab->lookup("CHAR")->get_type();
		const auto array_ = dynamic_cast<ArrayType*>(var_ast->get_type());
		if (!array_ || array_->get_type() != char_type) {
			// neither an integer, char, or array of char
			const struct SourceInfo err = var_ast->get_source_info();
			err_fatal("%s:%d:%d: Error: Inappropriate type '%s' for WRITE statement\n", err.filename, err.line, err.col,
			          var_ast->get_type()->to_string().c_str());
		}
		// it's an array of char
	}
}

void ASTVisitor::visit_read(struct Node* ast) {
	recur_on_children(ast);
	const auto var_ast = ast->get_kid(0);
	if (!check_integral(symtab, var_ast)) {
		// not an integer or char, maybe an array of char
		const auto char_type = symtab->lookup("CHAR")->get_type();
		const auto array_ = dynamic_cast<ArrayType*>(var_ast->get_type());
		if (!array_ || array_->get_type() != char_type) {
			// neither an integer, char, or array of char
			const struct SourceInfo err = var_ast->get_source_info();
			err_fatal("%s:%d:%d: Error: Inappropriate type '%s' for READ statement\n", err.filename, err.line, err.col,
			          var_ast->get_type()->to_string().c_str());
		}
		// it's an array of char
	}
}

void ASTVisitor::visit_var_ref(struct Node* ast) {
	const struct Node* tok_identifier = ast->get_kid(0);
	const auto sym = symtab->lookup(tok_identifier->get_str());
	if (!sym) {
		const struct SourceInfo err = tok_identifier->get_source_info();
		err_fatal("%s:%d:%d: Error: Use of an undefined variable '%s'\n", err.filename, err.line, err.col,
		          tok_identifier->get_str().c_str());
		return;
	}
	ast->set_ival(sym->get_ival());
	ast->set_source_info(tok_identifier->get_source_info());
	ast->set_str(tok_identifier->get_str());
	ast->set_type(sym->get_type());
}

void ASTVisitor::visit_array_element_ref(struct Node* ast) {
	recur_on_children(ast);
	const auto identifier_ast = ast->get_kid(0);
	const auto index_ast = ast->get_kid(1);
	// try to coerce into array
	const auto array_ = dynamic_cast<ArrayType*>(identifier_ast->get_type());
	if (!array_) {
		const struct SourceInfo err = index_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Attempt to use the array subscript operator on non-array '%s'\n", err.filename,
		          err.line, err.col, identifier_ast->get_str().c_str());
	}
	if (!check_integral(symtab, index_ast)) {
		const struct SourceInfo err = index_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Using non-integral value '%s' as an array index\n", err.filename,
		          err.line, err.col, index_ast->get_str().c_str());
	}
	ast->set_str(identifier_ast->get_str());
	ast->set_source_info(identifier_ast->get_source_info());
	ast->set_type(array_->get_type());
}

void ASTVisitor::visit_field_ref(struct Node* ast) {
	recur_on_children(ast);
	const auto record_ast = ast->get_kid(0);
	const auto field_ast = ast->get_kid(1);
	// try to coerce into record
	const auto record = dynamic_cast<RecordType*>(record_ast->get_type());
	if (!record) {
		const struct SourceInfo err = field_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Attempt to access field on non-record '%s'\n", err.filename,
		          err.line, err.col, record_ast->get_str().c_str());
	}
	const auto field_type = record->get_field(field_ast->get_str());
	if (!field_type) {
		const struct SourceInfo err = field_ast->get_source_info();
		err_fatal("%s:%d:%d: Error: Attempt to access nonexistent field '%s' on record '%s'\n", err.filename,
		          err.line, err.col, field_ast->get_str().c_str(), record_ast->get_str().c_str());
	}
	ast->set_str(field_ast->get_str());
	ast->set_source_info(field_ast->get_source_info());
	ast->set_type(field_type);
}

void ASTVisitor::visit_identifier_list(struct Node* ast) {
	recur_on_children(ast); // default behavior
}

void ASTVisitor::visit_expression_list(struct Node* ast) {
	recur_on_children(ast);
	// just pass information up the tree
	const auto expression_ast = ast->get_kid(0);
	ast->set_ival(expression_ast->get_ival());
	ast->set_type(expression_ast->get_type());
	ast->set_source_info(expression_ast->get_source_info());
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
