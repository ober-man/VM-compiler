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

void BasicBlock::pushBackInst(Inst* inst)
{
    ASSERT(inst->getInstType() != InstType::Phi);
    ASSERT(!inst->getPrev(), "inserted inst has predecessor");

    if (first_inst == nullptr)
        first_inst = inst;

    inst->setPrev(last_inst);
    inst->setNext(nullptr);
    inst->setBB(this);

    if (last_inst != nullptr)
        last_inst->setNext(inst);
    last_inst = inst;
    ++bb_size;
}

void BasicBlock::pushBackPhiInst(PhiInst* inst)
{
    ASSERT(inst->getInstType() == InstType::Phi);
    ASSERT(!inst->getPrev(), "inserted inst has predecessor");

    if (first_phi == nullptr)
        first_phi = inst;

    inst->setPrev(last_phi);
    inst->setNext(nullptr);
    inst->setBB(this);

    if (last_phi != nullptr)
        last_phi->setNext(inst);
    last_phi = inst;
    ++bb_size;
}

void BasicBlock::pushFrontInst(Inst* inst)
{
    ASSERT(inst->getInstType() != InstType::Phi);
    ASSERT(!inst->getNext(), "inserted inst has successor");

    if (!last_inst)
        last_inst = inst;

    inst->setPrev(nullptr);
    inst->setNext(first_inst);
    inst->setBB(this);

    if (first_inst)
        first_inst->setPrev(inst);
    first_inst = inst;
    ++bb_size;
}

void BasicBlock::pushFrontPhiInst(PhiInst* inst)
{
    ASSERT(inst->getInstType() == InstType::Phi);
    ASSERT(!inst->getNext(), "inserted inst has successor");

    if (!last_phi)
        last_phi = inst;

    inst->setPrev(nullptr);
    inst->setNext(first_phi);
    inst->setBB(this);

    if (first_phi)
        first_phi->setPrev(inst);
    first_phi = inst;
    ++bb_size;
}

/**
 * Insert inst after prev_inst
 */
void BasicBlock::insertAfter(Inst* prev_inst, Inst* inst)
{
    ASSERT(!inst->getPrev(), "inserted inst has predecessor");
    ASSERT(!inst->getNext(), "inserted inst has successor");

    if (!prev_inst->getNext())
    {
        pushBackInst(inst);
        return;
    }

    auto next_inst = prev_inst->getNext();
    next_inst->setPrev(inst);
    prev_inst->setNext(inst);

    inst->setNext(next_inst);
    inst->setPrev(prev_inst);
    inst->setBB(this);
    ++bb_size;
}

void BasicBlock::popFrontInst()
{
    ASSERT(first_inst, "first inst not existed");
    auto second_inst = first_inst->getNext();
    second_inst->setPrev(nullptr);
    first_inst->setNext(nullptr);
    first_inst = second_inst;
    --bb_size;
}

void BasicBlock::popBackInst()
{
    ASSERT(last_inst, "last inst not existed");
    auto prev_inst = last_inst->getPrev();
    prev_inst->setNext(nullptr);
    last_inst->setPrev(nullptr);
    last_inst = prev_inst;
    --bb_size;
}

void BasicBlock::removeInst(Inst* inst)
{
    auto next_inst = inst->getNext();
    auto prev_inst = inst->getPrev();
    if (next_inst)
        next_inst->setPrev(prev_inst);
    if (prev_inst)
        prev_inst->setNext(next_inst);
    --bb_size;
}

void BasicBlock::addPred(BasicBlock* bb)
{
    ASSERT(std::find(preds.begin(), preds.end(), bb) == preds.end(), "pred already existed");
    preds.push_back(bb);
}

void BasicBlock::addSucc(BasicBlock* bb)
{
    if (true_succ == nullptr)
        true_succ = bb;
    else if (false_succ == nullptr)
        false_succ = bb;
    else
    {
        std::cerr << "cannot add succ" << std::endl;
        abort();
    }
}

void BasicBlock::removePred(BasicBlock* bb)
{
    preds.erase(std::find(preds.begin(), preds.end(), bb));
}

void BasicBlock::removePred(size_t num)
{
    preds.erase(std::find_if(preds.begin(), preds.end(),
                             [num](auto pred) { return pred->getId() == num; }));
}

void BasicBlock::removeSucc(BasicBlock* bb)
{
    if (true_succ == bb)
        true_succ = nullptr;
    else if (false_succ == bb)
        false_succ = nullptr;
    else
        return;
}

void BasicBlock::removeSucc(size_t num)
{
    if (true_succ->getId() == num)
        true_succ = nullptr;
    else if (false_succ->getId() == num)
        false_succ = nullptr;
    else
        return;
}

void BasicBlock::replacePred(BasicBlock* pred, BasicBlock* bb)
{
    auto it = std::find(preds.begin(), preds.end(), pred);
    ASSERT(it != preds.end(), "replace not existing pred");
    preds[(*it)->getId()] = bb;
}

void BasicBlock::replacePred(size_t num, BasicBlock* bb)
{
    auto it =
        std::find_if(preds.begin(), preds.end(), [num](auto pred) { return pred->getId() == num; });
    ASSERT(it != preds.end(), "replace not existing pred");
    preds[num] = bb;
}

void BasicBlock::replaceSucc(BasicBlock* succ, BasicBlock* bb)
{
    if (true_succ == succ)
        true_succ = bb;
    else if (false_succ == succ)
        false_succ = bb;
    else
    {
        std::cerr << "replace not existing succ" << std::endl;
        abort();
    }
}

void BasicBlock::replaceSucc(size_t num, BasicBlock* bb)
{
    if (true_succ->getId() == num)
        true_succ = bb;
    else if (false_succ->getId() == num)
        false_succ = bb;
    else
    {
        std::cerr << "replace not existing succ" << std::endl;
        abort();
    }
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
    {
        phi->dump(out);
        phi->dumpUsers(out);
        out << "\n";
    }

    for (auto* inst = first_inst; inst != nullptr; inst = inst->getNext())
    {
        inst->dump(out);
        inst->dumpUsers(out);
        out << "\n";
    }

    out << "succs : ";
    if (true_succ != nullptr)
        out << "true bb" << true_succ->getId();
    if (false_succ != nullptr)
        out << ", false bb" << false_succ->getId();
    out << "\n"
        << "\n";
}

} // namespace compiler