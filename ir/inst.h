#pragma once
#include "const.h"
#include "utils.h"

#include <initializer_list>
#include <iostream>
#include <list>
#include <memory>
#include <vector>

namespace compiler
{

constexpr size_t INST_USERS_NUM = 4;

class BasicBlock;
class Graph;

class Inst
{
  public:
    explicit Inst(size_t id_, InstType inst_type_ = InstType::NoneInst, BasicBlock* bb_ = nullptr)
        : inst_type(inst_type_), id(id_), bb(bb_)
    {}
    virtual ~Inst() = default;

    DEFINE_GETTER_SETTER(id, Id, size_t)
    DEFINE_GETTER(inst_type, InstType, InstType)
    DEFINE_GETTER_SETTER(linear_num, LinearNum, size_t)
    DEFINE_GETTER_SETTER(live_num, LiveNum, size_t)
    DEFINE_GETTER_SETTER(bb, BB, BasicBlock*)
    DEFINE_GETTER_SETTER(next, Next, Inst*)
    DEFINE_GETTER_SETTER(prev, Prev, Inst*)
    DEFINE_ARRAY_GETTER(users, Users, std::list<Inst*>&)

    size_t getUsersNum()
    {
        return users.size();
    }

    void addUser(Inst* user)
    {
        auto it = std::find_if(users.begin(), users.end(),
                               [user](auto* u) { return u->getId() > user->getId(); });
        if (it != users.end())
            users.insert(it, user);
        else
            users.push_back(user);
    }

    void removeUser(Inst* user)
    {
        users.erase(std::find(users.begin(), users.end(), user));
    }

    void removeUser(size_t num)
    {
        users.erase(
            std::find_if(users.begin(), users.end(), [num](auto u) { return u->getId() == num; }));
    }

    virtual DataType getType() const noexcept
    {
        return DataType::NoType;
    }

    virtual void dump(std::ostream& out = std::cout) const = 0;
    void dumpUsers(std::ostream& out = std::cout) const;

  protected:
    InstType inst_type = InstType::NoneInst;
    size_t id = 0;

    size_t linear_num = 0;
    size_t live_num = 0;

    BasicBlock* bb = nullptr;
    Inst* prev = nullptr;
    Inst* next = nullptr;

    std::list<Inst*> users;
};

std::string getDataTypeString(DataType type);

template <size_t N = 0>
class FixedInputsInst : public Inst
{
  public:
    using Inst::Inst;
    virtual ~FixedInputsInst() = default;

    Inst* getInput(size_t num) const
    {
        ASSERT(num < N, "too big input number");
        return inputs[num];
    }

    void setInput(Inst* input, size_t num)
    {
        ASSERT(num < N, "too big input number");
        inputs[num] = input;
        input->addUser(this);
    }

    void replaceInput(Inst* old_input, Inst* new_input)
    {
        std::replace(inputs.begin(), inputs.end(), old_input, new_input);
        old_input->removeUser(this);
        new_input->addUser(this);
    }

    void replaceInput(size_t num, Inst* new_input)
    {
        ASSERT(num < N, "too big input number");
        auto* old_input = inputs[num];
        inputs[num] = new_input;
        old_input->removeUser(this);
        new_input->addUser(this);
    }

  protected:
    std::array<Inst*, N> inputs;
};

class BinaryInst final : public FixedInputsInst<2>
{
  public:
    explicit BinaryInst(size_t id_, BinOpType op_ = BinOpType::NoneBinOp, Inst* left = nullptr,
                        Inst* right = nullptr)
        : FixedInputsInst(id_, InstType::Binary), op(op_)
    {
        inputs[0] = left;
        inputs[1] = right;
        left->addUser(this);
        right->addUser(this);
    }

    ~BinaryInst() = default;

    DEFINE_GETTER_SETTER(op, BinOpType, BinOpType)

    DataType getType() const noexcept override
    {
        DataType data_type = inputs[0]->getType();
        if (data_type == DataType::NoType)
            return inputs[1]->getType();
        return data_type;
    }

    void dump(std::ostream& out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(op)] << " "
            << TYPE_NAME[static_cast<uint8_t>(getType())] << " v" << inputs[0]->getId() << ", v"
            << inputs[1]->getId();
    }

  private:
    std::string getBinOpTypeString() const noexcept;

  private:
    BinOpType op = BinOpType::NoneBinOp;
};

class UnaryInst final : public FixedInputsInst<1>
{
  public:
    explicit UnaryInst(size_t id_, UnOpType op_ = UnOpType::NoneUnOp, Inst* input = nullptr)
        : FixedInputsInst(id_, InstType::Unary), op(op_)
    {
        inputs[0] = input;
        input->addUser(this);
    }

    ~UnaryInst() = default;

    DEFINE_GETTER_SETTER(op, UnOpType, UnOpType)

    DataType getType() const noexcept override
    {
        return inputs[0]->getType();
    }

    void dump(std::ostream& out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(op)] << " "
            << TYPE_NAME[static_cast<uint8_t>(getType())] << " v" << inputs[0]->getId();
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
        ASSERT(data_type == DataType::i32);
        return static_cast<uint32_t>(value);
    }

    uint64_t getInt64Value() const
    {
        ASSERT(data_type == DataType::i64);
        return value;
    }

    uint64_t getIntValue() const
    {
        ASSERT(data_type == DataType::i32 || data_type == DataType::i64);
        return value;
    }

    float getFloatValue() const
    {
        ASSERT(data_type == DataType::f32);
        return static_cast<float>(value);
    }

    double getDoubleValue() const
    {
        ASSERT(data_type == DataType::f64);
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

    void dump(std::ostream& out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " "
            << TYPE_NAME[static_cast<uint8_t>(data_type)] << " " << value;
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

    DEFINE_GETTER_SETTER(name, ParamName, std::string)

    DataType getType() const noexcept override
    {
        return data_type;
    }

    void dump(std::ostream& out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " "
            << TYPE_NAME[static_cast<uint8_t>(data_type)] << " " << name;
    }

  private:
    DataType data_type = DataType::NoType;
    std::string name = "";
};

class JumpInst final : public Inst
{
  public:
    explicit JumpInst(size_t id_, JumpOpType op_ = JumpOpType::NoneJumpOp,
                      BasicBlock* target_ = nullptr)
        : Inst(id_, InstType::Jump), op(op_), target(target_)
    {}

    ~JumpInst() = default;

    DEFINE_GETTER_SETTER(op, JumpOpType, JumpOpType)
    DEFINE_GETTER_SETTER(target, TargetBB, BasicBlock*)

    void dump(std::ostream& out = std::cout) const override;

  private:
    JumpOpType op = JumpOpType::NoneJumpOp;
    BasicBlock* target = nullptr;
};

class CallInst final : public Inst
{
  public:
    explicit CallInst(size_t id_, Graph* g) : Inst(id_, InstType::Call), func(g)
    {}

    explicit CallInst(std::initializer_list<Inst*> args_, size_t id_, Graph* g)
        : Inst(id_, InstType::Call), func(g)
    {
        args.insert(args.end(), args_.begin(), args_.end());
        std::for_each(args_.begin(), args_.end(), [this](auto* arg) { arg->addUser(this); });
    }

    CallInst(std::initializer_list<size_t> args_, size_t id_, Graph* g);

    ~CallInst() = default;

    DEFINE_ARRAY_GETTER(args, Args, std::vector<Inst*>&)
    DEFINE_GETTER_SETTER(func, Func, Graph*)

    void setArg(Inst* arg, size_t num)
    {
        ASSERT(num < args.size(), "too big arg number");
        args[num] = arg;
        arg->addUser(this);
    }

    void insertArg(Inst* arg)
    {
        args.push_back(arg);
        arg->addUser(this);
    }

    void replaceArg(Inst* old_arg, Inst* new_arg)
    {
        std::replace(args.begin(), args.end(), old_arg, new_arg);
        old_arg->removeUser(this);
        new_arg->addUser(this);
    }

    void replaceArg(size_t num, Inst* new_arg)
    {
        auto* old_arg = args[num];
        args[num] = new_arg;
        old_arg->removeUser(this);
        new_arg->addUser(this);
    }

    void dump(std::ostream& out = std::cout) const override;

  private:
    Graph* func;
    std::vector<Inst*> args;
};

class CastInst final : public FixedInputsInst<1>
{
  public:
    explicit CastInst(size_t id_, Inst* input = nullptr, DataType to_ = DataType::NoType)
        : FixedInputsInst(id_, InstType::Cast), to(to_)
    {
        inputs[0] = input;
        input->addUser(this);
    }

    ~CastInst() = default;

    DataType getFromType() const noexcept
    {
        return inputs[0]->getType();
    }

    DEFINE_GETTER_SETTER(to, ToType, DataType)

    DataType getType() const noexcept override
    {
        return to;
    }

    void dump(std::ostream& out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " v"
            << inputs[0]->getId() << " to " << TYPE_NAME[static_cast<uint8_t>(to)];
    }

  private:
    DataType to = DataType::NoType;
};

class MovInst final : public FixedInputsInst<1>
{
  public:
    explicit MovInst(size_t id_, size_t reg = 0, Inst* input = nullptr)
        : FixedInputsInst(id_, InstType::Mov), reg_num(reg)
    {
        inputs[0] = input;
        input->addUser(this);
    }

    ~MovInst() = default;

    DEFINE_GETTER_SETTER(reg_num, RegNum, size_t)

    DataType getType() const noexcept override
    {
        return inputs[0]->getType();
    }

    void dump(std::ostream& out = std::cout) const override
    {
        out << "\t"
            << "v" << id << ". " << OPER_NAME[static_cast<uint8_t>(inst_type)] << " "
            << TYPE_NAME[static_cast<uint8_t>(getType())] << " r" << reg_num << ", v"
            << inputs[0]->getId();
    }

  private:
    size_t reg_num = 0;
};

class PhiInst : public Inst
{
  public:
    using phi_pair_t = std::pair<Inst*, BasicBlock*>;

    explicit PhiInst(size_t id_) : Inst(id_, InstType::Phi)
    {}

    explicit PhiInst(std::initializer_list<phi_pair_t> inputs_, size_t id_)
        : Inst(id_, InstType::Phi)
    {
        for (auto input : inputs_)
        {
            inputs.push_back(input);
            input.first->addUser(this);
        }
    }

    ~PhiInst() = default;

    DEFINE_ARRAY_GETTER(inputs, Inputs, std::vector<phi_pair_t>&)

    void addInput(Inst* inst, BasicBlock* bb)
    {
        inputs.push_back(std::make_pair(inst, bb));
        inst->addUser(this);
    }

    void addInput(phi_pair_t pair)
    {
        inputs.push_back(pair);
        pair.first->addUser(this);
    }

    void replaceBB(size_t num, BasicBlock* new_bb)
    {
        ASSERT(num < inputs.size() && "too big input number");
        inputs[num].second = new_bb;
    }

    void replaceArg(size_t num, Inst* new_arg)
    {
        ASSERT(num < inputs.size() && "too big input number");
        auto* old_arg = inputs[num].first;
        old_arg->removeUser(this);
        inputs[num].first = new_arg;
        new_arg->addUser(this);
    }

    DataType getType() const noexcept override
    {
        for (auto&& input : inputs)
        {
            DataType type = input.first->getType();
            if (type != DataType::NoType)
                return type;
        }
        return DataType::NoType;
    }

    void dump(std::ostream& out = std::cout) const override;

  private:
    std::vector<phi_pair_t> inputs;
};

} // namespace compiler