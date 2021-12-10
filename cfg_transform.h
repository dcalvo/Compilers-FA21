#pragma once
#ifndef CFG_TRANSFORM_H
#define CFG_TRANSFORM_H
#include "cfg.h"
#include "live_vregs.h"

class ControlFlowGraph;

class ControlFlowGraphTransform {
private:
	ControlFlowGraph* m_cfg;
	LiveVregs::LiveSet m_live_set;

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
