#pragma once

#include "pass.h"

namespace compiler
{

class DomTree final : public Analysis
{
  public:
    DomTree(Graph *g) : Analysis(g)
    {}
    ~DomTree() override = default;

    bool runPassImpl() override;

    std::string getAnalysisName() const noexcept override
    {
        return "DomTree";
    }

  private:
    auto getUnreachedBBs(std::vector<BasicBlock *> &reached);

  private:
    std::vector<BasicBlock *> bbs;
};

} // namespace compiler