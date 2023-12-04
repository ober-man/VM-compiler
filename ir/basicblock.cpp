#include "basicblock.h"
#include "graph.h"
#include "pass/liveness.h"
#include "pass/loop_analysis.h"

namespace compiler
{

BasicBlock::~BasicBlock()
{
    Inst* inst = first_inst;
    while (inst != nullptr)
    {
        first_inst = inst->getNext();
        delete inst;
        inst = first_inst;
    }
    if (live_int != nullptr)
        delete live_int;
}

void BasicBlock::setId(size_t id_) noexcept
{
    assert(graph && !graph->getBB(id_) && "BB id already existed");
    id = id_;
}

void BasicBlock::replacePred(BasicBlock* pred, BasicBlock* bb)
{
    auto it = std::find(preds.begin(), preds.end(), pred);
    assert(it != preds.end() && "replace not existing pred");
    preds[(*it)->getId()] = bb;
}

void BasicBlock::replacePred(size_t num, BasicBlock* bb)
{
    auto it =
        std::find_if(preds.begin(), preds.end(), [num](auto pred) { return pred->getId() == num; });
    assert(it != preds.end() && "replace not existing pred");
    preds[num] = bb;
}

void BasicBlock::setMarker(marker_t marker)
{
    markers->setMarker(marker);
}

void BasicBlock::resetMarker(marker_t marker)
{
    markers->resetMarker(marker);
}

bool BasicBlock::isMarked(marker_t marker) const
{
    return markers->isMarked(marker);
}

bool BasicBlock::isHeader() const noexcept
{
    return loop->getHeader() == this;
}

void BasicBlock::dump(std::ostream& out) const
{
    out << "BB " << name << "[" << id << "/" << graph->size() << "]" << std::endl;
    out << "preds : ";
    std::for_each(preds.begin(), preds.end(),
                  [&out](auto pred) { out << "bb" << pred->getId() << " "; });
    out << "\n";

    for (auto* phi = getFirstPhi(); phi != nullptr; phi = phi->getNext())
        phi->dump(out);

    for (auto* inst = first_inst; inst != nullptr; inst = inst->getNext())
        inst->dump(out);

    out << "succs : ";
    if (true_succ != nullptr)
        out << "true bb" << true_succ->getId();
    if (false_succ != nullptr)
        out << ", false bb" << false_succ->getId();
    out << "\n"
        << "\n";
}

} // namespace compiler