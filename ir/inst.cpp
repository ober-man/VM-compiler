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
        << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(op)] << " bb" << target->getId();
}

CallInst::CallInst(std::initializer_list<size_t> args_, size_t id_, Graph* g)
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
    out << "(";
    std::for_each(args.begin(), args.end(), [&out](auto arg) { out << arg->getId() << " "; });
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

} // namespace compiler