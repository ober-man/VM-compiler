#pragma once

#include "pass.h"

namespace compiler
{

#define NO_MARKER 1000

// TODO: fix algo when implement fast DomTree
class Rpo final : public Analysis
{
  public:
    Rpo(Graph* g, marker_t marker);
    ~Rpo() override = default;

    bool runPassImpl() override;

    std::string getAnalysisName() const noexcept override
    {
        return "RPO";
    }

    virtual void invalidateAnalysis() noexcept override
    {
        graph->getRpoBBs().clear();
        is_valid = false;
    }

  private:
    void visitBasicBlock(BasicBlock* bb);

  private:
    // size_t cur_num;
    std::vector<BasicBlock*> rpo_bbs;
    marker_t visited = NO_MARKER;
};

} // namespace compiler