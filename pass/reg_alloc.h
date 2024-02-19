#pragma once

#include "ir/graph.h"
#include "liveness.h"
#include "pass.h"
#include <bitset>
#include <set>
#include <vector>

namespace compiler
{

class LiveInterval;

constexpr size_t REG_NUM = 2;
constexpr size_t INST_NUM = 50;

class RegisterAllocation final : public Optimization
{
  public:
    explicit RegisterAllocation(Graph* g) : Optimization(g), active_intervals(&decrIntervalEndComp)
    {
        live_intervals.reserve(GRAPH_INST_NUM);
    }

    ~RegisterAllocation() override = default;

    bool runPassImpl() override;

    std::string getOptName() const noexcept override
    {
        return "RegisterAllocation";
    }

  private:
    size_t getReg()
    {
        if (used_regs == REG_NUM)
            return INVALID_REG;
        for (size_t i = 0; i < REG_NUM; ++i)
            if (regs[i] == 0)
            {
                regs[i] = 1;
                ++used_regs;
                return i;
            }
        UNREACHABLE();
    }

    void releaseReg(size_t number)
    {
        ASSERT(number < REG_NUM, "incorrect reg number");
        regs[number] = 0;
        --used_regs;
    }

    void getLiveIntervals();
    void linearScan();
    void expireOldIntervals(LiveInterval* curr);
    void spillAtInterval(LiveInterval* curr);

    static bool decrIntervalEndComp(LiveInterval* left, LiveInterval* right)
    {
        return left->getIntervalEnd() < right->getIntervalEnd();
    }
    using decrIntervalEndComp_t = decltype(&decrIntervalEndComp);

  private:
    std::vector<LiveInterval*> live_intervals;
    std::set<LiveInterval*, decrIntervalEndComp_t> active_intervals;
    std::bitset<REG_NUM> regs; // 1 if used, 0 if free
    size_t used_regs = 0;
    size_t cur_spill_fill = 0;
};

} // namespace compiler