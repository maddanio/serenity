#pragma once

namespace JIT::Interface {
template<typename RegType>
struct OperandInterface {
    enum class Type {
        Reg,
        FReg,
        Imm,
        Mem64BaseAndOffset,
    };
    using Operand = OperandInterface<RegType>;
    using Reg = RegType;

    Type type {};
    Reg reg {};
    u64 offset_or_immediate { 0 };

    static Operand Register(Reg reg)
    {
        Operand operand;
        operand.type = Type::Reg;
        operand.reg = reg;
        return operand;
    }

    static Operand FloatRegister(Reg reg)
    {
        Operand operand;
        operand.type = Type::FReg;
        operand.reg = reg;
        return operand;
    }

    static Operand Imm(u64 imm)
    {
        Operand operand;
        operand.type = Type::Imm;
        operand.offset_or_immediate = imm;
        return operand;
    }

    static Operand Mem64BaseAndOffset(Reg base, u64 offset)
    {
        Operand operand;
        operand.type = Type::Mem64BaseAndOffset;
        operand.reg = base;
        operand.offset_or_immediate = offset;
        return operand;
    }

    bool is_register_or_memory() const
    {
        return type == Type::Reg || type == Type::Mem64BaseAndOffset;
    }

    bool fits_in_u8() const
    {
        VERIFY(type == Type::Imm);
        return offset_or_immediate <= NumericLimits<u8>::max();
    }
    bool fits_in_u32() const
    {
        VERIFY(type == Type::Imm);
        return offset_or_immediate <= NumericLimits<u32>::max();
    }
    bool fits_in_i8() const
    {
        VERIFY(type == Type::Imm);
        return (offset_or_immediate <= NumericLimits<i8>::max()) || (((~offset_or_immediate) & NumericLimits<i8>::min()) == 0);
    }
    bool fits_in_i32() const
    {
        VERIFY(type == Type::Imm);
        return (offset_or_immediate <= NumericLimits<i32>::max()) || (((~offset_or_immediate) & NumericLimits<i32>::min()) == 0);
    }
};
template<typename LabelImpl>
struct LabelInterface {
    void link(AssemberImpl& assembler)
    {
        link_to(assembler, assembler.m_output.size());
    }

    void link_to(AssemberImpl& assembler, size_t link_offset)
    {
        impl()->link_to(assemler, link_offset);
    }
private:
    LabelImpl* impl()
    {
        return static_cast<LabelImpl*>(this);
    }
};
template<typename AssemberImpl>
struct AssemblerInterface
{
// label(
// label.link(
// label.link_to(
    using Operand = OperandInterface<typename Impl::Reg>;
    using Label = typename AssemberImpl::Label;
    enum class Condition {
        Overflow = 0x0,
        EqualTo = 0x4,
        NotEqualTo = 0x5,
        UnsignedGreaterThan = 0x7,
        UnsignedGreaterThanOrEqualTo = 0x3,
        UnsignedLessThan = 0x2,
        UnsignedLessThanOrEqualTo = 0x6,
        ParityEven = 0xA,
        ParityOdd = 0xB,
        SignedGreaterThan = 0xF,
        SignedGreaterThanOrEqualTo = 0xD,
        SignedLessThan = 0xC,
        SignedLessThanOrEqualTo = 0xE,

        Unordered = ParityEven,
        NotUnordered = ParityOdd,

        Below = UnsignedLessThan,
        BelowOrEqual = UnsignedLessThanOrEqualTo,
        Above = UnsignedGreaterThan,
        AboveOrEqual = UnsignedGreaterThanOrEqualTo,
    };
    enum class Patchable {
        Yes,
        No,
    };
    enum Extension {
        ZeroExtend,
        SignExtend,
    };
    void add(Operand dst, Operand src) {impl()->add(dst, src);}
    void add32(Operand dst, Operand src, Optional<Label&> overflow_label) {impl()->ad32(dst, src, overflow_label);}
    void arithmetic_right_shift(Operand dst, Optional<Operand> count) {impl()->arithmetic_right_shift(dst, count);}
    void arithmetic_right_shift32(Operand dst, Optional<Operand> count) {impl()->arithmetic_right_shift32(dst, count);}
    void bitwise_and(Operand dst, Operand src) {impl()->bitwise_and(dst, src);}
    void bitwise_or(Operand dst, Operand src) {impl()->bitwise_or(dst, src);}
    void bitwise_xor32(Operand dst, Operand src) {impl()->bitwise_xor32(dst, src);}
    void cmp(Operand lhs, Operand rhs) {impl()->cmp(lhs, rhs);}
    void convert_i32_to_double(Operand dst, Operand src) {impl()->convert_i32_to_double(dst, src);}
    void dec32(Operand op, Optional<Label&> overflow_label) {impl()->dec32(op, overflow_label);}
    void enter() {impl()->enter();}
    void exit() {impl()->exit();}
    void inc32(Operand op, Optional<Label&> overflow_label) {impl()->inc32(op, overflow_label);}
    Label jump() {return impl()->jump();}
    void jump(Label& label) {impl()->jump(label);}
    void jump(Operand op) {impl()->jump(op);}
    void jump_if(Condition condition, Label& label) {impl()->jump_if(condition, label);}
    void jump_if(Operand lhs, Condition condition, Operand rhs, Label& label);
    void mov(Operand dst, Operand src, Patchable patchable = Patchable::No) {impl()->mov(dst, src, patchable);}
    void mov8(Operand dst, Operand src, Extension extension = Extension::ZeroExtend) {impl()->mov8(dst, src, extension);}
    void mov32(Operand dst, Operand src, Extension extension = Extension::ZeroExtend) {impl()->mov32(dst, src, extension);}
    void mov_if(Condition condition, Operand dst, Operand src) {impl()->mul32(condition, dst, src);}
    void mul(Operand dst, Operand src) {impl()->mul(dst, src);}
    void mul32(Operand dst, Operand src, Optional<Label&> overflow_label) {impl()->mul32(dst, src, overflow_label);}
    void native_call(
        u64 callee,
        Vector<Operand> const& preserved_registers = {},
        Vector<Operand> const& stack_arguments = {}
    ) {
        impl()->native_call(
            callee,
            preserved_registers,
            stack_arguments
        );
    }
    void neg32(Operand reg);
    void set_if(Condition condition, Operand dst) {impl()->set_if(condition, dst);}
    void shift_left(Operand dst, Optional<Operand> count) {impl()->shift_left(dst, count);}
    void shift_left32(Operand dst, Optional<Operand> count) {impl()->shift_left32(dst, count);}
    void shift_right(Operand dst, Operand count) {impl()->shift_right(dst, count);}
    void sign_extend_32_to_64_bits(Reg reg) {impl()->sign_extend_32_to_64_bits(reg);}
    void sub(Operand dst, Operand src) {impl()->sub(dst, src);}
    void sub32(Operand dst, Operand src, Optional<Label&> overflow_label) {impl()->sub(dst, src, overflow_label);}
private:
    AssemberImpl* impl()
    {
        return static_cast<AssemberImpl*>(this);
    }
};
}
