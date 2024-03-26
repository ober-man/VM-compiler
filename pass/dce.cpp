#include "dce.h"

namespace compiler
{

bool Dce::isDceAppliable(Inst* inst)
{
    auto type = inst->getInstType();
    return !inst->isJumpInst() && type != InstType::Call && type != InstType::Mov &&
           type != InstType::Cmp && type != InstType::Return;
}

bool Dce::runPassImpl()
{
    ASSERT(graph != nullptr, "nullptr graph in DCE pass");

    // naive algorithm
    for (auto* bb : graph->getBBs())
    {
        auto* inst = bb->getFirstInst();
        while (inst != nullptr)
        {
            if (inst->getUsersNum() == 0 && isDceAppliable(inst))
            {
                auto* next_inst = inst->getNext();
                bb->removeInst(inst);
                inst = next_inst;
            }
            else
                inst = inst->getNext();
        }
    }
    return true;
}

} // namespace compiler