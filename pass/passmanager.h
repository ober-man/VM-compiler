#pragma once

#include "ir/graph.h"

namespace compiler
{

class PassManager final
{
  public:
    PassManager() = default;
    ~PassManager() = default;

    // TODO: make concept
    template <typename Pass>
    void RunPass(std::shared_ptr<Graph> graph)
    {
        Pass pass;
        pass.RunPassImpl(graph);
    }
};

class Pass
{
  public:
    Pass() = default;
    virtual ~Pass() = default;

    virtual void RunPassImpl(std::shared_ptr<Graph> graph) = 0;
};

class Optimization : public Pass
{
  public:
    Optimization() = default;
    virtual ~Optimization() = default;
};

class RPO;
class DomTree;

class Analysis : public Pass
{
  public:
    Analysis() = default;
    virtual ~Analysis() = default;
};

} // namespace compiler