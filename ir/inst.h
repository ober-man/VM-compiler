#pragma once
#include "const.h"

#include <cassert>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <vector>

namespace compiler
{

class BasicBlock;
class Graph;

class Inst
{
  public:
    explicit Inst(size_t id_, InstType inst_type_ = InstType::NoneInst, BasicBlock *bb_ = nullptr)
        : inst_type(inst_type_), id(id_), bb(bb_)
    {}

    virtual ~Inst() = default;

    InstType getInstType() const
    {
        return inst_type;
    }

    size_t getId() const noexcept
    {
        return id;
    }

    void setId(size_t id_) noexcept
    {
        id = id_;
    }

    auto getBB() const noexcept
    {
        return bb;
    }

    void setBB(BasicBlock *bb_) noexcept
    {
        bb = bb_;
    }

    std::shared_ptr<Inst> getNext() const noexcept
    {
        return next;
    }

    void setNext(std::shared_ptr<Inst> next_) noexcept
    {
        next = next_;
    }

    std::shared_ptr<Inst> getPrev() const noexcept
    {
        return prev;
    }

    void setPrev(std::shared_ptr<Inst> prev_) noexcept
    {
        prev = prev_;
    }

    virtual DataType getType() const noexcept
    {
        return DataType::NoType;
    }

    virtual void dump(std::ostream &out = std::cout) const = 0;

  protected:
    InstType inst_type = InstType::NoneInst;
    size_t id = 0;

    BasicBlock *bb = nullptr;
    std::shared_ptr<Inst> prev = nullptr;
    std::shared_ptr<Inst> next = nullptr;
};

std::string getDataTypeString(DataType type);

template <size_t N = 0>
class FixedInputsInst : public Inst
{
  public:
    using Inst::Inst;
    virtual ~FixedInputsInst() = default;

    std::shared_ptr<Inst> getInput(size_t num) const
    {
        assert(num < N && "too big input number");
        return inputs[num];
    }

    void setInput(std::shared_ptr<Inst> input, size_t num)
    {
        assert(num < N && "too big input number");
        inputs[num] = input;
    }

    void replaceInput(std::shared_ptr<Inst> old_input, std::shared_ptr<Inst> new_input)
    {
        std::replace(inputs.begin(), inputs.end(), old_input, new_input);
    }

    void replaceInput(size_t num, std::shared_ptr<Inst> new_input)
    {
        assert(num < N && "too big input number");
        inputs[num] = new_input;
    }

  protected:
    std::array<std::shared_ptr<Inst>, N> inputs;
};

class BinaryInst final : public FixedInputsInst<2>
{
  public:
    explicit BinaryInst(size_t id_, BinOpType op_ = BinOpType::NoneBinOp,
                        std::shared_ptr<Inst> left = nullptr, std::shared_ptr<Inst> right = nullptr)
        : FixedInputsInst(id_, InstType::Binary), op(op_)
    {
        inputs[0] = left;
        inputs[1] = right;
    }

    ~BinaryInst() = default;

    BinOpType getBinOpType() const noexcept
    {
        return op;
    }

    void setBinOpType(BinOpType op_) noexcept
    {
        op = op_;
    }

    DataType getType() const noexcept override
    {
        DataType data_type = inputs[0]->getType();
        if (data_type == DataType::NoType)
            return inputs[1]->getType();
        return data_type;
    }

    void dump(std::ostream &out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(op)] << " "
            << TYPE_NAME[static_cast<uint8_t>(getType())] << " v" << inputs[0]->getId() << ", v"
            << inputs[1]->getId() << std::endl;
    }

  private:
    std::string getBinOpTypeString() const noexcept;

  private:
    BinOpType op = BinOpType::NoneBinOp;
};

class UnaryInst final : public FixedInputsInst<1>
{
  public:
    explicit UnaryInst(size_t id_, UnOpType op_ = UnOpType::NoneUnOp,
                       std::shared_ptr<Inst> input = nullptr)
        : FixedInputsInst(id_, InstType::Unary), op(op_)
    {
        inputs[0] = input;
    }

    ~UnaryInst() = default;

    UnOpType getUnOpType() const noexcept
    {
        return op;
    }

    void setUnOpType(UnOpType op_) noexcept
    {
        op = op_;
    }

    DataType getType() const noexcept override
    {
        return inputs[0]->getType();
    }

    void dump(std::ostream &out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(op)] << " "
            << TYPE_NAME[static_cast<uint8_t>(getType())] << " v" << inputs[0]->getId()
            << std::endl;
    }

  private:
    std::string getUnOpTypeString() const noexcept;

  private:
    UnOpType op = UnOpType::NoneUnOp;
};

class ConstInst final : public Inst
{
  public:
    explicit ConstInst(size_t id_, DataType data_type_ = DataType::NoType)
        : Inst(id_, InstType::Const), data_type(data_type_)
    {}

    template <typename T>
    ConstInst(size_t id_, T value_) : Inst(id_, InstType::Const)
    {
        static_assert(getDataType<T>() != DataType::NoType);

        if constexpr (getDataType<T>() == DataType::i32)
            value = static_cast<uint64_t>(value_);
        else if constexpr (getDataType<T>() == DataType::i64)
            value = value_;
        else if constexpr (getDataType<T>() == DataType::f32)
            value = static_cast<uint64_t>(value_);
        else // f64
            value = static_cast<uint64_t>(value_);

        data_type = getDataType<T>();
    }

    ~ConstInst() = default;

    template <typename T>
    static constexpr DataType getDataType()
    {
        if constexpr (std::is_integral_v<T>)
        {
            if (sizeof(T) == sizeof(uint32_t))
                return DataType::i32;
            else
                return DataType::i64;
        }
        else if constexpr (std::is_same_v<T, float>)
            return DataType::f32;
        else if constexpr (std::is_same_v<T, double>)
            return DataType::f64;
        return DataType::NoType;
    }

    uint32_t getInt32Value() const
    {
        assert(data_type == DataType::i32);
        return static_cast<uint32_t>(value);
    }

    uint64_t getInt64Value() const
    {
        assert(data_type == DataType::i64);
        return value;
    }

    uint64_t getIntValue() const
    {
        assert(data_type == DataType::i32 || data_type == DataType::i64);
        return value;
    }

    float getFloatValue() const
    {
        assert(data_type == DataType::f32);
        return static_cast<float>(value);
    }

    double getDoubleValue() const
    {
        assert(data_type == DataType::f64);
        return static_cast<double>(value);
    }

    DataType getType() const noexcept override
    {
        return data_type;
    }

    void setType(DataType data_type_) noexcept
    {
        data_type = data_type_;
    }

    void dump(std::ostream &out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " "
            << TYPE_NAME[static_cast<uint8_t>(data_type)] << " " << value << std::endl;
    }

  private:
    DataType data_type = DataType::NoType;
    uint64_t value;
};

class ParamInst final : public Inst
{
  public:
    explicit ParamInst(size_t id_, DataType data_type_ = DataType::NoType, std::string name_ = "")
        : Inst(id_, InstType::Param), data_type(data_type_), name(name_)
    {}

    ~ParamInst() = default;

    void setType(DataType data_type_) noexcept
    {
        data_type = data_type_;
    }

    std::string getParamName() const noexcept
    {
        return name;
    }

    void setParamName(std::string name_) noexcept
    {
        name = name_;
    }

    DataType getType() const noexcept override
    {
        return data_type;
    }

    void dump(std::ostream &out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " "
            << TYPE_NAME[static_cast<uint8_t>(data_type)] << " " << name << std::endl;
    }

  private:
    DataType data_type = DataType::NoType;
    std::string name = "";
};

class JumpInst final : public Inst
{
  public:
    explicit JumpInst(size_t id_, JumpOpType op_ = JumpOpType::NoneJumpOp,
                      BasicBlock *target_ = nullptr)
        : Inst(id_, InstType::Jump), op(op_), target(target_)
    {}

    ~JumpInst() = default;

    JumpOpType getJumpOpType() const noexcept
    {
        return op;
    }

    void setJumpOpType(JumpOpType op_) noexcept
    {
        op = op_;
    }

    BasicBlock *getTargetBB() const noexcept
    {
        return target;
    }

    void setTargetBB(BasicBlock *target_) noexcept
    {
        target = target_;
    }

    void dump(std::ostream &out = std::cout) const override;

  private:
    JumpOpType op = JumpOpType::NoneJumpOp;
    BasicBlock *target = nullptr;
};

class CallInst final : public Inst
{
  public:
    explicit CallInst(size_t id_) : Inst(id_, InstType::Call)
    {}

    explicit CallInst(std::initializer_list<std::shared_ptr<Inst>> args_) : Inst(0, InstType::Call)
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

    void replaceArg(std::shared_ptr<Inst> old_arg, std::shared_ptr<Inst> new_arg)
    {
        std::replace(args.begin(), args.end(), old_arg, new_arg);
    }

    void replaceArg(size_t num, std::shared_ptr<Inst> new_arg)
    {
        args[num] = new_arg;
    }

    void dump(std::ostream &out = std::cout) const override;

  private:
    std::shared_ptr<Graph> func;
    std::vector<std::shared_ptr<Inst>> args;
};

class CastInst final : public FixedInputsInst<1>
{
  public:
    explicit CastInst(size_t id_, std::shared_ptr<Inst> input = nullptr,
                      DataType to_ = DataType::NoType)
        : FixedInputsInst(id_, InstType::Cast), to(to_)
    {
        inputs[0] = input;
    }

    ~CastInst() = default;

    DataType getFromType() const noexcept
    {
        return inputs[0]->getType();
    }

    DataType getToType() const noexcept
    {
        return to;
    }

    void setToType(DataType to_) noexcept
    {
        to = to_;
    }

    DataType getType() const noexcept override
    {
        return to;
    }

    void dump(std::ostream &out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " v"
            << inputs[0]->getId() << " to " << TYPE_NAME[static_cast<uint8_t>(to)] << std::endl;
    }

  private:
    DataType to = DataType::NoType;
};

class MovInst final : public FixedInputsInst<1>
{
  public:
    explicit MovInst(size_t id_, size_t reg = 0, std::shared_ptr<Inst> input = nullptr)
        : FixedInputsInst(id_, InstType::Mov), reg_num(reg)
    {
        inputs[0] = input;
    }

    ~MovInst() = default;

    size_t getRegNum() const noexcept
    {
        return reg_num;
    }

    void setRegNum(size_t reg) noexcept
    {
        reg = reg_num;
    }

    DataType getType() const noexcept override
    {
        return inputs[0]->getType();
    }

    void dump(std::ostream &out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " "
            << TYPE_NAME[static_cast<uint8_t>(getType())] << " r" << reg_num << ", v"
            << inputs[0]->getId() << std::endl;
    }

  private:
    size_t reg_num = 0;
};

class PhiInst : public Inst
{
  public:
    using phi_pair_t = std::pair<std::shared_ptr<Inst>, BasicBlock *>;

    explicit PhiInst(size_t id_) : Inst(id_, InstType::Phi)
    {}

    explicit PhiInst(std::initializer_list<phi_pair_t> inputs_) : Inst(0, InstType::Phi)
    {
        for (auto input : inputs_)
            inputs.push_back(input);
    }

    ~PhiInst() = default;

    auto &getInputs() const noexcept
    {
        return inputs;
    }

    void addInput(std::shared_ptr<Inst> inst, BasicBlock *bb)
    {
        inputs.push_back(std::make_pair(inst, bb));
    }

    void addInput(phi_pair_t pair)
    {
        inputs.push_back(pair);
    }

    void replaceBB(size_t num, BasicBlock *new_bb)
    {
        assert(num < inputs.size() && "too big input number");
        inputs[num].second = new_bb;
    }

    void replaceArg(size_t num, std::shared_ptr<Inst> new_arg)
    {
        assert(num < inputs.size() && "too big input number");
        inputs[num].first = new_arg;
    }

    DataType getType() const noexcept override
    {
        for (auto &&input : inputs)
        {
            DataType type = input.first->getType();
            if (type != DataType::NoType)
                return type;
        }
        return DataType::NoType;
    }

    void dump(std::ostream &out = std::cout) const override;

  private:
    std::vector<phi_pair_t> inputs;
};

} // namespace compiler