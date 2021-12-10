#include <cassert>
#include "highlevel.h"

#include <iostream>

#include "x86_64.h"

PrintHighLevelInstructionSequence::PrintHighLevelInstructionSequence(InstructionSequence* ins)
	: PrintInstructionSequence(ins) {}

std::string PrintHighLevelInstructionSequence::get_opcode_name(int opcode) {
	switch (opcode) {
	case HINS_NOP: return "nop";
	case HINS_LOAD_ICONST: return "ldci";
	case HINS_INT_ADD: return "addi";
	case HINS_INT_SUB: return "subi";
	case HINS_INT_MUL: return "muli";
	case HINS_INT_DIV: return "divi";
	case HINS_INT_MOD: return "modi";
	case HINS_INT_NEGATE: return "negi";
	case HINS_LOCALADDR: return "localaddr";
	case HINS_LOAD_INT: return "ldi";
	case HINS_STORE_INT: return "sti";
	case HINS_READ_INT: return "readi";
	case HINS_WRITE_INT: return "writei";
	case HINS_JUMP: return "jmp";
	case HINS_JE: return "je";
	case HINS_JNE: return "jne";
	case HINS_JLT: return "jlt";
	case HINS_JLTE: return "jlte";
	case HINS_JGT: return "jgt";
	case HINS_JGTE: return "jgte";
	case HINS_INT_COMPARE: return "cmpi";
	case HINS_LEA: return "lea";
	case HINS_MOV: return "mov";

	default:
		assert(false);
		return "<invalid>";
	}
}

std::string PrintHighLevelInstructionSequence::get_mreg_name(int regnum) {
	// high level instructions should not use machine registers
	const char* s;
	switch (regnum) {
	case MREG_RAX: s = "%rax";
		break;
	case MREG_RBX: s = "%rbx";
		break;
	case MREG_RCX: s = "%rcx";
		break;
	case MREG_RDX: s = "%rdx";
		break;
	case MREG_RDI: s = "%rdi";
		break;
	case MREG_RSI: s = "%rsi";
		break;
	case MREG_RSP: s = "%rsp";
		break;
	case MREG_RBP: s = "%rbp";
		break;
	case MREG_R8: s = "%r8";
		break;
	case MREG_R9: s = "%r9";
		break;
	case MREG_R10: s = "%r10";
		break;
	case MREG_R11: s = "%r11";
		break;
	case MREG_R12: s = "%r12";
		break;
	case MREG_R13: s = "%r13";
		break;
	case MREG_R14: s = "%r14";
		break;
	case MREG_R15: s = "%r15";
		break;
	default:
		assert(false);
		s = "<invalid>";
	}
	return std::string(s);
}

HighLevelControlFlowGraphBuilder::HighLevelControlFlowGraphBuilder(InstructionSequence* iseq)
	: ControlFlowGraphBuilder(iseq) {}

HighLevelControlFlowGraphBuilder::~HighLevelControlFlowGraphBuilder() {}

bool HighLevelControlFlowGraphBuilder::falls_through(Instruction* ins) {
	// only unconditional jump instructions don't fall through
	return ins->get_opcode() != HINS_JUMP;
}

HighLevelControlFlowGraphPrinter::HighLevelControlFlowGraphPrinter(ControlFlowGraph* cfg)
	: ControlFlowGraphPrinter(cfg) {}

HighLevelControlFlowGraphPrinter::~HighLevelControlFlowGraphPrinter() {}

void HighLevelControlFlowGraphPrinter::print_basic_block(BasicBlock* bb) {
	PrintHighLevelInstructionSequence print_hliseq(bb);
	print_hliseq.print();
}

std::string HighLevelControlFlowGraphPrinter::format_instruction(const BasicBlock* bb, const Instruction* ins) {
	PrintHighLevelInstructionSequence p(nullptr);
	return p.format_instruction(ins);
}

bool HighLevel::is_def(const Instruction* ins) {
	int opcode = ins->get_opcode();
	switch (opcode) {
	case HINS_INT_ADD:
	case HINS_INT_SUB:
	case HINS_INT_MUL:
	case HINS_INT_DIV:
	case HINS_INT_MOD:
	case HINS_INT_NEGATE:
	case HINS_LOAD_ICONST:
	case HINS_MOV:
	case HINS_LOAD_INT:
	case HINS_READ_INT:
	case HINS_LOCALADDR:
		return ins->get_operand(0).get_kind() == OPERAND_VREG;
	default:
		return false;
	}
}

bool HighLevel::is_use(const Instruction* ins, unsigned operand) {
	return ins->get_operand(operand).get_kind() == OPERAND_VREG;
}
