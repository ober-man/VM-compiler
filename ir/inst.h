#pragma once
#include "const.h"

#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace compiler
{

class BasicBlock;
class Graph;

class Inst
{
  public:
    Inst(InstType inst_type_, size_t id_) : inst_type(inst_type_), id(id_)
    {}

    Inst(InstType inst_type_, size_t id_, std::shared_ptr<BasicBlock> bb_)
        : inst_type(inst_type_), id(id_), bb(bb_)
    {}

    virtual ~Inst() = default;

    InstType getInstType() const
    {
        return inst_type;
    }

    size_t getId() const
    {
        return id;
    }
    void setId(size_t id_)
    {
        id = id_;
    }

    auto getBB() const
    {
        return bb;
    }
    void setBB(std::shared_ptr<BasicBlock> bb_)
    {
        bb = bb_;
    }

    std::shared_ptr<Inst> getNext() const
    {
        return next;
    }
    void setNext(std::shared_ptr<Inst> next_)
    {
        next = next_;
    }
    std::shared_ptr<Inst> getPrev() const
    {
        return prev;
    }
    void setPrev(std::shared_ptr<Inst> prev_)
    {
        prev = prev_;
    }

    virtual void dump(std::ostream &out = std::cout) = 0;

  protected:
    InstType inst_type = InstType::NONE;
    size_t id = 0;

    std::shared_ptr<BasicBlock> bb = nullptr;
    std::shared_ptr<Inst> prev = nullptr;
    std::shared_ptr<Inst> next = nullptr;
};

template <size_t N = 0>
class FixedInputsInst : public Inst
{
  public:
    using Inst::Inst;
    virtual ~FixedInputsInst() = default;

    std::shared_ptr<Inst> getInput(size_t num) const
    {
        static_assert(num < N && "too big input number");
        return inputs[num];
    }

    void setInput(std::shared_ptr<Inst> input, size_t num)
    {
        static_assert(num < N && "too big input number");
        inputs[num] = input;
    }

    void replaceInput(std::shared_ptr<Inst> old_input,
                      std::shared_ptr<Inst> new_input)
    {
        std::replace(inputs.begin(), inputs.end(), old_input, new_input);
    }

    void replaceInput(size_t num, std::shared_ptr<Inst> new_input)
    {
        inputs[num] = new_input;
    }

  protected:
    std::array<std::shared_ptr<Inst>, N> inputs;
};

class BinaryInst final : public FixedInputsInst<2>
{
  public:
    BinaryInst(uint32_t id_, BinOpType op_ = BinOpType::NONE,
               std::shared_ptr<Inst> left = nullptr,
               std::shared_ptr<Inst> right = nullptr)
        : FixedInputsInst(InstType::BINARY, id_), op(op_)
    {
        inputs[0] = left;
        inputs[1] = right;
    }

    ~BinaryInst() = default;

    void dump(std::ostream &out = std::cout) override
    {
        out << "\t" << id << " " << getBinOpTypeString() << " "
            << inputs[0]->getId() << " " << inputs[1]->getId() << std::endl;
    }

  private:
    std::string getBinOpTypeString();

  private:
    BinOpType op = BinOpType::NONE;
};

class UnaryInst final : public FixedInputsInst<1>
{
  public:
    UnaryInst(uint32_t id_, UnOpType op_ = UnOpType::NONE,
              std::shared_ptr<Inst> input = nullptr)
        : FixedInputsInst(InstType::UNARY, id_), op(op_)
    {
        inputs[0] = input;
    }

    ~UnaryInst() = default;

    void dump(std::ostream &out = std::cout) override
    {
        out << "\t" << id << " " << getUnOpTypeString() << " "
            << inputs[0]->getId() << std::endl;
    }

  private:
    std::string getUnOpTypeString();

  private:
    UnOpType op = UnOpType::NONE;
};

template <typename T>
constexpr DataType getDataType();

class ConstInst final : public Inst
{
  public:
    ConstInst(uint32_t id_, DataType type_ = DataType::NONE)
        : Inst(InstType::CONST, id_), type(type_)
    {}

    template <typename T>
    ConstInst(uint32_t id_, T value_, DataType type_ = DataType::NONE)
        : Inst(InstType::CONST, id), type(type_)
    {
        static_assert(getDataType<T>() != DataType::NONE);

        if constexpr (getDataType<T>() == DataType::INT32)
            value = static_cast<uint64_t>(value_);
        else if constexpr (getDataType<T>() == DataType::INT64)
            value = value_;
        else if constexpr (getDataType<T>() == DataType::FLOAT32)
            value = static_cast<uint64_t>(value_);
        else // FLOAT64
            value_ = static_cast<uint64_t>(value);

        if (type == DataType::NONE)
            type = getDataType<T>();
    }

    ~ConstInst() = default;

    uint32_t getInt32Value() const
    {
        assert(type == DataType::INT32);
        return static_cast<uint32_t>(value);
    }

    uint64_t getInt64Value() const
    {
        assert(type == DataType::INT64);
        return value;
    }

    uint64_t getIntValue() const
    {
        assert(type == DataType::INT32 || type == DataType::INT64);
        return value;
    }

    float getFloatValue() const
    {
        assert(type == DataType::FLOAT32);
        return static_cast<float>(value);
    }

    double getDoubleValue() const
    {
        assert(type == DataType::FLOAT64);
        return static_cast<double>(value);
    }

    DataType getType() const
    {
        return type;
    }
    void setType(DataType type_)
    {
        type = type_;
    }

    void dump(std::ostream &out = std::cout) override
    {
        out << "\t" << id << " "
            << "const " << value << std::endl;
    }

  private:
    DataType type = DataType::NONE;
    uint64_t value;
};

class ParamInst final : public Inst
{
  public:
    ParamInst(uint32_t id_, size_t arg_) : Inst(InstType::PARAM, id_), arg(arg_)
    {}

    ~ParamInst() = default;

    size_t getArg() const
    {
        return arg;
    }
    void setArg(size_t arg_)
    {
        arg = arg_;
    }

    void dump(std::ostream &out = std::cout) override
    {
        out << "\t" << id << " "
            << "param " << arg << std::endl;
    }

  private:
    size_t arg = 0;
};

class JumpInst final : public Inst
{
  public:
    JumpInst(uint32_t id_, JumpOpType op_ = JumpOpType::NONE,
             std::shared_ptr<BasicBlock> target_ = nullptr)
        : Inst(InstType::JUMP, id_), op(op_), target(target_)
    {}

    ~JumpInst() = default;

    JumpOpType getArg() const
    {
        return op;
    }
    void setArg(JumpOpType op_)
    {
        op = op_;
    }

    std::shared_ptr<BasicBlock> getBB() const
    {
        return target;
    }
    void setBB(std::shared_ptr<BasicBlock> target_)
    {
        target = target_;
    }

    void dump(std::ostream &out = std::cout) override;

  private:
    std::string getJumpOpTypeString();

  private:
    JumpOpType op = JumpOpType::NONE;
    std::shared_ptr<BasicBlock> target = nullptr;
};

class CallInst final : public Inst
{
  public:
    CallInst(uint32_t id_) : Inst(InstType::CALL, id_)
    {}

    CallInst(std::initializer_list<std::shared_ptr<Inst>> args_)
        : Inst(InstType::CALL, 0)
    {
        args.insert(args.end(), args_.begin(), args_.end());
    }

    CallInst(std::initializer_list<size_t> args_);

    ~CallInst() = default;

    std::vector<std::shared_ptr<Inst>> &getArgs()
    {
        return args;
    }

    void setArg(std::shared_ptr<Inst> arg, int num)
    {
        assert(num < args.size());
        args[num] = arg;
    }

    void insertArg(std::shared_ptr<Inst> arg)
    {
        args.push_back(arg);
    }

    void replaceArg(std::shared_ptr<Inst> old_arg,
                    std::shared_ptr<Inst> new_arg)
    {
        std::replace(args.begin(), args.end(), old_arg, new_arg);
    }

    void replaceArg(size_t num, std::shared_ptr<Inst> new_arg)
    {
        args[num] = new_arg;
    }

    void dump(std::ostream &out = std::cout) override;

  private:
    // TODO make a function class
    std::shared_ptr<Graph> func;
    std::vector<std::shared_ptr<Inst>> args;
};

std::string getDataTypeString(DataType type);

class CastInst final : public FixedInputsInst<1>
{
  public:
    CastInst(uint32_t id_, std::shared_ptr<Inst> input)
        : FixedInputsInst(InstType::CAST, id)
    {
        inputs[0] = input;
    }

    CastInst(uint32_t id_, std::shared_ptr<Inst> input, DataType from_,
             DataType to_)
        : FixedInputsInst(InstType::CAST, id), from(from_), to(to_)
    {
        inputs[0] = input;
    }

    ~CastInst() = default;

    DataType getFromType() const
    {
        return from;
    }
    void setFromType(DataType from_)
    {
        from = from_;
    }
    DataType getToType() const
    {
        return to;
    }
    void setToType(DataType to_)
    {
        to = to_;
    }

    void dump(std::ostream &out = std::cout) override
    {
        out << "\t" << id << " "
            << "cast " << inputs[0] << "(" << getDataTypeString(from) << "->"
            << getDataTypeString(to) << ")" << std::endl;
    }

  private:
    DataType from = DataType::NONE;
    DataType to = DataType::NONE;
};

class PhiInst : public Inst
{
  public:
    PhiInst(uint32_t id_) : Inst(InstType::PHI, id_)
    {}

    PhiInst(std::initializer_list<
            std::pair<std::shared_ptr<Inst>, std::shared_ptr<BasicBlock>>>
                inputs_)
        : Inst(InstType::PHI, 0)
    {
        for (auto input : inputs_)
            inputs.push_back(input);
    }

    ~PhiInst() = default;

    auto &getInputs() const
    {
        return inputs;
    }

    void addInput(std::shared_ptr<Inst> inst, std::shared_ptr<BasicBlock> bb)
    {
        inputs.push_back(std::make_pair(inst, bb));
    }

    void replaceBB(size_t num, std::shared_ptr<BasicBlock> new_bb)
    {
        inputs[num].second = new_bb;
    }

    void replaceArg(size_t num, std::shared_ptr<Inst> new_arg)
    {
        inputs[num].first = new_arg;
    }

    void dump(std::ostream &out = std::cout) override;

  private:
    std::vector<std::pair<std::shared_ptr<Inst>, std::shared_ptr<BasicBlock>>>
        inputs;
};

} // namespace compiler