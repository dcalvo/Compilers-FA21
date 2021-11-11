#ifndef LOWLEVELCODEGEN_H
#define LOWLEVELCODEGEN_H
#include "cfg.h"
#include "symtab.h"
#include <map>

class LowLevelCodeGen {
	InstructionSequence* _iseq;
	SymbolTable* symtab;
	std::map<int, int> vreg_refs;

public:
	LowLevelCodeGen(SymbolTable* symtab, int vregs);
	virtual ~LowLevelCodeGen();

	InstructionSequence* get_iseq() const;

	Operand vreg_ref(Operand op);
	void generate(InstructionSequence* hl_iseq);
	void generate_load_int_literal(Instruction* hlins);
	void generate_add(Instruction* hlins);
	void generate_sub(Instruction* hlins);
	void generate_mul(Instruction* hlins);
	void generate_div(Instruction* hlins);
	void generate_mod(Instruction* hlins);
	void generate_negate(Instruction* hlins);
	void generate_localaddr(Instruction* hlins);
	void generate_load_int(Instruction* hlins);
	void generate_store_int(Instruction* hlins);
	void generate_read_int(Instruction* hlins);
	void generate_write_int(Instruction* hlins);
	void generate_jump(Instruction* hlins);
	void generate_je(Instruction* hlins);
	void generate_jne(Instruction* hlins);
	void generate_jlt(Instruction* hlins);
	void generate_jlte(Instruction* hlins);
	void generate_jgt(Instruction* hlins);
	void generate_jgte(Instruction* hlins);
	void generate_compare(Instruction* hlins);
	void generate_mov(Instruction* hlins);
};

#endif // LOWLEVELCODEGEN_H
