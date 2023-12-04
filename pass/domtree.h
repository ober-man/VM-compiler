#pragma once

#include "pass/pass.h"

namespace compiler
{

class DomTree final : public Analysis
{
  public:
    explicit DomTree(Graph* g) : Analysis(g)
    {}
    ~DomTree() override = default;

    bool runPassImpl() override;

    std::string getAnalysisName() const noexcept override
    {
        return "DomTree";
    }

    void invalidateAnalysis() noexcept override
    {
        for (auto* bb : bbs)
            bb->getDominators().clear();
    }

  private:
    auto getUnreachedBBs(std::vector<BasicBlock*>& reached);

  private:
    std::vector<BasicBlock*> bbs;
};

} // namespace compiler