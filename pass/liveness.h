#pragma once

#include "ir/graph.h"
#include "pass.h"
#include <set>
#include <unordered_map>

namespace compiler
{

#define GRAPH_BB_NUM 50

class LiveInterval;
class LiveSet;

class LivenessAnalysis final : public Analysis
{
  public:
    explicit LivenessAnalysis(Graph* g) : Analysis(g)
    {
        linear_bbs.reserve(GRAPH_BB_NUM);
    }

    ~LivenessAnalysis() override;

    bool runPassImpl() override;

    std::string getAnalysisName() const noexcept override
    {
        return "LivenessAnalysis";
    }

  private:
    void setInstsInitialNumbers();
    void buildLiveIntervals();
    void insertInstLiveInterval(Inst* inst, size_t start, size_t end);
    LiveSet* calcInitLiveSets(BasicBlock* bb);
    void processSucc(BasicBlock* bb, BasicBlock* succ, LiveSet* live_set);
    void appendBBIntervals(LiveSet* live_set, LiveInterval* live_int);
    void processBBInsts(BasicBlock* bb, LiveSet* live_set);
    void processInstInputs(Inst* inst, LiveSet* live_set, size_t start);
    void processInput(Inst* input, LiveSet* live_set, size_t start, size_t live_num);
    void processLoop(BasicBlock* header, LiveSet* live_set);

  private:
    std::vector<BasicBlock*> linear_bbs;
    std::unordered_map<Inst*, LiveInterval*> live_intervals;
    std::unordered_map<BasicBlock*, LiveSet*> live_sets;
};

class LiveInterval
{
  public:
    LiveInterval(size_t start_, size_t end_) : start(start_), end(end_)
    {}
    ~LiveInterval() = default;

    DEFINE_GETTER_SETTER(start, IntervalStart, size_t)
    DEFINE_GETTER_SETTER(end, IntervalEnd, size_t)

  private:
    size_t start = 0;
    size_t end = 0;
};

class LiveSet
{
  public:
    LiveSet() = default;
    ~LiveSet() = default;

    DEFINE_ARRAY_GETTER(live_set, LiveSet, std::set<Inst*>&)

    void addInst(Inst* inst)
    {
        live_set.insert(inst);
    }

    void deleteInst(Inst* inst)
    {
        live_set.erase(inst);
    }

    void unite(LiveSet* live)
    {
        for (auto* inst : live->getLiveSet())
            live_set.insert(inst);
    }

  private:
    std::set<Inst*> live_set;
};

} // namespace compiler