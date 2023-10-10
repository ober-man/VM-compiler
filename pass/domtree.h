#pragma once

#include "passmanager.h"

namespace compiler
{

class DomTree final : public Pass
{
  public:
    DomTree() = default;
    ~DomTree() = default;

    void RunPassImpl(std::shared_ptr<Graph> graph) override;

  private:
    auto &getUnreachedBBs(std::vector<std::shared_ptr<BasicBlock>> &reached);

  private:
    std::vector<std::shared_ptr<BasicBlock>> bbs;
};

} // namespace compiler