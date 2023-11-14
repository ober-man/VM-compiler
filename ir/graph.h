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
class LoopAnalyzer;

class Graph
{
  public:
    Graph(std::string name = "") : func_name(name)
    {
        BBs.reserve(GRAPH_BB_NUM);
        rpo_BBs.reserve(GRAPH_BB_NUM);
        graph_size = 0;
        pm = std::make_unique<PassManager>(this);
        mm = std::make_unique<MarkerManager>();
    }
    ~Graph();

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

    Loop* getRootLoop() const noexcept
    {
    	return root_loop;
    }

    void setRootLoop(Loop* loop_) noexcept
    {
    	root_loop = loop_;
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

    void insertBB(BasicBlock *bb);
    void insertBBAfter(BasicBlock *prev_bb, BasicBlock *bb, bool is_true_succ = true);
    void addEdge(BasicBlock *prev_bb, BasicBlock *bb);
    void replaceBB(BasicBlock *bb, BasicBlock *new_bb);
    void replaceBB(size_t num, BasicBlock *new_bb);

    marker_t getNewMarker();
    void deleteMarker(marker_t marker);

    void dump(std::ostream &out = std::cout)
    {
        std::cout << "Graph for proc " << func_name << std::endl;
        std::for_each(BBs.begin(), BBs.end(), [&out](auto bb) { bb->dump(out); });
    }

    void dumpDomTree(std::ostream &out = std::cout) const
    {
        // TODO after implementing fast DomTree
    }

    // template <typename PassName, typename... Args>
    // TODO: fix PassManager
    // [some template magic was broken and there is a crutch]
    bool runPassRpo(marker_t marker = 0);
    bool runPassDomTree();
    bool runPassLoopAnalyzer();

  private:
    std::string func_name = "";
    size_t graph_size = 0;

    std::vector<BasicBlock *> BBs;
    std::vector<BasicBlock *> rpo_BBs;

    std::unique_ptr<PassManager> pm = nullptr;
    std::unique_ptr<MarkerManager> mm = nullptr;
    Loop *root_loop = nullptr;
};

/*
template <typename PassName>
void Graph::runPass()
{
    pm->template runPass<PassName>();
}
*/

} // namespace compiler