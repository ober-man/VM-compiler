#pragma once

#include "pass.h"

namespace compiler
{

#define LOOP_BLOCKS_NUM 10
#define LOOP_LATCHES_NUM 3
#define LOOP_INNERS_NUM 3

class Loop;

class LoopAnalysis final : public Analysis
{
  public:
    explicit LoopAnalysis(Graph* g) : Analysis(g)
    {}
    ~LoopAnalysis() override = default;

    bool runPassImpl() override;

    std::string getAnalysisName() const noexcept override
    {
        return "LoopAnalysis";
    }

  private:
    void findLoopsRec(BasicBlock* bb, BasicBlock* prev_bb);
    void populateLoops();
    void fillLoopRec(Loop* loop, BasicBlock* bb);
    void buildLoopTree();

  private:
    marker_t gray_mrk;
    marker_t black_mrk;
};

class Loop final
{
  public:
    explicit Loop(BasicBlock* header_, BasicBlock* latch = nullptr, bool is_irreducible_ = false)
        : header(header_), is_irreducible(is_irreducible_)
    {
        body.reserve(LOOP_BLOCKS_NUM);
        latches.reserve(LOOP_LATCHES_NUM);
        inners.reserve(LOOP_INNERS_NUM);
        if (latch)
            addLatch(latch);
    }

    ~Loop()
    {
        for (auto* inner : inners)
            delete inner;
    }

    bool isIrreducible() const noexcept
    {
        return is_irreducible;
    }

    void setIrreducible(bool is_irreducible_) noexcept
    {
        is_irreducible = is_irreducible_;
    }

    BasicBlock* getHeader() const noexcept
    {
        return header;
    }

    void setHeader(BasicBlock* bb) noexcept
    {
        header = bb;
    }

    Loop* getOuterLoop() const noexcept
    {
        return outer;
    }

    void setOuterLoop(Loop* outer_) noexcept
    {
        outer = outer_;
    }

    std::vector<BasicBlock*>& getBody() noexcept
    {
        return body;
    }

    std::vector<BasicBlock*>& getLatches() noexcept
    {
        return latches;
    }

    std::vector<Loop*>& getInnerLoops() noexcept
    {
        return inners;
    }

    void addBlock(BasicBlock* bb)
    {
        body.push_back(bb);
    }

    void addLatch(BasicBlock* latch)
    {
        latches.push_back(latch);
    }

    void addInner(Loop* inner)
    {
        inners.push_back(inner);
    }

  private:
    BasicBlock* header = nullptr;
    std::vector<BasicBlock*> body;
    std::vector<BasicBlock*> latches;

    Loop* outer = nullptr;
    std::vector<Loop*> inners;

    bool is_irreducible = false;
};

} // namespace compiler