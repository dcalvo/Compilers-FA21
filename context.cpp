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

#include "cfg_transform.h"
#include "live_vregs.h"

////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////

struct Context {
private:
	bool print_symbol_table = false;
	bool print_high_level = false;
	bool optimize = false;
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
	else if (flag == 'o') optimize = true;
	else
		assert(false);
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
	auto high_level_iseq = code_gen.get_iseq();
	if (optimize) {
		HighLevelControlFlowGraphBuilder cfg_builder(high_level_iseq);
		ControlFlowGraph* cfg = cfg_builder.build();
		HighLevelControlFlowGraphTransform transform(cfg);
		ControlFlowGraph* transformed_cfg = transform.transform_cfg();
		high_level_iseq = transformed_cfg->create_instruction_sequence();
	}
	if (print_high_level) {
		const auto printer = new PrintHighLevelInstructionSequence(high_level_iseq);
		printer->print();
	}
	this->high_level_iseq = high_level_iseq;
	vregs_used = code_gen.get_vreg_count();
}

void Context::generate_lcode() {
	LowLevelCodeGen code_gen(symtab, vregs_used);
	code_gen.generate(high_level_iseq);
	auto low_level_iseq = code_gen.get_iseq();
	//if (optimize) {
	//	X86_64ControlFlowGraphBuilder cfg_builder(low_level_iseq);
	//	ControlFlowGraph* cfg = cfg_builder.build();
	//	X86_64ControlFlowGraphTransform transform(cfg);
	//	ControlFlowGraph* transformed_cfg = transform.transform_cfg();
	//	low_level_iseq = transformed_cfg->create_instruction_sequence();
	//}
	PrintX86_64InstructionSequence printer(low_level_iseq);
	printer.print();
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
}

void context_generate_hl_code(struct Context* ctx) {
	ctx->build_symtab();
	ctx->generate_hcode();
}

void context_compile(struct Context* ctx) {
	ctx->build_symtab();
	ctx->generate_hcode();
	ctx->generate_lcode();
}
