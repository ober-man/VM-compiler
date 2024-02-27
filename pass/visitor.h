#pragma once

#include "ir/const.h"
#include "ir/graph.h"
#include "pass.h"

namespace compiler
{

class Visitor
{
  public:
    Visitor() = default;
    virtual ~Visitor() = default;

  protected:
#define CREATE_FUNC_DEFINITION(NAME, BASE)                                                         \
    virtual void visit##NAME([[maybe_unused]] Visitor* v, Inst* inst)                              \
    {                                                                                              \
        visitDefault(v, inst);                                                                     \
    }

    BINARY_OP_LIST(CREATE_FUNC_DEFINITION)
    UNARY_OP_LIST(CREATE_FUNC_DEFINITION)
    JUMP_OP_LIST(CREATE_FUNC_DEFINITION)
    INST_TYPE_LIST(CREATE_FUNC_DEFINITION)

#undef CREATE_FUNC_DEFINITION

    void visitGraph(Graph* graph)
    {
        for (auto bb : graph->getBBs())
            for (auto* inst = bb->getFirstInst(); inst != nullptr; inst = inst->getNext())
                (this->*VISITOR_FUNC_NAME[static_cast<uint8_t>(inst->getInstType())])(this, inst);
    }

    virtual void visitDefault([[maybe_unused]] Visitor* v, [[maybe_unused]] Inst* inst)
    {}

    // clang-format off
    using visitor_func_t = void (Visitor::*)(Visitor*, Inst*);
    static constexpr std::array<visitor_func_t, static_cast<uint8_t>(InstType::End)> 
   		VISITOR_FUNC_NAME = {

#define CREATE_FUNC_NAME(NAME, BASE) &Visitor::visit##NAME,

            &Visitor::visitDefault,
            BINARY_OP_LIST(CREATE_FUNC_NAME)
            UNARY_OP_LIST(CREATE_FUNC_NAME)
            JUMP_OP_LIST(CREATE_FUNC_NAME)
            INST_TYPE_LIST(CREATE_FUNC_NAME)

#undef CREATE_FUNC_NAME
    };
    // clang-format on
};

} // namespace compiler