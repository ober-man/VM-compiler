#pragma once

#include "ir/graph.h"

namespace compiler
{

class Graph;

class Pass
{
  public:
    explicit Pass(Graph* g) : graph(g)
    {}
    virtual ~Pass() = default;

    virtual bool runPassImpl() = 0;

    bool isAnalysis() const noexcept
    {
        return is_analysis;
    }

    bool isOptimization() const noexcept
    {
        return is_optimization;
    }

    Graph* getGraph() const noexcept
    {
        return graph;
    }

  protected:
    Graph* graph = nullptr;
    bool is_analysis = false;
    bool is_optimization = false;
};

class Analysis : public Pass
{
  public:
    explicit Analysis(Graph* g) : Pass(g)
    {
        is_analysis = true;
    }

    virtual ~Analysis() = default;
    virtual std::string getAnalysisName() const = 0;

    // some passes can invalidate analysis
    // to fix it rerun runImpl()
    virtual void invalidateAnalysis() noexcept
    {
        is_valid = false;
    }

    bool isValid() const noexcept
    {
        return is_valid;
    }

  protected:
    bool is_valid = false;
};

class Optimization : public Pass
{
  public:
    explicit Optimization(Graph* g) : Pass(g)
    {
        is_optimization = true;
    }

    virtual ~Optimization() = default;

    virtual std::string getOptName() const = 0;
};

} // namespace compiler