#include "checks_elimination.h"
#include "domtree.h"

namespace compiler
{

bool ChecksElimination::runPassImpl()
{
    ASSERT(graph != nullptr, "nullptr graph in ChecksElimination pass");

    bool domtree = graph->runPass<DomTree>();
    if (!domtree)
        return false;

    visitGraph(graph);
    return true;
}

void ChecksElimination::visitZeroCheck([[maybe_unused]] Visitor* v, Inst* inst)
{
    auto* input = static_cast<UnaryInst*>(inst)->getInput(0);
    for (auto* user : input->getUsers())
    {
        if (user->getInstType() == InstType::ZeroCheck && user != inst &&
            user->dominates(inst))
        {
            auto* bb = inst->getBB();
            input->removeUser(inst);
            bb->removeInst(inst);
            return;
        }
    }
}

void ChecksElimination::visitBoundsCheck([[maybe_unused]] Visitor* v, Inst* inst)
{
    auto* input = static_cast<BinaryInst*>(inst)->getInput(0);
    auto* index = static_cast<BinaryInst*>(inst)->getInput(1);
    for (auto* user : input->getUsers())
    {
        if (user->getInstType() == InstType::BoundsCheck && user != inst &&
            static_cast<BinaryInst*>(user)->getInput(1) == index && user->dominates(inst))
        {
            auto* bb = inst->getBB();
            input->removeUser(inst);
            index->removeUser(inst);
            bb->removeInst(inst);
            return;
        }
    }
}

} // namespace compiler