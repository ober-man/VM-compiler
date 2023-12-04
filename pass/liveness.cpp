#include "liveness.h"
#include "linear_order.h"
#include "loop_analysis.h"

namespace compiler
{

#define LINEAR_NUMBER_STEP 1
#define LIVE_NUMBER_STEP 2

LivenessAnalysis::~LivenessAnalysis()
{
    for (auto [bb, live_set] : live_sets)
        delete live_set;
}

bool LivenessAnalysis::runPassImpl()
{
    assert(graph != nullptr && "nullptr graph in LoopAnalysis pass");

    bool linear = graph->runPassLinearOrder();
    if (!linear)
        return false;

    linear_bbs = graph->getLinearOrderBBs();

    setInstsInitialNumbers();
    buildLiveIntervals();

    graph->setLiveIntervals(live_intervals);
    return true;
}

void LivenessAnalysis::insertInstLiveInterval(Inst* inst, size_t start, size_t end)
{
    auto it = live_intervals.find(inst);
    if (it == live_intervals.end())
        live_intervals[inst] = new LiveInterval{start, end};
    else
    {
        auto* cur_interval = (*it).second;
        auto cur_start = cur_interval->getIntervalStart();
        auto cur_end = cur_interval->getIntervalEnd();

        // TODO: think about live holes
        live_intervals[inst]->setIntervalStart(std::min(start, cur_start));
        live_intervals[inst]->setIntervalEnd(std::max(end, cur_end));
    }
}

void LivenessAnalysis::setInstsInitialNumbers()
{
    size_t cur_lin_num = 0;
    size_t cur_live_num = 0;

    for (auto* bb : linear_bbs)
    {
        auto live = new LiveInterval{cur_live_num, cur_live_num};
        for (auto* phi = bb->getFirstPhi(); phi != nullptr; phi = phi->getNext())
        {
            phi->setLinearNum(cur_lin_num);
            phi->setLiveNum(cur_live_num);
            cur_lin_num += LINEAR_NUMBER_STEP;
        }

        for (auto* inst = bb->getFirstInst(); inst != nullptr; inst = inst->getNext())
        {
            inst->setLinearNum(cur_lin_num);
            inst->setLiveNum(cur_live_num);
            cur_lin_num += LINEAR_NUMBER_STEP;
            cur_live_num += LIVE_NUMBER_STEP;
        }
        live->setIntervalEnd(cur_live_num);
        bb->setLiveInterval(live);
    }
}

void LivenessAnalysis::buildLiveIntervals()
{
    for (auto it = linear_bbs.rbegin(), first = linear_bbs.rend(); it != first; ++it)
    {
        auto* bb = *it;
        auto* live_set = calcInitLiveSets(bb);

        appendBBIntervals(live_set, bb->getLiveInterval());
        processBBInsts(bb, live_set);

        if (bb->isHeader() && !bb->getLoop()->isIrreducible())
            processLoop(bb, live_set);
    }
}

LiveSet* LivenessAnalysis::calcInitLiveSets(BasicBlock* bb)
{
    auto* live_set = new LiveSet{};
    live_sets[bb] = live_set;

    auto* true_succ = bb->getTrueSucc();
    auto* false_succ = bb->getFalseSucc();

    if (true_succ != nullptr)
        processSucc(bb, true_succ, live_set);

    if (false_succ != nullptr)
        processSucc(bb, false_succ, live_set);

    return live_set;
}

void LivenessAnalysis::processSucc(BasicBlock* bb, BasicBlock* succ, LiveSet* live_set)
{
    live_set->unite(live_sets[succ]);
    for (auto* phi = succ->getFirstPhi(); phi != nullptr; phi = phi->getNext())
    {
        auto& phi_inputs = static_cast<PhiInst*>(phi)->getInputs();
        for (auto& input : phi_inputs)
            if (input.second == bb)
                live_set->addInst(input.first);
    }
}

void LivenessAnalysis::appendBBIntervals(LiveSet* live_set, LiveInterval* live_int)
{
    auto& live_insts = live_set->getLiveSet();
    auto start = live_int->getIntervalStart();
    auto end = live_int->getIntervalEnd();

    for (auto* inst : live_insts)
        insertInstLiveInterval(inst, start, end);
}

void LivenessAnalysis::processBBInsts(BasicBlock* bb, LiveSet* live_set)
{
    for (auto* inst = bb->getLastInst(); inst != nullptr; inst = inst->getPrev())
    {
        auto live_num = inst->getLiveNum();
        insertInstLiveInterval(inst, live_num, live_num + LIVE_NUMBER_STEP);
        /*
        auto it = live_intervals.find(inst);
        auto live_num = inst->getLiveNum();
        if (it == live_intervals.end())
            live_intervals[inst] = new LiveInterval{live_num, live_num + LIVE_NUMBER_STEP};
        else
            (*it)->setIntervalStart(live_num);*/

        processInstInputs(inst, live_set, bb->getLiveInterval()->getIntervalStart());
        live_set->deleteInst(inst);
    }

    for (auto* phi = bb->getLastPhi(); phi != nullptr; phi = phi->getPrev())
        live_set->deleteInst(phi);
}

void LivenessAnalysis::processInstInputs(Inst* inst, LiveSet* live_set, size_t start)
{
    switch (inst->getInstType())
    {
        case InstType::Binary:
        {
            auto* bin_inst = static_cast<FixedInputsInst<2>*>(inst);
            auto num = bin_inst->getLiveNum();
            processInput(bin_inst->getInput(0), live_set, start, num);
            processInput(bin_inst->getInput(1), live_set, start, num);
            break;
        }
        case InstType::Unary:
        case InstType::Cast:
        case InstType::Mov:
        {
            auto* un_inst = static_cast<FixedInputsInst<1>*>(inst);
            auto num = un_inst->getLiveNum();
            processInput(un_inst->getInput(0), live_set, start, num);
            break;
        }

        case InstType::Call:
        {
            auto* call_inst = static_cast<CallInst*>(inst);
            auto num = call_inst->getLiveNum();
            auto& args = call_inst->getArgs();
            for (auto* arg : args)
                processInput(arg, live_set, start, num);
            break;
        }

        default:
            break;
    }
}

void LivenessAnalysis::processInput(Inst* input, LiveSet* live_set, size_t start, size_t live_num)
{
    live_set->addInst(input);
    insertInstLiveInterval(input, start, live_num);
}

void LivenessAnalysis::processLoop(BasicBlock* header, LiveSet* live_set)
{
    auto* loop = header->getLoop();
    auto start = header->getLiveInterval()->getIntervalStart();
    auto& body = loop->getBody();
    auto end = body[body.size() - 1]->getLiveInterval()->getIntervalEnd();
    auto& insts = live_set->getLiveSet();

    for (auto* inst : insts)
        insertInstLiveInterval(inst, start, end);
}

} // namespace compiler