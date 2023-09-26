#pragma once

#include "inst.h"

namespace compiler
{

class Graph;

class BasicBlock
{
	public:
		BasicBlock(size_t id_, std::shared_ptr<Graph> graph_ = nullptr,
				   std::string& name_ = "") 
			: id(id_), graph(graph_), name(name_)
		{
			preds.reserve(5);
			succs.reserve(2);
			size = 0;
		}

		~BasicBlock() = default;

		size_t getId() const { return id; }
		void incrId()        { ++id; }
		void setId(size_t id_) 
		{ 
			assert(graph && !graph->getBB(id_) && "BB id already existed");
			id = id_; 
		}

		std::string getName() const     { return name; }
		void setName(std::string name_) { name = name_; }

		size_t size() const  { return size; }
		bool isEmpty()       { return size == 0; }

		std::shared_ptr<Graph> getGraph() const      { return graph; }
		void setGraph(std::shared_ptr<Graph> graph_) { graph = graph_; }

		std::vector<std::shared_ptr<BasicBlock>>& getPreds() const { return preds; }
		std::vector<std::shared_ptr<BasicBlock>>& getSuccs() const { return succs; }

		std::shared_ptr<Inst> getFirstInst() const { return first_inst; }
		std::shared_ptr<Inst> getLastInst () const { return last_inst; }
		std::shared_ptr<Inst> getInst(size_t id)
		{
			for(auto inst = first_inst; inst != nullptr; inst = inst->getNext())
				if(inst->getId() == id)
					return inst;
			return nullptr;
		}

		void pushBackInst(std::shared_ptr<Inst> inst)
		{
			assert(!inst->getPrev() && "inserted inst has predecessor");

			inst->setPrev(last_inst);
			inst->setNext(nullptr);
			inst->setBB(std::make_shared<BasicBlock>(*this));

			if(last_inst)
				last_inst->setNext(inst);
			last_inst = inst;
			++size;
		}

		void pushFrontInst(std::shared_ptr<Inst> inst)
		{
			assert(!inst->getNext() && "inserted inst has successor");
			inst->setPrev(nullptr);
			inst->setNext(first_inst);
			inst->setBB(std::make_shared<BasicBlock>(*this));

			if(first_inst)
				first_inst->setPrev(inst);
			++size;
		}

		/**
		 * Insert inst after prev_inst
		 */
		void insertInst(std::shared_ptr<Inst> prev_inst, std::shared_ptr<Inst> inst)
		{	
			assert(!inst->getPrev() && "inserted inst has predecessor");
			assert(!inst->getNext() && "inserted inst has successor");

			if(!prev_inst->getNext())
			{
				pushBackInst(inst);
				return;
			}

			auto next_inst = prev_inst->getNext();
			next_inst->setPrev(inst);
			prev_inst->setNext(inst);

			inst->setNext(next_inst);
			inst->setPrev(prev_inst);
			inst->setBB(std:shared_ptr<Inst>(*this));
			++size;
		}

		void insertInst(std::shared_ptr<Inst> inst)
		{
			assert(!inst->getPrev() && "inserted inst has predecessor");
			assert(!inst->getNext() && "inserted inst has successor");

			if (!first_inst)
			{
				first_inst = inst;
				last_inst = inst;
				inst->setId(0);
				inst->setBB(std::make_shared<BasicBlock>(*this));
				++size;
			}
			else
			{
				pushBackInst(inst);
			}
		}

		void popFrontInst(std::shared_ptr<Inst> inst)
		{
			assert(first_inst && "first inst not existed");
			auto second_inst = first_inst->getNext();
			second_inst->setPrev(nullptr);
			first_inst->setNext(nullptr);
			first_inst = second_inst;
			--size;
		}

		void popBackInst(std::shared_ptr<Inst> inst)
		{
			assert(last_inst && "last inst not existed");
			auto prev_inst = last_inst->getPrev();
			prev_inst->setNext(nullptr);
			last_inst->setPrev(nullptr);
			last_inst = prev_inst;
			--size;
		}

		void removeInst(std::shared_ptr<Inst> inst)
		{
			auto next_inst = inst->getNext();
			auto prev_inst = inst->getPrev();
			if (next_inst)
				next_inst->setPrev(prev_inst);
			if (prev_inst)
				prev_inst->setNext(next_inst);
			--size;		
		}

		void addPred(std::shared_ptr<BasicBlock> bb) 
		{ 
    		assert(std::find(preds.begin(), preds.end(), bb) == preds.end() && 
    			   "pred already existed");
			preds.push_back(bb);
		}

		void addSucc(std::shared_ptr<BasicBlock> bb) 
		{ 
			if (!succ_true)
				succ_true = bb;
			else if (!succ_false)
				succ_false = bb;
			else 
			{
				std::cerr << "cannot add succ" << std::endl;
				abort();
			}
		}

		void removePred(std::shared_ptr<BasicBlock> bb) 
		{
			preds.erase(std::find(preds.begin(), preds.end(), bb)); 
		}

		void removePred(size_t num) 
		{ 
			preds.erase(std::find_if(preds.begin(), preds.end(), 
						             [](auto pred) { pred->getId() == num; }));
		}

		void removeSucc(std::shared_ptr<BasicBlock> bb) 
		{ 
			if (succ_true == bb)
				succ_true = nullptr;;
			else if (succ_false == bb)
				succ_false = nullptr;
			else return;
		}

		void removeSucc(size_t num) 
		{ 
			if (succ_true->getId() == num)
				succ_true = nullptr;;
			else if (succ_false->getId() == num)
				succ_false = nullptr;
			else return;
		}

		void replacePred(std::shared_ptr<BasicBlock> pred, 
						 std::shared_ptr<BasicBlock> bb) 
		{ 
			auto it = std::find(preds.begin(), preds.end(), pred);
			assert(it != preds.end() && "replace not existing pred");
			preds[it->getId()] = bb;
		}

		void replacePred(size_t num, std::shared_ptr<BasicBlock> bb) 
		{ 
			auto it = std::find_if(preds.begin(), preds.end(), 
								   [](auto pred) { pred->getId() == num; });
			assert(it != preds.end() && "replace not existing pred");
			preds[num] = bb;
		}

		void replaceSucc(std::shared_ptr<BasicBlock> succ, 
						 std::shared_ptr<BasicBlock> bb) 
		{ 
			if (succ_true == succ)
				succ_true = bb;;
			else if (succ_false == succ)
				succ_false = bb;
			else
			{
				std::cerr << "replace not existing succ" << std::endl;
				abort();
			}
		}

		void replaceSucc(size_t num, std::shared_ptr<BasicBlock> bb) 
		{ 
			if (succ_true->getId() == num)
				succ_true = bb;;
			else if (succ_false->getId() == num)
				succ_false = bb;
			else
			{
				std::cerr << "replace not existing succ" << std::endl;
				abort();
			}
		}

		void dump(std::ostream& out = std::cout)
		{
			out << "BB " << name << "[" << id  << "/" << graph->size() 
								 << "]" << std::endl;
			out << "preds : ";
			std::for_each(preds.begin(), preds.end(), 
						  [](auto pred) { out << pred.getId() << " "; });
			out << "\n";

			out << "succs : ";
			if (succ_true)
				out << "true " << succ_true->getId();
			if (succ_false)
				out << "false" << succ_false->getId();
			out << "\n";

			for (auto inst = first_inst, last = last_inst; inst != last; inst->getNextInst())
				dump(inst);
		}

	private:
		size_t id = 0;
		size_t size = 0;
		std::string name = "";
		std::shared_ptr<Graph> graph = nullptr;

		std::vector<std::shared_ptr<BasicBlock>> preds;
		std::shared_ptr<BasicBlock> succ_true;
		std::shared_ptr<BasicBlock> succ_false;

		std::shared_ptr<Inst> first_inst = nullptr;
		std::shared_ptr<Inst> last_inst = nullptr;
};

} // namespace compiler