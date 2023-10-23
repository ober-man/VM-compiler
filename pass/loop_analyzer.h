#pragma once

#include "pass.h"

namespace compiler
{

class LoopAnalyzer final : public Analysis
{
  public:
    LoopAnalyzer(Graph *g) : Analysis(g)
    {}
    ~LoopAnalyzer() override = default;

    bool runPassImpl() override;

    std::string getAnalysisName() const noexcept override
    {
        return "LoopAnalyzer";
    }
};

class Loop final
{
  public:
    Loop() = default;
    ~Loop() = default;

    bool isIrreducible() const noexcept
    {
        return is_irreducible;
    }

    BasicBlock *getHeader() const
    {
        return header;
    }

    Loop *getParentLoop() const noexcept
    {
        return parent;
    }

    std::vector<BasicBlock *> getBlocks() const noexcept
    {
        return blocks;
    }

    std::vector<Loop *> getChildren() const noexcept
    {
        return children;
    }

  private:
    BasicBlock *header = nullptr;
    Loop *parent = nullptr;
    std::vector<Loop *> children;
    std::vector<BasicBlock *> blocks;

    bool is_irreducible = false;
};

} // namespace compiler