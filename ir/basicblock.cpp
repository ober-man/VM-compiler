#include "basicblock.h"
#include "graph.h"

namespace compiler
{

void BasicBlock::setId(size_t id_) noexcept
{
    assert(graph && !graph->getBB(id_) && "BB id already existed");
    id = id_;
}

void BasicBlock::replacePred(BasicBlock *pred, BasicBlock *bb)
{
    auto it = std::find(preds.begin(), preds.end(), pred);
    assert(it != preds.end() && "replace not existing pred");
    preds[(*it)->getId()] = bb;
}

void BasicBlock::replacePred(size_t num, BasicBlock *bb)
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

bool BasicBlock::isMarked(marker_t marker)
{
    return markers->isMarked(marker);
}

void BasicBlock::dump(std::ostream &out) const
{
    out << "BB " << name << "[" << id << "/" << graph->size() << "]" << std::endl;
    out << "preds : ";
    std::for_each(preds.begin(), preds.end(),
                  [&out](auto pred) { out << "bb" << pred->getId() << " "; });
    out << "\n";

    for (auto inst = first_inst; inst != nullptr; inst = inst->getNext())
        inst->dump(out);

    out << "succs : ";
    if (true_succ != nullptr)
        out << "true bb" << true_succ->getId();
    if (false_succ != nullptr)
        out << ", false bb" << false_succ->getId();
    out << "\n"
        << "\n";
}

void BasicBlock::dumpDomTree(std::ostream &out) const
{
    // TODO after implementing fast DomTree
}

} // namespace compiler