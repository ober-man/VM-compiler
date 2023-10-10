#include "basicblock.h"

namespace compiler
{

void BasicBlock::setId(size_t id_)
{
    assert(graph && !graph->getBB(id_) && "BB id already existed");
    id = id_;
}

void replacePred(std::shared_ptr<BasicBlock> pred,
                 std::shared_ptr<BasicBlock> bb)
{
    auto it = std::find(preds.begin(), preds.end(), pred);
    assert(it != preds.end() && "replace not existing pred");
    preds[it->getId()] = bb;
}

void replacePred(size_t num, std::shared_ptr<BasicBlock> bb)
{
    auto it = std::find_if(preds.begin(), preds.end(),
                           [num](auto pred) { return pred->getId() == num; });
    assert(it != preds.end() && "replace not existing pred");
    preds[num] = bb;
}

void dump(std::ostream &out = std::cout)
{
    out << "BB " << name << "[" << id << "/" << graph->bb_size() << "]"
        << std::endl;
    out << "preds : ";
    std::for_each(preds.begin(), preds.end(),
                  [&out](auto pred) { out << pred.getId() << " "; });
    out << "\n";

    for (auto inst = first_inst; inst != last_inst; inst->getNext())
        inst->dump(out);

    out << "succs : ";
    if (true_succ)
        out << "true " << true_succ->getId();
    if (false_succ)
        out << ", false" << false_succ->getId();
    out << "\n"
        << "\n";
}

void dumpDomTree(std::ostream &out = std::cout)
{
    out << "BB " << name << "[" << id << "/" << graph->bb_size() << "]"
        << std::endl;
    out << "Dominators :"
        << "\n";
    out << "\t";

    for (auto dom : dominated)
    {
        dom->dumpDomTree(out);
    }
}

} // namespace compiler