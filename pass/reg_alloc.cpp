#include "reg_alloc.h"

namespace compiler
{

bool RegisterAllocation::runPassImpl()
{
    ASSERT(graph != nullptr, "nullptr graph in RegisterAllocation pass");

    bool liveness = graph->runPass<LivenessAnalysis>();
    if (!liveness)
        return false;

    getLiveIntervals();
    linearScan();
    return true;
}

void RegisterAllocation::getLiveIntervals()
{
    auto& intervals = graph->getLiveIntervals();
    for (auto [inst, live_int] : intervals)
        if (!live_int->isEmpty())
            live_intervals.push_back(live_int);

    std::sort(live_intervals.begin(), live_intervals.end(), [](auto* left, auto* right) {
        return left->getIntervalStart() < right->getIntervalStart();
    });
}

void RegisterAllocation::linearScan()
{
    for (auto* live_int : live_intervals)
    {
        expireOldIntervals(live_int);
        if (used_regs == REG_NUM)
        {
            spillAtInterval(live_int);
        }
        else
        {
            auto reg = getReg();
            live_int->setLocation(reg);
            active_intervals.insert(live_int);
        }
    }
}

void RegisterAllocation::expireOldIntervals(LiveInterval* curr)
{
    std::vector<LiveInterval*> erasing;
    for (auto* live_int : active_intervals)
    {
        if (live_int->getIntervalEnd() > curr->getIntervalStart())
            return;
        erasing.push_back(live_int);
        auto reg = live_int->getLocation();
        ASSERT(reg != INVALID_REG);
        releaseReg(reg);
    }

    for (auto* item : erasing)
        active_intervals.erase(item);
}

void RegisterAllocation::spillAtInterval(LiveInterval* curr)
{
    auto it = std::prev(active_intervals.end());
    auto spill = *it;
    if (spill->getIntervalEnd() > curr->getIntervalEnd())
    {
        auto reg = spill->getLocation();
        ASSERT(reg != INVALID_REG);
        curr->setLocation(reg);
        spill->setLocation(cur_spill_fill);
        spill->setNeedSpillFill();
        ++cur_spill_fill;
        active_intervals.erase(spill);
        active_intervals.insert(curr);
    }
    else
    {
        curr->setLocation(cur_spill_fill);
        curr->setNeedSpillFill();
        ++cur_spill_fill;
    }
}

} // namespace compiler