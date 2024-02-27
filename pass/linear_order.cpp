#include "linear_order.h"
#include "loop_analysis.h"

namespace compiler
{

bool LinearOrder::runPassImpl()
{
    ASSERT(graph != nullptr, "nullptr graph in LinearOrder pass");

    bool loops = graph->runPass<LoopAnalysis>();
    if (!loops)
        return false;

    mrk = graph->getNewMarker();

    processBBs();
    graph->setLinearOrderBBs(linear_bbs);

    graph->deleteMarker(mrk);
    return true;
}

void LinearOrder::processBBs()
{
    auto& rpo_bbs = graph->getRpoBBs();

    for (auto* bb : rpo_bbs)
    {
        if (bb->isMarked(mrk))
            continue;

        // loops have to lie contiguously
        if (bb->isHeader() && !bb->getLoop()->isIrreducible())
            processLoop(bb->getLoop());
        else
        {
            bb->setMarker(mrk);
            swapSuccessors(bb);
            linear_bbs.push_back(bb);
        }
    }
}

void LinearOrder::processLoop(Loop* loop)
{
    auto& body = loop->getBody();
    // blocks in loop lying in reversed order
    for (auto it = body.rbegin(), first = body.rend(); it != first; ++it)
    {
        auto* bb = *it;
        if (bb->isMarked(mrk))
            continue;

        // process inner loops
        if (bb->isHeader() && bb->getLoop() != loop)
            processLoop(bb->getLoop());
        else
        {
            bb->setMarker(mrk);
            swapSuccessors(bb);
            linear_bbs.push_back(bb);
        }
    }
}

static InstType getInverseJumpType(InstType type)
{
    ASSERT(type >= InstType::Jmp && type <= InstType::Jae, "wrong jump type");
    switch (type)
    {
        case InstType::Je:
            return InstType::Jne;
        case InstType::Jne:
            return InstType::Je;
        case InstType::Jb:
            return InstType::Jae;
        case InstType::Jbe:
            return InstType::Ja;
        case InstType::Ja:
            return InstType::Jbe;
        case InstType::Jae:
            return InstType::Jb;
        default:
            return type;
    }
}

void LinearOrder::swapSuccessors(BasicBlock* bb)
{
    if (linear_bbs.empty())
        return;

    auto* pred = linear_bbs.back();
    auto* true_succ = pred->getTrueSucc();
    if (true_succ != bb)
        return;

    pred->swapSuccs();
    if (pred->size() > 0)
    {
        auto* last_inst = pred->getLastInst();
        if (!last_inst->isJumpInst())
            return;
        last_inst->setInstType(getInverseJumpType(last_inst->getInstType()));
    }
}

} // namespace compiler