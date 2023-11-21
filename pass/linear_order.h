#pragma once

#include "pass.h"

namespace compiler
{

class LinearOrder final : public Analysis
{
  public:
    explicit LinearOrder(Graph *g) : Analysis(g)
    {
        linear_bbs.reserve(GRAPH_BB_NUM);
    }
    ~LinearOrder() override = default;

    bool runPassImpl() override;

    std::string getAnalysisName() const noexcept override
    {
        return "LinearOrder";
    }

  private:
    void processBBs();
    void processLoop(Loop *loop);

  private:
    marker_t mrk;
    std::vector<BasicBlock *> linear_bbs;
};

} // namespace compiler