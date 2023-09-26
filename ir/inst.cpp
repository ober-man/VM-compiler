#include "inst.h"

namespace compiler
{
	template <typename T>
	constexpr DataType getDataType()
	{
		if constexpr (std::is_integral_v<T>) 
		{
            if (sizeof(T) == sizeof(uint32_t))
                return DataType::INT32;
            else return DataType::INT64;
        }
    	else if constexpr (std::is_same_v<T, float>)
        	return DataType::FLOAT32;
        else if constexpr (std::is_same_v<T, double>)
        	return DataType::FLOAT64;
    	return DataType::NO_TYPE;
	}

	// TODO rewrite to macroses
	std::string BinaryInst::getBinOpTypeString()
	{
		switch (op)
		{
			case BinOpType::ADD:
				return "add";

			case BinOpType::SUB:
				return "sub";

			case BinOpType::MUL:
				return "mul";

			case BinOpType::DIV:
				return "div";

			case BinOpType::MOD:
				return "mod";

			case BinOpType::SHL:
				return "shl";

			case BinOpType::SHR:
				return "shr";

			case BinOpType::AND:
				return "and";

			case BinOpType::OR:
				return "or";

			case BinOpType::XOR:
				return "xor";

			case BinOpType::CMP:
				return "cmp";

			default:
				return "";
		}
	}

	std::string UnaryInst::getUnOpTypeString()
	{
		switch (op)
		{
			case UnOpType::NOT:
				return "not";

			case UnOpType::MOV:
				return "mov";

			case UnOpType::RETURN:
				return "return";

			default:
				return "";
		}

	}

	std::string JumpInst::getJumpOpTypeString()
	{
		switch (op)
		{
			case BinOpType::JMP:
				return "jmp";

			case BinOpType::JE:
				return "je";

			case BinOpType::JNE:
				return "jne";

			case BinOpType::JLT:
				return "jlt";

			case BinOpType::JLE:
				return "jle";

			case BinOpType::JGT:
				return "jgt";

			case BinOpType::JGE:
				return "jge";

			default:
				return "";
		}
	}

	std::string getDataTypeString(DataType type)
	{
		switch (type)
		{
			case DataType::INT32:
				return "int32";

			case DataType::INT64:
				return "int64";

			case DataType::FLOAT32:
				return "float32";

			case DataType::FLOAT64:
				return "float64";

			default:
				return "";
		}
	}

} // namespace compiler