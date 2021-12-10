#pragma once
#ifndef CFG_TRANSFORM_H
#define CFG_TRANSFORM_H
#include <unordered_map>
#include "cfg.h"
#include "live_vregs.h"
#include <tuple>

////////////////////////////
// local value number map //
////////////////////////////
using lvn_key = std::tuple<int, int, int, bool>;

struct key_hash : std::unary_function<lvn_key, std::size_t> {
	std::size_t operator()(const lvn_key& key) const {
		return std::get<0>(key) ^ std::get<1>(key) ^ std::get<2>(key) ^ std::get<3>(key);
	}
};

struct key_equal : std::binary_function<lvn_key, lvn_key, bool> {
	bool operator()(const lvn_key& key0, const lvn_key& key1) const {
		return std::get<0>(key0) == std::get<0>(key1) &&
			std::get<1>(key0) == std::get<1>(key1) &&
			std::get<2>(key0) == std::get<2>(key1);
	}
};

class LVNMap {
private:
	InstructionSequence* m_iseq;
	std::unordered_map<lvn_key, int, key_hash, key_equal> m_key_to_value;
	std::unordered_map<int, lvn_key> m_value_to_key;
	std::unordered_map<int, int> m_vreg_to_value;
	std::unordered_map<int, std::vector<int>> m_value_to_vregs;
	int m_current_lvn;

	int next_lvn() { return m_current_lvn++; }
	int get_value(Operand op);
	int get_value(lvn_key key);
	void change_value(Operand op, int lvn);
	Operand get_vreg(Operand op);
public:
	LVNMap(InstructionSequence* iseq);
	~LVNMap();
	void execute();
	Instruction* simplify(Instruction* ins);
};

class ControlFlowGraph;

class ControlFlowGraphTransform {
private:
	ControlFlowGraph* m_cfg;
	LiveVregs::LiveSet m_live_set;

protected:
	LVNMap* m_lvn;

public:
	ControlFlowGraphTransform(ControlFlowGraph* cfg);
	virtual ~ControlFlowGraphTransform();

	ControlFlowGraph* get_orig_cfg();
	ControlFlowGraph* transform_cfg();

	virtual InstructionSequence* transform_basic_block(InstructionSequence* iseq) = 0;
};

class HighLevelControlFlowGraphTransform : public ControlFlowGraphTransform {
private:
	ControlFlowGraph* m_cfg;

public:
	using ControlFlowGraphTransform::ControlFlowGraphTransform;
	InstructionSequence* transform_basic_block(InstructionSequence* iseq) override;
};

class X86_64ControlFlowGraphTransform : public ControlFlowGraphTransform {
private:
	ControlFlowGraph* m_cfg;

public:
	using ControlFlowGraphTransform::ControlFlowGraphTransform;
	InstructionSequence* transform_basic_block(InstructionSequence* iseq) override;
};
#endif // CFG_TRANSFORM_H
