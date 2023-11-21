#include "inst.h"
#include "basicblock.h"
#include "graph.h"

namespace compiler
{

template <typename T, typename... Args>
Inst *createInst(Args &&... args)
{
    return T{std::forward<Args>(args)...};
}

void JumpInst::dump(std::ostream &out) const
{
    out << "\t"
        << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(op)] << " bb" << target->getId()
        << std::endl;
}

CallInst::CallInst(std::initializer_list<size_t> args_) : Inst(0, InstType::Call)
{
    for (auto arg : args_)
        args.push_back(bb->getInst(arg));
}

void CallInst::dump(std::ostream &out) const
{
    out << "\t"
        << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " "
        << func->getName();
    out << "(";
    std::for_each(args.begin(), args.end(), [&out](auto arg) { out << arg->getId() << " "; });
    out << ")" << std::endl;
}

void PhiInst::dump(std::ostream &out) const
{
    out << "\t"
        << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " ";
    for (auto &&input : inputs)
        out << "("
            << "v" << input.first->getId() << ", bb" << input.second->getId() << ") ";
    out << std::endl;
}

} // namespace compiler