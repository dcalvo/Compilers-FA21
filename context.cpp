#include <cassert>
#include <cstdio>
#include <set>
#include "util.h"
#include "cpputil.h"
#include "node.h"
#include "symtab.h"
#include "astvisitor.h"
#include "context.h"
#include "highlevelcodegen.h"
#include "highlevel.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////

struct Context {
private:
	bool print_symbol_table = false;
	bool print_high_level = false;
	Node* root;
public:
	Context(struct Node* ast);
	~Context();

	void set_flag(char flag);

	void build_symtab();
	void generate_hcode();
};

////////////////////////////////////////////////////////////////////////
// Context class implementation
////////////////////////////////////////////////////////////////////////

Context::Context(struct Node* ast) {
	root = ast;
}

Context::~Context() {}

void Context::set_flag(char flag) {
	if (flag == 's') print_symbol_table = true;
	else if (flag == 'i') print_high_level = true;
	else
		assert(false); // we only support 's' right now
}

void Context::build_symtab() {
	const auto symtab = new SymbolTable(print_symbol_table);
	ASTVisitor visitor(symtab);
	visitor.visit(root);
}

void Context::generate_hcode() {
	HighLevelCodeGen code_gen;
	code_gen.visit(root);
	if (print_high_level) {
		const auto iseq = code_gen.get_iseq();
		const auto printer = new PrintHighLevelInstructionSequence(iseq);
		printer->print();
	}
}


////////////////////////////////////////////////////////////////////////
// Context API functions
////////////////////////////////////////////////////////////////////////

struct Context* context_create(struct Node* ast) {
	return new Context(ast);
}

void context_destroy(struct Context* ctx) {
	delete ctx;
}

void context_set_flag(struct Context* ctx, char flag) {
	ctx->set_flag(flag);
}

void context_build_symtab(struct Context* ctx) {
	ctx->build_symtab();
	ctx->generate_hcode();
}

void context_check_types(struct Context* ctx) {}
