#pragma once

#include "basicblock.h"
#include "inst.h"
#include "pass/passmanager.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace compiler
{

#define GRAPH_BB_NUM 50

class BasicBlock;
class PassManager;
class Rpo;
class DomTree;
class Loop;
class LoopAnalysis;
class LinearOrder;
class LivenessAnalysis;
class LiveInterval;

class Graph
{
  public:
    Graph(std::string name = "") : func_name(name), graph_size(0)
    {
        BBs.reserve(GRAPH_BB_NUM);
        rpo_BBs.reserve(GRAPH_BB_NUM);
        linear_order_BBs.reserve(GRAPH_BB_NUM);
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

    void setName(std::string& name) noexcept
    {
        func_name = name;
    }

    std::vector<BasicBlock*>& getBBs() noexcept
    {
        return BBs;
    }

    std::vector<BasicBlock*>& getRpoBBs() noexcept
    {
        return rpo_BBs;
    }

    std::vector<BasicBlock*>& getLinearOrderBBs() noexcept
    {
        return linear_order_BBs;
    }

    std::unordered_map<Inst*, LiveInterval*>& getLiveIntervals() noexcept
    {
        return live_intervals;
    }

    void setRPOBBs(std::vector<BasicBlock*>& rpo) noexcept
    {
        rpo_BBs = rpo;
    }

    void setLinearOrderBBs(std::vector<BasicBlock*>& linear) noexcept
    {
        linear_order_BBs = linear;
    }

    void setLiveIntervals(std::unordered_map<Inst*, LiveInterval*>& live) noexcept
    {
        live_intervals = live;
    }

    Loop* getRootLoop() const noexcept
    {
        return root_loop;
    }

    void setRootLoop(Loop* loop_) noexcept
    {
        root_loop = loop_;
    }

    BasicBlock* getFirstBB() const noexcept
    {
        if (graph_size == 0)
            return nullptr;
        return BBs[0];
    }

    BasicBlock* getLastBB() const noexcept
    {
        if (graph_size == 0)
            return nullptr;
        return BBs[graph_size - 1];
    }

    BasicBlock* getBB(size_t id) const noexcept
    {
        if (id >= graph_size)
            return nullptr;
        else
            return BBs[id];
    }

    void removeBB(BasicBlock* bb)
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

    void insertBB(BasicBlock* bb);
    void insertBBAfter(BasicBlock* prev_bb, BasicBlock* bb, bool is_true_succ = true);
    void addEdge(BasicBlock* prev_bb, BasicBlock* bb);
    void replaceBB(BasicBlock* bb, BasicBlock* new_bb);
    void replaceBB(size_t num, BasicBlock* new_bb);

    marker_t getNewMarker();
    void deleteMarker(marker_t marker);

    void dump(std::ostream& out = std::cout)
    {
        std::cout << "Graph for proc " << func_name << std::endl;
        std::for_each(BBs.begin(), BBs.end(), [&out](auto bb) { bb->dump(out); });
    }

    // template <typename PassName, typename... Args>
    // TODO: fix PassManager
    // [some template magic was broken and there is a crutch]
    bool runPassRpo(marker_t marker = 0);
    bool runPassDomTree();
    bool runPassLoopAnalysis();
    bool runPassLinearOrder();
    bool runPassLivenessAnalysis();

  private:
    std::string func_name = "";
    size_t graph_size = 0;

    std::vector<BasicBlock*> BBs;
    std::vector<BasicBlock*> rpo_BBs;
    std::vector<BasicBlock*> linear_order_BBs;

    std::unique_ptr<PassManager> pm = nullptr;
    std::unique_ptr<MarkerManager> mm = nullptr;
    Loop* root_loop = nullptr;

    std::unordered_map<Inst*, LiveInterval*> live_intervals;
};

/*
template <typename PassName>
void Graph::runPass()
{
    pm->template runPass<PassName>();
}
*/

} // namespace compiler
