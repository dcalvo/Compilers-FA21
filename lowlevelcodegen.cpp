#include "lowlevelcodegen.h"
#include <iostream>
#include "highlevel.h"
#include "x86_64.h"

LowLevelCodeGen::LowLevelCodeGen(SymbolTable* symtab, int vregs): _iseq(new InstructionSequence), symtab(symtab) {
	// calculate vreg memory locations
	int offset = 0; // from start of vreg memory
	for (int i = 0; i <= vregs; i++) {
		vreg_refs[i] = offset;
		offset += 8;
	}
	//std::cout << symtab->get_offset() << '\n';
	//std::cout << vregs << '\n';
	//for (const auto thing : vreg_refs) {
	//	std::cout << thing.first << '\t' << thing.second << '\n';
	//}
	// offset %rsp by array & record memory
	const auto ins = new Instruction(MINS_SUBQ, Operand(OPERAND_INT_LITERAL, symtab->get_offset() + offset),
	                                 Operand(OPERAND_MREG, MREG_RSP));
	_iseq->add_instruction(ins);
}

LowLevelCodeGen::~LowLevelCodeGen() {}

InstructionSequence* LowLevelCodeGen::get_iseq() const {
	return _iseq;
}

Operand LowLevelCodeGen::vreg_ref(Operand op) {
	const int offset = vreg_refs.at(op.get_base_reg());
	return Operand(OPERAND_MREG_MEMREF_OFFSET, MREG_RSP, offset);
}

void LowLevelCodeGen::generate(InstructionSequence* hl_iseq) {
	for (unsigned int i = 0; i < hl_iseq->get_length(); i++) {
		if (hl_iseq->has_label(i)) _iseq->define_label(hl_iseq->get_label(i));
		const auto hlins = hl_iseq->get_instruction(i);
		int opcode = hlins->get_opcode();
		switch (opcode) {
		case HINS_NOP:
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
}

void LowLevelCodeGen::generate_load_int_literal(Instruction* hlins) {
	const auto ins = new Instruction(MINS_MOVQ, (*hlins)[1], vreg_ref((*hlins)[0]));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_add(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto leftreg = (*hlins)[1].has_base_reg()
		                     ? vreg_ref((*hlins)[1])
		                     : Operand(OPERAND_INT_LITERAL, (*hlins)[1].get_int_value());
	const auto rightreg = (*hlins)[2].has_base_reg()
		                      ? vreg_ref((*hlins)[2])
		                      : Operand(OPERAND_INT_LITERAL, (*hlins)[2].get_int_value());
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
	const auto leftreg = (*hlins)[1].has_base_reg()
		                     ? vreg_ref((*hlins)[1])
		                     : Operand(OPERAND_INT_LITERAL, (*hlins)[1].get_int_value());
	const auto rightreg = (*hlins)[2].has_base_reg()
		                      ? vreg_ref((*hlins)[2])
		                      : Operand(OPERAND_INT_LITERAL, (*hlins)[2].get_int_value());
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
	const auto leftreg = (*hlins)[1].has_base_reg()
		                     ? vreg_ref((*hlins)[1])
		                     : Operand(OPERAND_INT_LITERAL, (*hlins)[1].get_int_value());
	const auto rightreg = (*hlins)[2].has_base_reg()
		                      ? vreg_ref((*hlins)[2])
		                      : Operand(OPERAND_INT_LITERAL, (*hlins)[2].get_int_value());
	auto ins = new Instruction(MINS_MOVQ, leftreg, Operand(OPERAND_MREG, MREG_RAX));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_IMULQ, rightreg, Operand(OPERAND_MREG, MREG_RAX));
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_RAX), destreg);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_div(Instruction* hlins) {
	assert(false);
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto leftreg = (*hlins)[1].has_base_reg()
		                     ? vreg_ref((*hlins)[1])
		                     : Operand(OPERAND_INT_LITERAL, (*hlins)[1].get_int_value());
	const auto rightreg = (*hlins)[2].has_base_reg()
		                      ? vreg_ref((*hlins)[2])
		                      : Operand(OPERAND_INT_LITERAL, (*hlins)[2].get_int_value());
	auto ins = new Instruction(MINS_MOVQ, leftreg, Operand(OPERAND_MREG, MREG_RAX));
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_NOP, rightreg, Operand(OPERAND_MREG, MREG_RAX)); // TODO: implement DIV instruction
	_iseq->add_instruction(ins);
	ins = new Instruction(MINS_MOVQ, Operand(OPERAND_MREG, MREG_RAX), destreg);
	_iseq->add_instruction(ins);
}

void LowLevelCodeGen::generate_mod(Instruction* hlins) {
	assert(false);
}

void LowLevelCodeGen::generate_negate(Instruction* hlins) {
	assert(false);
}

void LowLevelCodeGen::generate_localaddr(Instruction* hlins) {}

void LowLevelCodeGen::generate_load_int(Instruction* hlins) {}

void LowLevelCodeGen::generate_store_int(Instruction* hlins) {}

void LowLevelCodeGen::generate_read_int(Instruction* hlins) {}

void LowLevelCodeGen::generate_write_int(Instruction* hlins) {}

void LowLevelCodeGen::generate_jump(Instruction* hlins) {}

void LowLevelCodeGen::generate_je(Instruction* hlins) {}

void LowLevelCodeGen::generate_jne(Instruction* hlins) {}

void LowLevelCodeGen::generate_jlt(Instruction* hlins) {}

void LowLevelCodeGen::generate_jlte(Instruction* hlins) {}

void LowLevelCodeGen::generate_jgt(Instruction* hlins) {}

void LowLevelCodeGen::generate_jgte(Instruction* hlins) {}

void LowLevelCodeGen::generate_compare(Instruction* hlins) {}

void LowLevelCodeGen::generate_mov(Instruction* hlins) {
	const auto destreg = vreg_ref((*hlins)[0]);
	const auto sourcereg = vreg_ref((*hlins)[1]);
	const auto ins = new Instruction(MINS_MOVQ, sourcereg, destreg);
	ins->set_comment(hlins->get_comment());
	_iseq->add_instruction(ins);
}
