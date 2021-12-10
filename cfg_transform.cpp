#include <cassert>
#include "cfg_transform.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>

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
		printer.print_basic_block(orig);
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
	if (iseq->get_length() == 0) return iseq;
	//////////////////
	// https://stackoverflow.com/questions/11408934/using-a-stdtuple-as-key-for-stdunordered-map
	using key_t = std::tuple<int, int, int>;
	struct key_hash : std::unary_function<key_t, std::size_t> {
		std::size_t operator()(const key_t& k) const {
			return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
		}
	};
	struct key_equal : public std::binary_function<key_t, key_t, bool> {
		bool operator()(const key_t& v0, const key_t& v1) const {
			return (
				std::get<0>(v0) == std::get<0>(v1) &&
				std::get<1>(v0) == std::get<1>(v1) &&
				std::get<2>(v0) == std::get<2>(v1)
			);
		}
	};

	using map_t = std::unordered_map<key_t, int, key_hash, key_equal>;
	/////////////////
	auto result = new InstructionSequence();
	int lvn = 1;
	std::unordered_map<int, int> const_to_vn;
	std::unordered_map<int, int> vn_to_const;
	std::unordered_map<int, int> vreg_to_vn;
	std::unordered_map<int, std::vector<int>> vn_to_vregs;
	map_t op_to_vn;
	for (unsigned i = 0; i < iseq->get_length(); i++) {
		auto ins = iseq->get_instruction(i)->duplicate();
		const int opcode = ins->get_opcode();
		switch (opcode) {
		case HINS_LOAD_ICONST:
			{
				const int ival = ins->get_operand(1).get_int_value();
				const int vreg = ins->get_operand(0).get_base_reg();
				// if the left op has been seen before
				if (vreg_to_vn.find(vreg) != vreg_to_vn.end()) {
					// erase it from the respective vn's vregs
					const int left_vn = vreg_to_vn[vreg];
					std::vector<int>& vregs = vn_to_vregs[left_vn];
					vregs.erase(std::remove(vregs.begin(), vregs.end(), vreg), vregs.end());
				}
				// if we've seen this const before
				if (const_to_vn.find(ival) != const_to_vn.end()) {
					// only add it to the vregs that hold it
					const int vn = const_to_vn[ival];
					vreg_to_vn[vreg] = vn;
					vn_to_vregs[vn].push_back(vreg);
				}
				else {
					const int vn = lvn++;
					const_to_vn[ival] = vn;
					vn_to_const[vn] = ival;
					vreg_to_vn[vreg] = vn;
					vn_to_vregs[vn].push_back(vreg);
				}
				result->add_instruction(ins);
				break;
			}
		case HINS_MOV:
			{
				const Operand left_op = ins->get_operand(0);
				const Operand right_op = ins->get_operand(1);
				const int right_vn = vreg_to_vn.find(right_op.get_base_reg()) != vreg_to_vn.end()
					                     ? vreg_to_vn[right_op.get_base_reg()]
					                     : lvn++;
				// if the left op has been seen before
				if (vreg_to_vn.find(left_op.get_base_reg()) != vreg_to_vn.end()) {
					// erase it from the respective vn's vregs
					const int left_vn = vreg_to_vn[left_op.get_base_reg()];
					std::vector<int>& vregs = vn_to_vregs[left_vn];
					vregs.erase(std::remove(vregs.begin(), vregs.end(), left_op.get_base_reg()), vregs.end());
				}
				// if the right op has been seen before
				if (vreg_to_vn.find(right_op.get_base_reg()) != vreg_to_vn.end()) {
					// replace it with the first instance of the op
					std::vector<int>& vregs = vn_to_vregs[right_vn];
					ins->set_operand(1, Operand(OPERAND_VREG, vregs.front()));
				}
				vreg_to_vn[left_op.get_base_reg()] = right_vn;
				vn_to_vregs[right_vn].push_back(left_op.get_base_reg());
				result->add_instruction(ins);
				break;
			}
		case HINS_LOCALADDR:
			{
				const Operand left_op = ins->get_operand(0);
				// if the left op has been seen before
				if (vreg_to_vn.find(left_op.get_base_reg()) != vreg_to_vn.end()) {
					// erase it from the respective vn's vregs
					const int left_vn = vreg_to_vn[left_op.get_base_reg()];
					std::vector<int>& vregs = vn_to_vregs[left_vn];
					vregs.erase(std::remove(vregs.begin(), vregs.end(), left_op.get_base_reg()), vregs.end());
				}
				const int left_vn = lvn++;
				vreg_to_vn[left_op.get_base_reg()] = left_vn;
				vn_to_vregs[left_vn].push_back(left_op.get_base_reg());
				result->add_instruction(ins);
				break;
			}
		case HINS_INT_ADD:
		case HINS_INT_MUL:
			{
				const Operand dest_op = ins->get_operand(0);
				const Operand left_op = ins->get_operand(1);
				const Operand right_op = ins->get_operand(2);
				int left_vn, right_vn;
				// we have to check if the operands are ints specifically due to localaddr computations
				if (left_op.get_kind() == OPERAND_INT_LITERAL)
					left_vn = const_to_vn.find(left_op.get_int_value()) != const_to_vn.end()
						          ? const_to_vn[left_op.get_int_value()]
						          : lvn++;
				else
					left_vn = vreg_to_vn.find(left_op.get_base_reg()) != vreg_to_vn.end()
						          ? vreg_to_vn[left_op.get_base_reg()]
						          : lvn++;
				if (right_op.get_kind() == OPERAND_INT_LITERAL)
					right_vn = const_to_vn.find(right_op.get_int_value()) != const_to_vn.end()
						           ? const_to_vn[right_op.get_int_value()]
						           : lvn++;
				else
					right_vn = vreg_to_vn.find(right_op.get_base_reg()) != vreg_to_vn.end()
						           ? vreg_to_vn[right_op.get_base_reg()]
						           : lvn++;
				// if the left op has been seen before
				if (!vn_to_vregs[left_vn].empty()) {
					// replace it with the first instance of the op
					std::vector<int>& vregs = vn_to_vregs[left_vn];
					ins->set_operand(1, Operand(OPERAND_VREG, vregs.front()));
				}
				// if the right op has been seen before
				if (!vn_to_vregs[right_vn].empty()) {
					// replace it with the first instance of the op
					std::vector<int>& vregs = vn_to_vregs[right_vn];
					ins->set_operand(2, Operand(OPERAND_VREG, vregs.front()));
				}
				key_t op_key = std::make_tuple(ins->get_opcode(), std::min(left_vn, right_vn),
				                               std::max(left_vn, right_vn));
				const int dest_vn = op_to_vn.find(op_key) != op_to_vn.end() ? op_to_vn[op_key] : lvn++;
				// if this instruction has not been seen before
				if (op_to_vn.find(op_key) == op_to_vn.end()) op_to_vn[op_key] = dest_vn;
				vreg_to_vn[dest_op.get_base_reg()] = dest_vn;
				vn_to_vregs[dest_vn].push_back(dest_op.get_base_reg());
				result->add_instruction(ins);
				break;
			}
		case HINS_INT_SUB:
		case HINS_INT_DIV:
		case HINS_INT_MOD:
			{
				const Operand dest_op = ins->get_operand(0);
				const Operand left_op = ins->get_operand(1);
				const Operand right_op = ins->get_operand(2);
				const int left_vn = vreg_to_vn.find(left_op.get_base_reg()) != vreg_to_vn.end()
					                    ? vreg_to_vn[left_op.get_base_reg()]
					                    : lvn++;
				const int right_vn = vreg_to_vn.find(right_op.get_base_reg()) != vreg_to_vn.end()
					                     ? vreg_to_vn[right_op.get_base_reg()]
					                     : lvn++;
				// if the left op has been seen before
				if (vreg_to_vn.find(left_op.get_base_reg()) != vreg_to_vn.end()) {
					// replace it with the first instance of the op
					std::vector<int>& vregs = vn_to_vregs[left_vn];
					ins->set_operand(1, Operand(OPERAND_VREG, vregs.front()));
				}
				// if the right op has been seen before
				if (vreg_to_vn.find(right_op.get_base_reg()) != vreg_to_vn.end()) {
					// replace it with the first instance of the op
					std::vector<int>& vregs = vn_to_vregs[right_vn];
					ins->set_operand(2, Operand(OPERAND_VREG, vregs.front()));
				}
				key_t op_key = std::make_tuple(ins->get_opcode(), left_vn, right_vn);
				const int dest_vn = op_to_vn.find(op_key) != op_to_vn.end() ? op_to_vn[op_key] : lvn++;
				// if this instruction has not been seen before
				if (op_to_vn.find(op_key) == op_to_vn.end()) op_to_vn[op_key] = dest_vn;
				vreg_to_vn[dest_op.get_base_reg()] = dest_vn;
				vn_to_vregs[dest_vn].push_back(dest_op.get_base_reg());
				result->add_instruction(ins);
				break;
			}
		default:
			result->add_instruction(ins);
			break;
		}
	}
	return result;
}

///////////////////////////////
// X86_64 CFG Transformation //
///////////////////////////////
InstructionSequence* X86_64ControlFlowGraphTransform::transform_basic_block(InstructionSequence* iseq) {
	return iseq;
}
