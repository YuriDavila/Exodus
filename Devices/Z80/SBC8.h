#include "Z80Instruction.h"
#ifndef __Z80_SBC8_H__
#define __Z80_SBC8_H__
namespace Z80 {

class SBC8 :public Z80Instruction
{
public:
	virtual SBC8* Clone() const {return new SBC8();}
	virtual SBC8* ClonePlacement(void* buffer) const {return new(buffer) SBC8();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"10011***", L"");
		result &= table.AllocateRegionToOpcode(this, L"11011110", L"");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"SBC";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), _target.Disassemble() + L", " + _source.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_source.SetIndexState(GetIndexState(), GetIndexOffset());
		_target.SetIndexState(GetIndexState(), GetIndexOffset());
		_target.SetMode(EffectiveAddress::Mode::A);

		if (_source.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			// SBC A,r		10001rrr
			AddExecuteCycleCount(4);
		}
		else if (data.GetBit(6))
		{
			// SBC A,n		11001110 nnnnnnnn
			_source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent);
			AddExecuteCycleCount(7);
		}
		else
		{
			// SBC A,(HL)	10001110
			// SBC A,(IX+d)	11011110 10001110 dddddddd
			// SBC A,(IY+d)	11111110 10001110 dddddddd
			_source.SetMode(EffectiveAddress::Mode::HLIndirect);
			if (GetIndexState() == EffectiveAddress::IndexState::None)
			{
				AddExecuteCycleCount(7);
			}
			else
			{
				AddExecuteCycleCount(15);
			}
		}

		AddInstructionSize(GetIndexOffsetSize(_source.UsesIndexOffset() || _target.UsesIndexOffset()));
		AddInstructionSize(_source.ExtensionSize());
		AddInstructionSize(_target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte op2;
		Z80Byte result;
		Z80Byte carry;

		// Perform the operation
		additionalTime += _source.Read(cpu, location, op1);
		additionalTime += _target.Read(cpu, location, op2);
		carry = (unsigned int)cpu->GetFlagC();
		result = op2 - (op1 + carry);
		additionalTime += _target.Write(cpu, location, result);

		// Set the flag results
		cpu->SetFlagS(result.Negative());
		cpu->SetFlagZ(result.Zero());
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH((op1.GetDataSegment(0, 4) + carry.GetData()) > op2.GetDataSegment(0, 4));
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagPV((op1.MSB() != op2.MSB()) && (result.MSB() != op2.MSB()));
		cpu->SetFlagN(true);
		cpu->SetFlagC((op1.GetData() + carry.GetData()) > op2.GetData());

		// Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
};

} // Close namespace Z80
#endif
