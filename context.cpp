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
#include "lowlevelcodegen.h"
#include "x86_64.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////

struct Context {
private:
	bool print_symbol_table = false;
	bool print_high_level = false;
	Node* root;
	SymbolTable* symtab;
	InstructionSequence* high_level_iseq;
	int vregs_used = 0;
public:
	Context(struct Node* ast);
	~Context();

	void set_flag(char flag);

	void build_symtab();
	void generate_hcode();
	void generate_lcode();
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
	this->symtab = symtab;
}

void Context::generate_hcode() {
	HighLevelCodeGen code_gen(symtab);
	code_gen.visit(root);
	if (print_high_level) {
		const auto iseq = code_gen.get_iseq();
		const auto printer = new PrintHighLevelInstructionSequence(iseq);
		printer->print();
	}
	high_level_iseq = code_gen.get_iseq();
	vregs_used = code_gen.get_vreg_count();
}

void Context::generate_lcode() {
	LowLevelCodeGen code_gen(symtab, vregs_used);
	code_gen.generate(high_level_iseq);
	const auto low_level_iseq = code_gen.get_iseq();
	PrintX86_64InstructionSequence printer(low_level_iseq);
	printer.print();
	// end of program
	std::cout << "\tret" << '\n';
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

void context_compile(struct Context* ctx) {
	ctx->generate_lcode();
}
