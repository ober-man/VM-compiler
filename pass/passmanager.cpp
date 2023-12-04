#include "passmanager.h"
#include "domtree.h"
#include "linear_order.h"
#include "liveness.h"
#include "loop_analysis.h"
#include "pass.h"
#include "rpo.h"

namespace compiler
{

PassManager::~PassManager()
{
    for (auto* analysis : analyses)
        delete analysis;

    for (auto* opt : opts)
        delete opt;
}

// template <typename PassName, typename... Args>
bool PassManager::runPassRpo(marker_t marker)
{
    auto* pass = new Rpo{graph, marker};
    /*if (pass->isAnalysis())
        analyses.push_back(static_cast<Analysis*>(pass));
    else if (pass->isOptimization())
        opts.push_back(static_cast<Optimization*>(pass));
    else
        UNREACHABLE();*/

    if (!pass->runPassImpl())
    {
        std::cerr << "Pass RPO failed" << std::endl;
        return false;
    }
    return true;
}

bool PassManager::runPassDomTree()
{
    auto* pass = new DomTree{graph};

    if (!pass->runPassImpl())
    {
        std::cerr << "Pass DomTree failed" << std::endl;
        return false;
    }
    return true;
}

bool PassManager::runPassLoopAnalysis()
{
    auto* pass = new LoopAnalysis{graph};

    if (!pass->runPassImpl())
    {
        std::cerr << "Pass LoopAnalysis failed" << std::endl;
        return false;
    }
    return true;
}

bool PassManager::runPassLinearOrder()
{
    auto* pass = new LinearOrder{graph};

    if (!pass->runPassImpl())
    {
        std::cerr << "Pass LinearOrder failed" << std::endl;
        return false;
    }
    return true;
}

bool PassManager::runPassLivenessAnalysis()
{
    auto* pass = new LivenessAnalysis{graph};

    if (!pass->runPassImpl())
    {
        std::cerr << "Pass LivenessAnalysis failed" << std::endl;
        return false;
    }
    return true;
}

void PassManager::dumpAnalyses(std::ostream& out)
{
    std::for_each(analyses.begin(), analyses.end(),
                  [&out](auto* analysis) { out << analysis->getAnalysisName() << " "; });
}

void PassManager::dumpOpts(std::ostream& out)
{
    std::for_each(opts.begin(), opts.end(), [&out](auto* opt) { out << opt->getOptName() << " "; });
}

} // namespace compiler