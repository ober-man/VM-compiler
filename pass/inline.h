#pragma once

#include "ir/graph.h"
#include "pass.h"
#include "visitor.h"

namespace compiler
{

class Inline final : public Optimization
{
  public:
    explicit Inline(Graph* g) : Optimization(g)
    {}

    ~Inline() override = default;

    bool runPassImpl() override;

    std::string getOptName() const noexcept override
    {
        return "Inline";
    }

  private:
    void inlineMethod(Inst* inst);
    void processInputs(CallInst* call_inst, Graph* callee);
    void processReturn(Inst* call_inst, Graph* callee);
    void moveConstants(Graph* callee);
    void doInline(Graph* callee);
    void linkBlocks(BasicBlock* prev_bb, BasicBlock* next_bb, Graph* callee);
};

} // namespace compiler