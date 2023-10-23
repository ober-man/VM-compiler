#pragma once

#include "basicblock.h"
#include "pass/passmanager.h"

namespace compiler
{

#define GRAPH_BB_NUM 50

class PassManager;
class Rpo;
class DomTree;
class Loop;
class Loop_Analyzer;

class Graph
{
  public:
    Graph(std::string name = "") : func_name(name)
    {
        BBs.reserve(GRAPH_BB_NUM);
        rpo_BBs.reserve(GRAPH_BB_NUM);
        graph_size = 0;
        pm = std::make_unique<PassManager>(this);
    }

    ~Graph()
    {
        for (auto *bb : BBs)
            delete bb;
        if (loop != nullptr)
            delete loop;
    }

    size_t size() const noexcept
    {
        return graph_size;
    }

    bool isEmpty() noexcept
    {
        return graph_size == 0;
    }

    std::string getName() const noexcept
    {
        return func_name;
    }

    void setName(std::string &name) noexcept
    {
        func_name = name;
    }

    std::vector<BasicBlock *> &getBBs() noexcept
    {
        return BBs;
    }

    std::vector<BasicBlock *> &getRpoBBs() noexcept
    {
        return rpo_BBs;
    }

    void setRPOBBs(std::vector<BasicBlock *> &rpo) noexcept
    {
        rpo_BBs = rpo;
    }

    BasicBlock *getFirstBB() const noexcept
    {
        if (graph_size == 0)
            return nullptr;
        return BBs[0];
    }

    BasicBlock *getLastBB() const noexcept
    {
        if (graph_size == 0)
            return nullptr;
        return BBs[graph_size - 1];
    }

    BasicBlock *getBB(size_t id) const noexcept
    {
        if (id >= graph_size)
            return nullptr;
        else
            return BBs[id];
    }

    void insertBB(BasicBlock *bb)
    {
        if (graph_size == 0)
        {
            BBs.push_back(bb);
            ++graph_size;
            return;
        }

        auto *pred = BBs[graph_size - 1];
        pred->addSucc(bb);
        bb->addPred(pred);
        BBs.push_back(bb);
        ++graph_size;
    }

    /**
     * Insert bb after prev_bb
     */
    void insertBBAfter(BasicBlock *prev_bb, BasicBlock *bb, bool is_true_succ = true)
    {
        if (is_true_succ)
        {
            auto *true_succ = prev_bb->getTrueSucc();
            prev_bb->setTrueSucc(bb);
            bb->addPred(prev_bb);

            if (true_succ != nullptr)
                true_succ->addPred(bb);
        }
        else
        {
            auto *false_succ = prev_bb->getFalseSucc();
            prev_bb->setFalseSucc(bb);
            bb->addPred(prev_bb);

            if (false_succ != nullptr)
                false_succ->addPred(bb);
        }
        BBs.push_back(bb);
        ++graph_size;
    }

    void addEdge(BasicBlock *prev_bb, BasicBlock *bb);

    void removeBB(BasicBlock *bb)
    {
        auto it = BBs.erase(std::find(BBs.begin(), BBs.end(), bb));
        assert(it != BBs.end() && "remove not existing bb");
    }

    void removeBB(size_t num)
    {
        auto it = BBs.erase(
            std::find_if(BBs.begin(), BBs.end(), [num](auto bb) { return bb->getId() == num; }));
        assert(it != BBs.end() && "remove not existing bb");
    }

    void replaceBB(BasicBlock *bb, BasicBlock *new_bb);
    void replaceBB(size_t num, BasicBlock *new_bb);

    void dump(std::ostream &out = std::cout)
    {
        std::cout << "Graph for proc " << func_name << std::endl;
        std::for_each(BBs.begin(), BBs.end(), [&out](auto bb) { bb->dump(out); });
    }

    void dumpDomTree(std::ostream &out = std::cout) const
    {
        // TODO after implementing fast DomTree
    }

    // template <typename PassName>
    // TODO: fix PassManager
    // [some template magic was broken and there is crutch]
    void runPassRpo();
    void runPassDomTree();
    void runPassLoopAnalyzer();

  private:
    std::string func_name = "";
    size_t graph_size = 0;

    std::vector<BasicBlock *> BBs;
    std::vector<BasicBlock *> rpo_BBs;

    std::unique_ptr<PassManager> pm = nullptr;
    Loop *loop = nullptr;
};
/*
template <typename PassName>
void Graph::runPass()
{
    pm->template runPass<PassName>();
}
*/

} // namespace compiler