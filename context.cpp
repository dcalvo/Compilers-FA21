#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <set>
#include "util.h"
#include "cpputil.h"
#include "node.h"
#include "type.h"
#include "symbol.h"
#include "symtab.h"
#include "ast.h"
#include "astvisitor.h"
#include "context.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////

struct Context {
private:
	bool print_symbol_table = false;
	Node* root;
public:
	Context(struct Node* ast);
	~Context();

	void set_flag(char flag);

	void build_symtab();

	// TODO: additional methods
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
	else
		assert(false); // we only support 's' right now
}

void Context::build_symtab() {
	const auto symtab = new SymbolTable(print_symbol_table);
	ASTVisitor visitor(symtab);
	visitor.visit(root);
}

// TODO: implementation of additional Context member functions

// TODO: implementation of member functions for helper classes

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
}

void context_check_types(struct Context* ctx) {}
