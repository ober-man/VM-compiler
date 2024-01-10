#include "loop_analysis.h"
#include "domtree.h"
#include "rpo.h"

namespace compiler
{

bool LoopAnalysis::runPassImpl()
{
    ASSERT(graph != nullptr, "nullptr graph in LoopAnalysis pass");

    bool rpo = graph->runPass<Rpo>();
    bool domtree = graph->runPass<DomTree>();
    if (!rpo || !domtree)
        return false;

    gray_mrk = graph->getNewMarker();
    black_mrk = graph->getNewMarker();

    auto* firstBB = graph->getFirstBB();
    findLoopsRec(firstBB, nullptr);

    graph->deleteMarker(gray_mrk);
    graph->deleteMarker(black_mrk);

    populateLoops();
    buildLoopTree();
    return true;
}

void LoopAnalysis::findLoopsRec(BasicBlock* bb, BasicBlock* prev_bb)
{
    if (bb->isMarked(gray_mrk))
    {
        // we met a back edge
        Loop* loop = bb->getLoop();
        if (loop != nullptr)
        {
            loop->addLatch(prev_bb);
            return;
        }

        bool is_irreducible = !bb->isDominator(prev_bb);
        loop = new Loop{/*header=*/bb, /*latch=*/prev_bb, is_irreducible};
        bb->setLoop(loop);
        return;
    }

    if (bb->isMarked(black_mrk))
    {
        // we met a cross edge
        return;
    }

    // mark bb with both markers
    bb->setMarker(gray_mrk);
    bb->setMarker(black_mrk);

    // visit successors
    auto* true_succ = bb->getTrueSucc();
    auto* false_succ = bb->getFalseSucc();
    if (true_succ != nullptr)
        findLoopsRec(true_succ, bb);
    if (false_succ != nullptr)
        findLoopsRec(false_succ, bb);

    // visit all successors -> unmark bb
    bb->resetMarker(gray_mrk);
}

void LoopAnalysis::populateLoops()
{
    auto& rpo_bbs = graph->getRpoBBs();
    for (auto it = rpo_bbs.rbegin(), first = rpo_bbs.rend(); it != first; ++it)
    {
        auto* bb = *it;
        auto* loop = bb->getLoop();

        // consider only loop headers
        if (loop == nullptr || loop->getHeader() != bb)
            continue;

        auto& latches = loop->getLatches();

        if (loop->isIrreducible())
        {
            // add all latches to loop
            for (auto latch : latches)
                if (latch->getLoop() != loop)
                {
                    loop->addBlock(latch);
                    latch->setLoop(loop);
                }
            loop->addBlock(bb);
        }
        else
        {
            gray_mrk = graph->getNewMarker();
            bb->setMarker(gray_mrk);

            // fill loop body going up from latch to header
            for (auto latch : latches)
                fillLoopRec(loop, latch);

            loop->addBlock(bb);
            graph->deleteMarker(gray_mrk);
        }
    }
}

void LoopAnalysis::fillLoopRec(Loop* loop, BasicBlock* bb)
{
    if (bb->isMarked(gray_mrk))
    {
        // we reach a header or already visited node
        return;
    }

    bb->setMarker(gray_mrk);

    // add inner-outer loops
    Loop* bb_loop = bb->getLoop();
    if (bb_loop != nullptr)
    {
        if (bb_loop->getOuterLoop() == nullptr)
        {
            bb_loop->setOuterLoop(loop);
            loop->addInner(bb_loop);
        }
        if (bb_loop->getHeader() == bb)
            loop->addBlock(bb);
    }
    else
    {
        bb->setLoop(loop);
        loop->addBlock(bb);
    }

    // process blocks going up
    auto& preds = bb->getPreds();
    for (auto* pred : preds)
        fillLoopRec(loop, pred);
}

void LoopAnalysis::buildLoopTree()
{
    Loop* root_loop = new Loop{nullptr};
    auto& bbs = graph->getBBs();

    for (auto* graph_bb : bbs)
    {
        auto bb_loop = graph_bb->getLoop();
        if (bb_loop == nullptr)
        {
            graph_bb->setLoop(root_loop);
            root_loop->addBlock(graph_bb);
        }
        else if (bb_loop->getOuterLoop() == nullptr)
        {
            root_loop->addInner(bb_loop);

            // for irreducible loops
            auto& bbs = bb_loop->getBody();
            for (auto* loop_bb : bbs)
            {
                auto* loop = loop_bb->getLoop();
                if (loop->getOuterLoop() == nullptr)
                    loop->setOuterLoop(root_loop);
            }
        }
    }
    graph->setRootLoop(root_loop);
}

} // namespace compiler