#pragma once

#include "passmanager.h"

namespace compiler
{

class RPO final : public Analysis
{
  public:
    RPO()
    {
        rpo_bbs.reserve(GRAPH_BB_NUM);
    }
    ~RPO() = default;

    void RunPassImpl(std::shared_ptr<Graph> graph) override;

  private:
    void VisitBasicBlock(std::shared_ptr<BasicBlock> bb);

  private:
    std::vector<std::shared_ptr<BasicBlock>> rpo_bbs;
};

} // namespace compiler