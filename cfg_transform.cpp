#include <cassert>
#include "cfg_transform.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include "highlevel.h"
#include "x86_64.h"

ControlFlowGraphTransform::ControlFlowGraphTransform(ControlFlowGraph* cfg)
	: m_cfg(cfg) {}

ControlFlowGraphTransform::~ControlFlowGraphTransform() {}

ControlFlowGraph* ControlFlowGraphTransform::get_orig_cfg() {
	return m_cfg;
}

ControlFlowGraph* ControlFlowGraphTransform::transform_cfg() {
	auto result = new ControlFlowGraph();
	// LVN analysis
	m_lvn = new LVNMap(m_cfg->create_instruction_sequence());
	m_lvn->execute();
	auto v = new LiveVregs(m_cfg);
	v->execute();

	// map of basic blocks of original CFG to basic blocks in transformed CFG
	std::map<BasicBlock*, BasicBlock*> block_map;

	// iterate over all basic blocks, transforming each one
	for (auto i = m_cfg->bb_begin(); i != m_cfg->bb_end(); ++i) {
		BasicBlock* orig = *i;

		// transform the instructions
		m_live_set = v->get_fact_at_beginning_of_block(orig);
		LiveVregsControlFlowGraphPrinter printer(m_cfg, v);
		//printer.print_basic_block(orig);
		InstructionSequence* result_iseq = transform_basic_block(orig);

		// create result basic block
		BasicBlock* result_bb = result->create_basic_block(orig->get_kind(), orig->get_label());
		block_map[orig] = result_bb;

		// copy instructions into result basic block
		for (auto j = result_iseq->cbegin(); j != result_iseq->cend(); ++j) {
			result_bb->add_instruction((*j)->duplicate());
		}

		delete result_iseq;
	}

	// add edges to transformed CFG
	for (auto i = m_cfg->bb_begin(); i != m_cfg->bb_end(); ++i) {
		BasicBlock* orig = *i;
		const ControlFlowGraph::EdgeList& outgoing_edges = m_cfg->get_outgoing_edges(orig);
		for (auto j = outgoing_edges.cbegin(); j != outgoing_edges.cend(); ++j) {
			Edge* orig_edge = *j;

			BasicBlock* transformed_source = block_map[orig_edge->get_source()];
			BasicBlock* transformed_target = block_map[orig_edge->get_target()];

			result->create_edge(transformed_source, transformed_target, orig_edge->get_kind());
		}
	}

	return result;
}

///////////////////////////////////
// High Level CFG Transformation //
///////////////////////////////////
InstructionSequence* HighLevelControlFlowGraphTransform::transform_basic_block(InstructionSequence* iseq) {
	return iseq;
}

///////////////////////////////
// X86_64 CFG Transformation //
///////////////////////////////
InstructionSequence* X86_64ControlFlowGraphTransform::transform_basic_block(InstructionSequence* iseq) {
	return iseq;
}

//////////////////////////////
//// local value number map //
//////////////////////////////
int LVNMap::get_value(Operand op) {
	if (op.get_kind() != OPERAND_VREG) return -1;
	std::unordered_map<int, int>::const_iterator entry = m_vreg_to_value.find(op.get_base_reg());
	if (entry == m_vreg_to_value.end()) {
		int lvn = next_lvn();
		m_vreg_to_value[op.get_base_reg()] = lvn;
		m_value_to_vregs[lvn].push_back(op.get_base_reg());
		return lvn;
	}
	return entry->second;
}

int LVNMap::get_value(lvn_key key) {
	auto entry = m_key_to_value.find(key);
	if (entry == m_key_to_value.end()) {
		int lvn = next_lvn();
		m_key_to_value[key] = lvn;
		return lvn;
	}
	return entry->second;
}

void LVNMap::change_value(Operand op, int lvn) {
	std::unordered_map<int, int>::const_iterator entry = m_vreg_to_value.find(op.get_base_reg());
	if (entry != m_vreg_to_value.end()) m_vreg_to_value[op.get_base_reg()] = lvn;
	auto vregs = m_value_to_vregs[lvn];
	vregs.erase(std::remove(vregs.begin(), vregs.end(), op.get_base_reg()), vregs.end());
	m_value_to_vregs[lvn].push_back(op.get_base_reg());
}

Operand LVNMap::get_vreg(Operand op) {
	std::unordered_map<int, int>::const_iterator entry = m_vreg_to_value.find(op.get_base_reg());
	if (entry == m_vreg_to_value.end()) return op;
	int lvn = entry->second;
	return Operand(OPERAND_VREG, m_value_to_vregs[lvn].at(0));
}

LVNMap::LVNMap(InstructionSequence* iseq): m_iseq(iseq), m_current_lvn(0) {}

LVNMap::~LVNMap() {}

void LVNMap::execute() {
	for (unsigned i = 0; i < m_iseq->get_length(); i++) {
		Instruction* ins = m_iseq->get_instruction(i);
		int opcode = ins->get_opcode();
		switch (opcode) {
		case HINS_INT_ADD:
		case HINS_INT_MUL:
			{
				int lvn_dest = get_value(ins->get_operand(0));
				int lvn_left = get_value(ins->get_operand(1));
				//int lvn_right = get_value(ins->get_operand(2));
				//lvn_key key = std::make_tuple(opcode, std::min(lvn_left, lvn_right), std::max(lvn_left, lvn_right),
				//                              false);
				//int lvn = get_value(key);
				break;
			}
			//case HINS_INT_SUB:
			//case HINS_INT_DIV:
			//case HINS_INT_MOD:
			//	{
			//		int lvn_dest = get_value(ins->get_operand(0));
			//		int lvn_left = get_value(ins->get_operand(1));
			//		int lvn_right = get_value(ins->get_operand(2));
			//		lvn_key key = std::make_tuple(opcode, lvn_left, lvn_right, false);
			//		int lvn = get_value(key);
			//		break;
			//	}
			//case HINS_INT_NEGATE:
			//	{
			//		int lvn_dest = get_value(ins->get_operand(0));
			//		int lvn_op = get_value(ins->get_operand(1));
			//		lvn_key key = std::make_tuple(opcode, lvn_op, -1, false);
			//		int lvn = get_value(key);
			//		break;
			//	}
			//case HINS_LOAD_ICONST:
			//	{
			//		int lvn_dest = get_value(ins->get_operand(0));
			//		break;
			//	}
			//case HINS_MOV:
			//	{
			//		int lvn_right = get_value(ins->get_operand(1));
			//		change_value(ins->get_operand(0), lvn_right);
			//		break;
			//	}
		default:
			break;
		}
	}
}

Instruction* LVNMap::simplify(Instruction* ins) {
	int operands = ins->get_num_operands();
	if (operands == 1) {
		Operand vreg = get_vreg(ins->get_operand(0));
		return new Instruction(ins->get_opcode(), vreg);
	}
	if (operands == 2) {
		Operand left_vreg = get_vreg(ins->get_operand(0));
		Operand right_vreg = get_vreg(ins->get_operand(1));
		return new Instruction(ins->get_opcode(), left_vreg, right_vreg);
	}
	if (operands == 3) {
		Operand dest_vreg = get_vreg(ins->get_operand(0));
		Operand left_vreg = get_vreg(ins->get_operand(1));
		Operand right_vreg = get_vreg(ins->get_operand(2));
		return new Instruction(ins->get_opcode(), dest_vreg, left_vreg, right_vreg);
	}
	return ins;
}
