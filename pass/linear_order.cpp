#include "linear_order.h"
#include "loop_analysis.h"

namespace compiler
{

bool LinearOrder::runPassImpl()
{
    assert(graph != nullptr && "nullptr graph in LoopAnalysis pass");

    bool loops = graph->runPassLoopAnalysis();
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

static JumpOpType getInverseJumpType(JumpOpType type)
{
    switch (type)
    {
      case JumpOpType::Je:
        return JumpOpType::Jne;
      case JumpOpType::Jne:
        return JumpOpType::Je;
      case JumpOpType::Jb:
        return JumpOpType::Jae;
      case JumpOpType::Jbe:
        return JumpOpType::Ja;
      case JumpOpType::Ja:
        return JumpOpType::Jbe;
      case JumpOpType::Jae:
        return JumpOpType::Jb;
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
        auto* last = pred->getLastInst();
        if (last->getInstType() != InstType::Jump)
            return;
        auto* jump_inst = static_cast<JumpInst*>(last);
        jump_inst->setJumpOpType(getInverseJumpType(jump_inst->getJumpOpType()));
    }
}

} // namespace compiler