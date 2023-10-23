#pragma once

#include "pass.h"

namespace compiler
{

// TODO: fix algo when implement fast DomTree
class Rpo final : public Analysis
{
  public:
    Rpo(Graph *g);
    ~Rpo() override = default;

    bool runPassImpl() override;

    std::string getAnalysisName() const noexcept override
    {
        return "RPO";
    }

  private:
    void visitBasicBlock(BasicBlock *bb);

  private:
    // size_t cur_num;
    std::vector<BasicBlock *> rpo_bbs;
};

} // namespace compiler