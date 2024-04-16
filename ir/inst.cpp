#include "inst.h"
#include "basicblock.h"
#include "graph.h"

namespace compiler
{

template <typename T, typename... Args>
Inst* createInst(Args&&... args)
{
    return T{std::forward<Args>(args)...};
}

// return true, if (*this) dominates inst
bool Inst::dominates(Inst* inst) const
{
    auto* other_bb = inst->getBB();
    if (bb != other_bb)
        return bb->dominates(other_bb);

    if (inst == this)
        return true;

    auto* cur_inst = next;
    while (cur_inst != nullptr)
    {
        if (cur_inst == inst)
            return true;
        cur_inst = cur_inst->getNext();
    }
    return false;
}

void Inst::dumpUsers(std::ostream& out) const
{
    auto size = users.size();
    if (size == 0)
        return;

    out << " ->"
        << " (";
    out << "v" << users.front()->getId();
    for (auto it = ++users.begin(), ite = users.end(); it != ite; ++it)
        out << ", v" << (*it)->getId();
    out << ")";
}

void JumpInst::dump(std::ostream& out) const
{
    out << "\t"
        << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " bb"
        << target->getId();
}

CallInst::CallInst(size_t id_, Graph* g, std::initializer_list<size_t> args_)
    : Inst(id_, InstType::Call), func(g)
{
    for (auto arg : args_)
    {
        auto* inst = bb->getInst(arg);
        args.push_back(inst);
        inst->addUser(this);
    }
}

void CallInst::dump(std::ostream& out) const
{
    out << "\t"
        << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " "
        << func->getName();
    if (args.size() == 0)
        return;
    out << "(v" << args[0]->getId();
    for (auto it = ++args.begin(), ite = args.end(); it != ite; ++it)
        out << ", v" << (*it)->getId();
    out << ")";
}

void PhiInst::dump(std::ostream& out) const
{
    out << "\t"
        << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " ";
    for (auto&& input : inputs)
        out << "("
            << "v" << input.first->getId() << ", bb" << input.second->getId() << ")";
}

void RetVoidInst::dump(std::ostream& out) const
{
    out << "\t"
        << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " ";
}

} // namespace compiler