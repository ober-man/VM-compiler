#include "inline.h"

namespace compiler
{

bool Inline::runPassImpl()
{
    ASSERT(graph != nullptr, "nullptr graph in Inline pass");
    for (auto* bb : graph->getBBs())
        for (auto* inst = bb->getFirstInst(); inst != nullptr; inst = inst->getNext())
            if (inst->getInstType() == InstType::Call)
                inlineMethod(inst);
    return true;
}

void Inline::inlineMethod(Inst* inst)
{
    BasicBlock* caller_bb = inst->getBB();
    CallInst* call_inst = static_cast<CallInst*>(inst);
    auto* callee = call_inst->getFunc();

    auto* next_bb = caller_bb->splitBlockAfterInst(inst, callee);
    processInputs(call_inst, callee);
    processReturn(inst, callee);
    caller_bb->removeInst(inst);
    moveConstants(callee);
    doInline(callee);
    linkBlocks(caller_bb, next_bb, callee);
}

void Inline::processInputs(CallInst* call_inst, Graph* callee)
{
    auto& args = call_inst->getArgs();
    auto* callee_first_bb = callee->getFirstBB();
    for (auto* arg : args)
    {
        call_inst->removeUser(arg);
        auto* param = callee_first_bb->getFirstInst();
        auto& users = param->getUsers();
        for (auto* user : users)
        {
            // replace all callee params to args instructions
            arg->addUser(user);
            user->replaceInput(param, arg);
        }
        callee_first_bb->popFrontInst();
    }
}

void Inline::processReturn(Inst* call_inst, Graph* callee)
{
    auto* call_inst_bb = call_inst->getBB();
    std::vector<Inst*> returns;
    for (auto* bb : callee->getBBs())
        if (bb->getLastInst()->getInstType() == InstType::Return)
            returns.push_back(bb->getLastInst());

    if (returns.size() == 1)
    {
        // substitute return value inst to call users
        auto& call_users = call_inst->getUsers();
        auto* retval_inst = static_cast<UnaryInst*>(returns[0])->getInput(0);
        for (auto* user : call_users)
        {
            retval_inst->addUser(user);
            user->replaceInput(call_inst, retval_inst);
        }
    }
    else if (returns.size() > 1)
    {
        // create phi instruction to return value
        auto* phi_retval = new PhiInst{graph->getCurInstId()};
        call_inst_bb->pushBackPhiInst(phi_retval);
        for (auto* ret_inst : returns)
        {
            auto* retval_inst = static_cast<UnaryInst*>(ret_inst)->getInput(0);
            phi_retval->addInput(std::make_pair(retval_inst, retval_inst->getBB()));
            retval_inst->addUser(phi_retval);
        }

        auto& call_users = call_inst->getUsers();
        for (auto* user : call_users)
        {
            phi_retval->addUser(user);
            user->replaceInput(call_inst, phi_retval);
        }
    }

    for (auto* bb : callee->getBBs())
    {
        auto last_inst = bb->getLastInst();
        if (last_inst->getInstType() == InstType::Return ||
            last_inst->getInstType() == InstType::RetVoid)
        {
            auto* retval_inst = static_cast<UnaryInst*>(last_inst)->getInput(0);
            retval_inst->removeUser(last_inst);
            bb->popBackInst();
        }
    }
}

void Inline::moveConstants(Graph* callee)
{
    auto* firstBB = callee->getFirstBB();
    auto* cur_const = callee->getFirstConst();
    while (cur_const != nullptr)
    {
        auto type = cur_const->getType();
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const = graph->findConstant(cur_const->getInt32Value());
                break;
            case DataType::i64:
                new_const = graph->findConstant(cur_const->getInt64Value());
                break;
            case DataType::f32:
                new_const = graph->findConstant(cur_const->getFloatValue());
                break;
            case DataType::f64:
                new_const = graph->findConstant(cur_const->getDoubleValue());
                break;
            default:
                UNREACHABLE();
        }
        cur_const->replaceUsers(new_const);
        firstBB->popFrontInst();
        cur_const = callee->getFirstConst();
    }
}

void Inline::doInline(Graph* callee)
{
    auto cur_inst_id = graph->getCurInstId();
    auto& blocks = callee->getBBs();
    // we start from the 1 block (not 0) because 0 block is start
    // and it contains params and consts that we have already added
    for (auto it = std::next(blocks.begin()), end = blocks.end(); it != end; ++it)
    {
        auto* bb = (*it);
        for (auto* inst = bb->getFirstInst(); inst != nullptr; inst = inst->getNext())
            inst->setId(cur_inst_id++);

        graph->addBB(bb);
    }
    graph->setCurInstId(cur_inst_id);
}

void Inline::linkBlocks(BasicBlock* prev_bb, BasicBlock* next_bb, Graph* callee)
{
    auto first_bb = callee->getFirstBB()->getTrueSucc();
    auto last_bb = callee->getLastBB();
    ASSERT(first_bb != nullptr);
    first_bb->replacePred(callee->getFirstBB(), prev_bb);
    last_bb->addSucc(next_bb);
    prev_bb->addSucc(first_bb);
    next_bb->replacePred(prev_bb, last_bb);
}

} // namespace compiler