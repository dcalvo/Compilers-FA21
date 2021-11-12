#include "lowlevelcodegen.h"
#include <iostream>
#include "highlevel.h"
#include "x86_64.h"

LowLevelCodeGen::LowLevelCodeGen(SymbolTable* symtab, int vregs): vregs_used(vregs), _iseq(new InstructionSequence),
                                                                  symtab(symtab) {}

LowLevelCodeGen::~LowLevelCodeGen() {}

InstructionSequence* LowLevelCodeGen::get_iseq() const {
	return _iseq;
}

Operand LowLevelCodeGen::vreg_ref(Operand op) {
	if (!op.has_base_reg()) return op; // op is actually a literal
	const int offset = vreg_refs.at(op.get_base_reg());
	return Operand(OPERAND_MREG_MEMREF_OFFSET, MREG_RSP, offset);
}

void LowLevelCodeGen::generate(InstructionSequence* hl_iseq) {
	// generate the boilerplate
	std::cout << "/* " << vregs_used << " vregs with storage allocated */" << '\n';
	std::cout << "\t.section .rodata" << '\n';
	std::cout << R"(s_readint_fmt: .string "%ld")" << '\n';
	std::cout << R"(s_writeint_fmt: .string "%ld\n")" << '\n';
	std::cout << "\t.section .bss" << '\n';
	std::cout << "\t.align 8" << '\n';
	std::cout << "s_readbuf: .space 8" << '\n';
	std::cout << "\t.section .text" << '\n';
	std::cout << "\t.globl main" << '\n';
	// calculate vreg memory locations
	int offset = symtab->get_offset(); // from start of vreg memory
	for (int i = 0; i <= vregs_used; i++) {
		vreg_refs[i] = offset;
		offset += 8;
	}
	//std::cout << symtab->get_offset() << '\n';
	//std::cout << vregs_used << '\n';
	//for (const auto thing : vreg_refs) {
	//	std::cout << thing.first << '\t' << thing.second << '\n';
	//}
	// offset %rsp by array & record memory
	auto ins = new Instruction(MINS_SUBQ, Operand(OPERAND_INT_LITERAL, offset), Operand(OPERAND_MREG, MREG_RSP));
	_iseq->define_label("main");
	_iseq->add_instruction(ins);
	// generate program
	for (unsigned int i = 0; i < hl_iseq->get_length(); i++) {
		if (hl_iseq->has_label(i)) _iseq->define_label(hl_iseq->get_label(i));
		const auto hlins = hl_iseq->get_instruction(i);
		int opcode = hlins->get_opcode();
		switch (opcode) {
		case HINS_NOP:
			generate_nop(hlins);
			break;
		case HINS_LOAD_ICONST:
			generate_load_int_literal(hlins);
			break;
		case HINS_INT_ADD:
			generate_add(hlins);
			break;
		case HINS_INT_SUB:
			generate_sub(hlins);
			break;
		case HINS_INT_MUL:
			generate_mul(hlins);
			break;
		case HINS_INT_DIV:
			generate_div(hlins);
			break;
		case HINS_INT_MOD:
			generate_mod(hlins);
			break;
		case HINS_INT_NEGATE:
			generate_negate(hlins);
			break;
		case HINS_LOCALADDR:
			generate_localaddr(hlins);
			break;
		case HINS_LOAD_INT:
			generate_load_int(hlins);
			break;
		case HINS_STORE_INT:
			generate_store_int(hlins);
			break;
		case HINS_READ_INT:
			generate_read_int(hlins);
			break;
		case HINS_WRITE_INT:
			generate_write_int(hlins);
			break;
		case HINS_JUMP:
			generate_jump(hlins);
			break;
		case HINS_JE:
			generate_je(hlins);
			break;
		case HINS_JNE:
			generate_jne(hlins);
			break;
		case HINS_JLT:
			generate_jlt(hlins);
			break;
		case HINS_JLTE:
			generate_jlte(hlins);
			break;
		case HINS_JGT:
			generate_jgt(hlins);
			break;
		case HINS_JGTE:
			generate_jgte(hlins);
			break;
		case HINS_INT_COMPARE:
			generate_compare(hlins);
			break;
		case HINS_MOV:
			generate_mov(hlins);
			break;
		default:
			assert(false); // unknown opcode
		}
	}
	if (hl_iseq->has_label_at_end()) _iseq->define_label(hl_iseq->get_label_at_end());
	ins = new Instruction(MINS_ADDQ, Operand(OPERAND_INT_LITERAL, offset), Operand(OPERAND_MREG, MREG_RSP));
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_INT_LITERAL, 0), Operand(OPERAND_MREG, MREG_RAX));
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_nop(Instruction* hlins) {
	const auto ins = new Instruction(MINS_NOP);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_load_int_literal(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto sourcereg = vreg_ref((*hlins)[1]);
	const auto ins = new Instruction(MINS_MOVQ, sourcereg, destreg);
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_add(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto leftreg = vreg_ref((*hlins)[1]);
	const auto rightreg = vreg_ref((*hlins)[2]);
	auto ins = new Instruction(MINS_MOVQ, leftreg, Operand(OPERAND_MREG, MREG_RAX));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_ADDQ, rightreg, Operand(OPERAND_MREG, MREG_RAX));
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_RAX), destreg);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_sub(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto leftreg = vreg_ref((*hlins)[1]);
	const auto rightreg = vreg_ref((*hlins)[2]);
	auto ins = new Instruction(MINS_MOVQ, leftreg, Operand(OPERAND_MREG, MREG_RAX));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_SUBQ, rightreg, Operand(OPERAND_MREG, MREG_RAX));
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_RAX), destreg);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_mul(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto leftreg = vreg_ref((*hlins)[1]);
	const auto rightreg = vreg_ref((*hlins)[2]);
	auto ins = new Instruction(MINS_MOVQ, leftreg, Operand(OPERAND_MREG, MREG_RAX));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_IMULQ, rightreg, Operand(OPERAND_MREG, MREG_RAX));
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_RAX), destreg);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_div(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto leftreg = vreg_ref((*hlins)[1]);
	const auto rightreg = vreg_ref((*hlins)[2]);
	auto ins = new Instruction(MINS_MOVQ, leftreg, Operand(OPERAND_MREG, MREG_RAX));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_CQTO);
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_IDIVQ, rightreg);
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_RAX), destreg);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_mod(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto leftreg = vreg_ref((*hlins)[1]);
	const auto rightreg = vreg_ref((*hlins)[2]);
	auto ins = new Instruction(MINS_MOVQ, leftreg, Operand(OPERAND_MREG, MREG_RAX));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_CQTO);
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_IDIVQ, rightreg);
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_RDX), destreg);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_negate(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto sourcereg = vreg_ref((*hlins)[1]);
	auto ins = new Instruction(MINS_MOVQ, sourcereg, Operand(OPERAND_MREG, MREG_RAX));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_IMULQ, Operand(OPERAND_INT_LITERAL, -1), Operand(OPERAND_MREG, MREG_RAX));
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_RAX), destreg);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_localaddr(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto sourcereg = vreg_ref((*hlins)[1]);
	const auto effective_address = Operand(OPERAND_MREG_MEMREF_OFFSET, MREG_RSP, sourcereg.get_int_value());
	auto ins = new Instruction(MINS_LEAQ, effective_address, Operand(OPERAND_MREG, MREG_R10));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_R10), destreg);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_load_int(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto sourcereg = vreg_ref((*hlins)[1]);
	auto ins = new Instruction(MINS_MOVQ, sourcereg, Operand(OPERAND_MREG, MREG_R10));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG_MEMREF, MREG_R10), Operand(OPERAND_MREG, MREG_R10));
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_R10), destreg);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_store_int(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto sourcereg = vreg_ref((*hlins)[1]);
	auto ins = new Instruction(MINS_MOVQ, destreg, Operand(OPERAND_MREG, MREG_R11));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, sourcereg, Operand(OPERAND_MREG, MREG_R10));
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_R10), Operand(OPERAND_MREG_MEMREF, MREG_R11));
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_read_int(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	auto ins = new Instruction(MINS_MOVQ, Operand("s_readint_fmt", true), Operand(OPERAND_MREG, MREG_RDI));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_LEAQ, destreg, Operand(OPERAND_MREG, MREG_RSI));
	_iseq->add_instruction(ins);
	//// zero out %rax
	//ins = new Instruction(MINS_MOVQ, Operand(OPERAND_INT_LITERAL, 0), Operand(OPERAND_MREG, MREG_RAX));
	//_iseq->add_instruction(ins);
	//// stack alignment
	//ins = new Instruction(MINS_SUBQ, Operand(OPERAND_INT_LITERAL, 8), Operand(OPERAND_MREG, MREG_RSP));
	//_iseq->add_instruction(ins);
	ins = new Instruction(MINS_CALL, Operand("scanf"));
	_iseq->add_instruction(ins);
	//// stack alignment
	//ins = new Instruction(MINS_ADDQ, Operand(OPERAND_INT_LITERAL, 8), Operand(OPERAND_MREG, MREG_RSP));
	//_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_write_int(Instruction* hlins) {
	const auto sourcereg = vreg_ref((*hlins)[0]);
	auto ins = new Instruction(MINS_MOVQ, Operand("s_writeint_fmt", true), Operand(OPERAND_MREG, MREG_RDI));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, sourcereg, Operand(OPERAND_MREG, MREG_RSI));
	_iseq->add_instruction(ins);
	// zero out %rax
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_INT_LITERAL, 0), Operand(OPERAND_MREG, MREG_RAX));
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_CALL, Operand("printf"));
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_jump(Instruction* hlins) {
	const auto label = (*hlins)[0];
	const auto ins = new Instruction(MINS_JMP, label);
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_je(Instruction* hlins) {
	const auto label = (*hlins)[0];
	const auto ins = new Instruction(MINS_JE, label);
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_jne(Instruction* hlins) {
	const auto label = (*hlins)[0];
	const auto ins = new Instruction(MINS_JNE, label);
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_jlt(Instruction* hlins) {
	const auto label = (*hlins)[0];
	const auto ins = new Instruction(MINS_JL, label);
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_jlte(Instruction* hlins) {
	const auto label = (*hlins)[0];
	const auto ins = new Instruction(MINS_JLE, label);
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_jgt(Instruction* hlins) {
	const auto label = (*hlins)[0];
	const auto ins = new Instruction(MINS_JG, label);
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_jgte(Instruction* hlins) {
	const auto label = (*hlins)[0];
	const auto ins = new Instruction(MINS_JGE, label);
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_compare(Instruction* hlins) {
	const auto leftreg = vreg_ref((*hlins)[0]);
	const auto rightreg = vreg_ref((*hlins)[1]);
	auto ins = new Instruction(MINS_MOVQ, leftreg, Operand(OPERAND_MREG, MREG_R10));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_CMPQ, rightreg, Operand(OPERAND_MREG, MREG_R10));
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_mov(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto sourcereg = vreg_ref((*hlins)[1]);
	auto ins = new Instruction(MINS_MOVQ, sourcereg, Operand(OPERAND_MREG, MREG_R10));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_R10), destreg);
	_iseq->add_instruction(ins);
}
