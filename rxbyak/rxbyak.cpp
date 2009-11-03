/**
 * RXbyak - Xbyak for Ruby
 * 
 * @author Nakatani Shuyo.
 */

#include <map>
#include <xbyak/xbyak.h>
#include <ruby.h>
#define RB_FUNC(f) reinterpret_cast<VALUE (*)(...)>(f)

// for Ruby 1.8
#ifndef RFLOAT_VALUE
#define RFLOAT_VALUE(v) (RFLOAT(v)->value)
#endif

#define RXBYAK_GENERATOR(s, x) RXbyakGenerator* x;Data_Get_Struct(s, RXbyakGenerator, x)
#define RXBYAK_CALL1(s, m, a1) RXBYAK_GENERATOR(s,rx);rx->m(a1);return Qnil
#define RXBYAK_CALL2(s, m, a1, a2) RXBYAK_GENERATOR(s,rx);rx->m(a1, a2);return Qnil

#define RXBYAK_JUMP(s, m, argc, argv) \
    RXBYAK_GENERATOR(s, rx); \
	if (argc==1) { \
		rx->_##m(argv[0], Qnil); \
	} else if (argc==2) { \
		rx->_##m(argv[0], argv[1]); \
	} else { \
	    rb_raise(rb_eArgError, "wrong number of arguments"); \
	} \
    return Qnil;

#define RXBYAK_JUMP_IMPL(m, dest, jump_type) \
    Check_Type(dest, T_SYMBOL); \
	const char* label = rb_id2name(rb_to_id(dest)); \
	if (jump_type != Qnil) { \
        m(label); /* TODO */ \
	} else { \
        m(label); \
	} \

/**
 * @brief RXbyak implementation
 */
class RXbyakGenerator : public Xbyak::CodeGenerator {
private:
    typedef std::map<ID, const Xbyak::Reg*> Regmap;
    Regmap regmap;

    const Xbyak::Reg& id2reg(const VALUE& x) {
        Check_Type(x, T_SYMBOL);
        ID id = rb_to_id(x);
        Regmap::const_iterator it = regmap.find(id);
        if (it==regmap.end()) rb_raise(rb_eNameError, "no register name");
        return *(it->second);
    }

    const Xbyak::Reg8& id2reg8(const VALUE& x) {
        const Xbyak::Reg& reg = id2reg(x);
        if (!reg.isREG(8)) rb_raise(rb_eTypeError, "not suitable register");
        return (const Xbyak::Reg8&)reg;
    }
    const Xbyak::Reg16& id2reg16(const VALUE& x) {
        const Xbyak::Reg& reg = id2reg(x);
        if (!reg.isREG(16)) rb_raise(rb_eTypeError, "not suitable register");
        return (const Xbyak::Reg16&)reg;
    }
    const Xbyak::Reg32& id2reg32(const VALUE& x) {
        const Xbyak::Reg& reg = id2reg(x);
        if (!reg.isREG(32)) rb_raise(rb_eTypeError, "not suitable register");
        return (const Xbyak::Reg32&)reg;
    }
    const Xbyak::Mmx& id2mmx(const VALUE& x) {
        const Xbyak::Reg& reg = id2reg(x);
        if (!reg.isMMX() && !reg.isXMM())
            rb_raise(rb_eTypeError, "not suitable register");
        return (const Xbyak::Mmx&)reg;
    }
    const Xbyak::Xmm& id2xmmx(const VALUE& x) {
        const Xbyak::Reg& reg = id2reg(x);
        if (!reg.isXMM()) rb_raise(rb_eTypeError, "not suitable register");
        return (const Xbyak::Xmm&)reg;
    }

    Xbyak::Address ary2address(const VALUE& address) {
        size_t len = RARRAY_LEN(address);
        const VALUE* ary = RARRAY_PTR(address);
        if (len<=0 || len>3) rb_raise(rb_eTypeError, "number of address parameters must be between 1 and 3");

        const Xbyak::Reg32& pointer = id2reg32(ary[0]);
        if (len==1) return ptr[pointer];

        int offset = FIX2INT(ary[1]);
        if (len==2) return ptr[pointer + offset];

        const Xbyak::Reg32 offset_r = id2reg32(ary[2]);
        return ptr[pointer + offset_r * offset];
    }

public:
    RXbyakGenerator() {
        regmap[rb_intern("al")] = &al;
        regmap[rb_intern("bl")] = &bl;
        regmap[rb_intern("cl")] = &cl;
        regmap[rb_intern("dl")] = &dl;
        regmap[rb_intern("ah")] = &ah;
        regmap[rb_intern("bh")] = &bh;
        regmap[rb_intern("ch")] = &ch;
        regmap[rb_intern("dh")] = &dh;

        regmap[rb_intern("ax")] = &ax;
        regmap[rb_intern("bx")] = &bx;
        regmap[rb_intern("cx")] = &cx;
        regmap[rb_intern("dx")] = &dx;
        regmap[rb_intern("sp")] = &sp;
        regmap[rb_intern("bp")] = &bp;
        regmap[rb_intern("si")] = &si;
        regmap[rb_intern("di")] = &di;

        regmap[rb_intern("eax")] = &eax;
        regmap[rb_intern("ebx")] = &ebx;
        regmap[rb_intern("ecx")] = &ecx;
        regmap[rb_intern("edx")] = &edx;
        regmap[rb_intern("esp")] = &esp;
        regmap[rb_intern("ebp")] = &ebp;
        regmap[rb_intern("esi")] = &esi;
        regmap[rb_intern("edi")] = &edi;

        regmap[rb_intern("mm0")] = &mm0;
        regmap[rb_intern("mm1")] = &mm1;
        regmap[rb_intern("mm2")] = &mm2;
        regmap[rb_intern("mm3")] = &mm3;
        regmap[rb_intern("mm4")] = &mm4;
        regmap[rb_intern("mm5")] = &mm5;
        regmap[rb_intern("mm6")] = &mm6;
        regmap[rb_intern("mm7")] = &mm7;

        regmap[rb_intern("xmm0")] = &xmm0;
        regmap[rb_intern("xmm1")] = &xmm1;
        regmap[rb_intern("xmm2")] = &xmm2;
        regmap[rb_intern("xmm3")] = &xmm3;
        regmap[rb_intern("xmm4")] = &xmm4;
        regmap[rb_intern("xmm5")] = &xmm5;
        regmap[rb_intern("xmm6")] = &xmm6;
        regmap[rb_intern("xmm7")] = &xmm7;
    }

    bool is_reg(const VALUE& x) {
        Check_Type(x, T_SYMBOL);
        ID id = rb_to_id(x);
        Regmap::const_iterator it = regmap.find(id);
        return (it!=regmap.end());
    }


    void _mov(const VALUE& dest, const VALUE& src) {
        switch (TYPE(dest)) {
        case T_ARRAY:
            switch (TYPE(src)) {
            case T_ARRAY:
            case T_SYMBOL:
            case T_FIXNUM:
                rb_raise(rb_eStandardError, "not yet support");
                return;
            }
        case T_SYMBOL:
            switch (TYPE(src)) {
            case T_ARRAY:
                mov(id2reg(dest), ary2address(src));
                return;
            case T_FIXNUM:
                mov(id2reg(dest), NUM2LONG(src));
                return;
            case T_SYMBOL:
                rb_raise(rb_eStandardError, "not yet support");
                return;
            }
        }
        rb_raise(rb_eArgError, "illegal operand");
    }

    void _movq(const VALUE& dest, const VALUE& src) {
        switch (TYPE(dest)) {
        case T_ARRAY:
            switch (TYPE(src)) {
            case T_SYMBOL:
                movq(ary2address(dest), id2mmx(src));
                return;
            case T_ARRAY:
            //case T_FLOAT:
                rb_raise(rb_eStandardError, "not yet support");
                return;
            }
        case T_SYMBOL:
            switch (TYPE(src)) {
            case T_ARRAY:
                movq(id2mmx(dest), ary2address(src));
                return;
            case T_SYMBOL:
            //case T_FLOAT:
                rb_raise(rb_eStandardError, "not yet support");
                return;
            }
        }
        rb_raise(rb_eArgError, "illegal operand");
    }

    void _addsd(const VALUE& xmm_dest, const VALUE& xmm_src) {
        const Xbyak::Xmm& dest = id2xmmx(xmm_dest);
        const Xbyak::Xmm& src  = id2xmmx(xmm_src);
        addsd(dest, src);
    }

    void _subsd(const VALUE& xmm_dest, const VALUE& xmm_src) {
        const Xbyak::Xmm& dest = id2xmmx(xmm_dest);
        const Xbyak::Xmm& src  = id2xmmx(xmm_src);
        subsd(dest, src);
    }

    void _mulsd(const VALUE& xmm_dest, const VALUE& xmm_src) {
        const Xbyak::Xmm& dest = id2xmmx(xmm_dest);
        const Xbyak::Xmm& src  = id2xmmx(xmm_src);
        mulsd(dest, src);
    }

    void _divsd(const VALUE& xmm_dest, const VALUE& xmm_src) {
        const Xbyak::Xmm& dest = id2xmmx(xmm_dest);
        const Xbyak::Xmm& src  = id2xmmx(xmm_src);
        divsd(dest, src);
    }

    void _pop(const VALUE& op1) {
        const Xbyak::Reg& dest = id2reg(op1);
        pop(dest);
    }
    void _push(const VALUE& op1) {
        const Xbyak::Reg& dest = id2reg(op1);
        push(dest);
    }
    void _dec(const VALUE& op1) {
        const Xbyak::Reg& dest = id2reg(op1);
        dec(dest);
    }
    void _inc(const VALUE& op1) {
        const Xbyak::Reg& dest = id2reg(op1);
        inc(dest);
    }
    void _div(const VALUE& op1) {
        const Xbyak::Reg& dest = id2reg(op1);
        div(dest);
    }
    void _idiv(const VALUE& op1) {
        const Xbyak::Reg& dest = id2reg(op1);
        idiv(dest);
    }
    void _imul(const VALUE& op1) {
        const Xbyak::Reg& dest = id2reg(op1);
        imul(dest);
    }
    void _mul(const VALUE& op1) {
        const Xbyak::Reg& dest = id2reg(op1);
        mul(dest);
    }
    void _neg(const VALUE& op1) {
        const Xbyak::Reg& dest = id2reg(op1);
        neg(dest);
    }
    void _not(const VALUE& op1) {
        const Xbyak::Reg& dest = id2reg(op1);
        not(dest);
    }
    void set_label(const VALUE& x) {
        Check_Type(x, T_SYMBOL);
        const char* label = rb_id2name(rb_to_id(x));
        L(label);
    }

    void _add(const VALUE& dest, const VALUE& src) {
        switch (TYPE(dest)) {
        case T_ARRAY:
            switch (TYPE(src)) {
            case T_SYMBOL:
                add(ary2address(dest), id2reg(src));
                return;
            case T_FIXNUM:
                add(ary2address(dest), NUM2LONG(src));
                return;
            }
        case T_SYMBOL:
            switch (TYPE(src)) {
            case T_ARRAY:
                add(id2reg(dest), ary2address(src));
                return;
            case T_FIXNUM:
                add(id2reg(dest), NUM2LONG(src));
                return;
            case T_SYMBOL:
                add(id2reg(dest), id2reg(src));
                return;
            }
        }
        rb_raise(rb_eArgError, "illegal operand");
    }
    void _adc(const VALUE& dest, const VALUE& src) {
    }
    void _sub(const VALUE& dest, const VALUE& src) {
    }
    void _sbb(const VALUE& dest, const VALUE& src) {
    }


    void _jmp(const VALUE& dest, const VALUE& jump_type) {
        Check_Type(dest, T_SYMBOL);
        const char* label = rb_id2name(rb_to_id(dest));
        if (jump_type != Qnil) {
            jmp(label); /* TODO */
        } else {
            jmp(label);
        } \
    }
    void _jo(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jo, dest, jump_type);
    }
    void _jno(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jno, dest, jump_type);
    }
    void _jb(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jb, dest, jump_type);
    }
    void _jnae(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jnae, dest, jump_type);
    }
    void _jnb(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jnb, dest, jump_type);
    }
    void _jae(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jae, dest, jump_type);
    }
    void _je(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(je, dest, jump_type);
    }
    void _jz(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jz, dest, jump_type);
    }
    void _jne(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jne, dest, jump_type);
    }
    void _jnz(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jnz, dest, jump_type);
    }
    void _jbe(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jbe, dest, jump_type);
    }
    void _jna(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jna, dest, jump_type);
    }
    void _jnbe(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jnbe, dest, jump_type);
    }
    void _ja(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(ja, dest, jump_type);
    }
    void _js(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(js, dest, jump_type);
    }
    void _jns(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jns, dest, jump_type);
    }
    void _jp(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jp, dest, jump_type);
    }
    void _jpe(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jpe, dest, jump_type);
    }
    void _jnp(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jnp, dest, jump_type);
    }
    void _jpo(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jpo, dest, jump_type);
    }
    void _jl(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jl, dest, jump_type);
    }
    void _jnge(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jnge, dest, jump_type);
    }
    void _jnl(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jnl, dest, jump_type);
    }
    void _jge(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jge, dest, jump_type);
    }
    void _jle(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jle, dest, jump_type);
    }
    void _jng(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jng, dest, jump_type);
    }
    void _jnle(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jnle, dest, jump_type);
    }
    void _jg(const VALUE& dest, const VALUE& jump_type) {
        RXBYAK_JUMP_IMPL(jg, dest, jump_type);
    }


    void _ret(int imm = 0) { ret(imm); }
    void _aaa() { aaa(); }
    void _aad() { aad(); }
    void _aam() { aam(); }
    void _aas() { aas(); }
    void _daa() { daa(); }
    void _das() { das(); }
    void _popad() { popad(); }
    void _popfd() { popfd(); }
    void _pusha() { pusha(); }
    void _pushad() { pushad(); }
    void _pushfd() { pushfd(); }
    void _popa() { popa(); }
    void _cbw() { cbw(); }
    void _cdq() { cdq(); }
    void _clc() { clc(); }
    void _cld() { cld(); }
    void _cli() { cli(); }
    void _cmc() { cmc(); }
    void _cpuid() { cpuid(); }
    void _cwd() { cwd(); }
    void _cwde() { cwde(); }
    void _lahf() { lahf(); }
    void _lock() { lock(); }
    void _nop() { nop(); }
    void _sahf() { sahf(); }
    void _stc() { stc(); }
    void _std() { std(); }
    void _sti() { sti(); }
    void _emms() { emms(); }
    void _pause() { pause(); }
    void _sfence() { sfence(); }
    void _lfence() { lfence(); }
    void _mfence() { mfence(); }
    void _monitor() { monitor(); }
    void _mwait() { mwait(); }
    void _rdmsr() { rdmsr(); }
    void _rdpmc() { rdpmc(); }
    void _rdtsc() { rdtsc(); }
    void _wait() { wait(); }
    void _wbinvd() { wbinvd(); }
    void _wrmsr() { wrmsr(); }
    void _xlatb() { xlatb(); }
    void _popf() { popf(); }
    void _pushf() { pushf(); }
};




//// Ruby extention interface ////

// IA32 operand

extern "C" VALUE RXbyak_mov(VALUE self, VALUE a1, VALUE a2) { RXBYAK_CALL2(self, _mov, a1, a2); }
extern "C" VALUE RXbyak_movq(VALUE self, VALUE a1, VALUE a2) { RXBYAK_CALL2(self, _movq, a1, a2); }

extern "C" VALUE RXbyak_addsd(VALUE self, VALUE a1, VALUE a2) {
    RXBYAK_GENERATOR(self, rx);
    rx->_addsd(a1, a2);
    return Qnil;
}

extern "C" VALUE RXbyak_subsd(VALUE self, VALUE a1, VALUE a2) {
    RXBYAK_GENERATOR(self, rx);
    rx->_subsd(a1, a2);
    return Qnil;
}

extern "C" VALUE RXbyak_mulsd(VALUE self, VALUE a1, VALUE a2) {
    RXBYAK_GENERATOR(self, rx);
    rx->_mulsd(a1, a2);
    return Qnil;
}

extern "C" VALUE RXbyak_divsd(VALUE self, VALUE a1, VALUE a2) {
    RXBYAK_GENERATOR(self, rx);
    rx->_divsd(a1, a2);
    return Qnil;
}


extern "C" VALUE RXbyak_ret(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_ret();
    return Qnil;
}

extern "C" VALUE RXbyak_aaa(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_aaa();
    return Qnil;
}
extern "C" VALUE RXbyak_aad(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_aad();
    return Qnil;
}
extern "C" VALUE RXbyak_aam(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_aam();
    return Qnil;
}
extern "C" VALUE RXbyak_aas(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_aas();
    return Qnil;
}
extern "C" VALUE RXbyak_daa(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_daa();
    return Qnil;
}
extern "C" VALUE RXbyak_das(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_das();
    return Qnil;
}
extern "C" VALUE RXbyak_popad(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_popad();
    return Qnil;
}
extern "C" VALUE RXbyak_popfd(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_popfd();
    return Qnil;
}
extern "C" VALUE RXbyak_pusha(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_pusha();
    return Qnil;
}
extern "C" VALUE RXbyak_pushad(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_pushad();
    return Qnil;
}
extern "C" VALUE RXbyak_pushfd(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_pushfd();
    return Qnil;
}
extern "C" VALUE RXbyak_popa(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_popa();
    return Qnil;
}
extern "C" VALUE RXbyak_cbw(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_cbw();
    return Qnil;
}
extern "C" VALUE RXbyak_cdq(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_cdq();
    return Qnil;
}
extern "C" VALUE RXbyak_clc(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_clc();
    return Qnil;
}
extern "C" VALUE RXbyak_cld(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_cld();
    return Qnil;
}
extern "C" VALUE RXbyak_cli(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_cli();
    return Qnil;
}
extern "C" VALUE RXbyak_cmc(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_cmc();
    return Qnil;
}
extern "C" VALUE RXbyak_cpuid(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_cpuid();
    return Qnil;
}
extern "C" VALUE RXbyak_cwd(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_cwd();
    return Qnil;
}
extern "C" VALUE RXbyak_cwde(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_cwde();
    return Qnil;
}
extern "C" VALUE RXbyak_lahf(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_lahf();
    return Qnil;
}
extern "C" VALUE RXbyak_lock(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_lock();
    return Qnil;
}
extern "C" VALUE RXbyak_nop(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_nop();
    return Qnil;
}
extern "C" VALUE RXbyak_sahf(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_sahf();
    return Qnil;
}
extern "C" VALUE RXbyak_stc(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_stc();
    return Qnil;
}
extern "C" VALUE RXbyak_std(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_std();
    return Qnil;
}
extern "C" VALUE RXbyak_sti(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_sti();
    return Qnil;
}
extern "C" VALUE RXbyak_emms(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_emms();
    return Qnil;
}
extern "C" VALUE RXbyak_pause(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_pause();
    return Qnil;
}
extern "C" VALUE RXbyak_sfence(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_sfence();
    return Qnil;
}
extern "C" VALUE RXbyak_lfence(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_lfence();
    return Qnil;
}
extern "C" VALUE RXbyak_mfence(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_mfence();
    return Qnil;
}
extern "C" VALUE RXbyak_monitor(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_monitor();
    return Qnil;
}
extern "C" VALUE RXbyak_mwait(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_mwait();
    return Qnil;
}
extern "C" VALUE RXbyak_rdmsr(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_rdmsr();
    return Qnil;
}
extern "C" VALUE RXbyak_rdpmc(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_rdpmc();
    return Qnil;
}
extern "C" VALUE RXbyak_rdtsc(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_rdtsc();
    return Qnil;
}
extern "C" VALUE RXbyak_wait(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_wait();
    return Qnil;
}
extern "C" VALUE RXbyak_wbinvd(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_wbinvd();
    return Qnil;
}
extern "C" VALUE RXbyak_wrmsr(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_wrmsr();
    return Qnil;
}
extern "C" VALUE RXbyak_xlatb(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_xlatb();
    return Qnil;
}
extern "C" VALUE RXbyak_popf(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_popf();
    return Qnil;
}
extern "C" VALUE RXbyak_pushf(VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    rx->_pushf();
    return Qnil;
}

extern "C" VALUE RXbyak_pop(VALUE self, VALUE op1) {
    RXBYAK_GENERATOR(self, rx);
    rx->_pop(op1);
    return Qnil;
}
extern "C" VALUE RXbyak_push(VALUE self, VALUE op1) {
    RXBYAK_GENERATOR(self, rx);
    rx->_push(op1);
    return Qnil;
}
extern "C" VALUE RXbyak_dec(VALUE self, VALUE op1) {
    RXBYAK_GENERATOR(self, rx);
    rx->_dec(op1);
    return Qnil;
}
extern "C" VALUE RXbyak_inc(VALUE self, VALUE op1) {
    RXBYAK_GENERATOR(self, rx);
    rx->_inc(op1);
    return Qnil;
}
extern "C" VALUE RXbyak_div(VALUE self, VALUE op1) {
    RXBYAK_GENERATOR(self, rx);
    rx->_div(op1);
    return Qnil;
}
extern "C" VALUE RXbyak_idiv(VALUE self, VALUE op1) {
    RXBYAK_GENERATOR(self, rx);
    rx->_idiv(op1);
    return Qnil;
}
extern "C" VALUE RXbyak_imul(VALUE self, VALUE op1) {
    RXBYAK_GENERATOR(self, rx);
    rx->_imul(op1);
    return Qnil;
}
extern "C" VALUE RXbyak_mul(VALUE self, VALUE op1) {
    RXBYAK_GENERATOR(self, rx);
    rx->_mul(op1);
    return Qnil;
}
extern "C" VALUE RXbyak_neg(VALUE self, VALUE op1) {
    RXBYAK_GENERATOR(self, rx);
    rx->_neg(op1);
    return Qnil;
}
extern "C" VALUE RXbyak_not(VALUE self, VALUE op1) { RXBYAK_CALL1(self, _not, op1); }

extern "C" VALUE RXbyak_label(VALUE self, VALUE label) { RXBYAK_CALL1(self, set_label, label); }

extern "C" VALUE RXbyak_add(VALUE self, VALUE a1, VALUE a2) { RXBYAK_CALL2(self, _add, a1, a2); }
extern "C" VALUE RXbyak_adc(VALUE self, VALUE a1, VALUE a2) { RXBYAK_CALL2(self, _adc, a1, a2); }
extern "C" VALUE RXbyak_sub(VALUE self, VALUE a1, VALUE a2) { RXBYAK_CALL2(self, _sub, a1, a2); }
extern "C" VALUE RXbyak_sbb(VALUE self, VALUE a1, VALUE a2) { RXBYAK_CALL2(self, _sbb, a1, a2); }


extern "C" VALUE RXbyak_jmp(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jmp, argc, argv); }
extern "C" VALUE RXbyak_jo(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jo, argc, argv); }
extern "C" VALUE RXbyak_jno(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jno, argc, argv); }
extern "C" VALUE RXbyak_jb(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jb, argc, argv); }
extern "C" VALUE RXbyak_jnae(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jnae, argc, argv); }
extern "C" VALUE RXbyak_jnb(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jnb, argc, argv); }
extern "C" VALUE RXbyak_jae(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jae, argc, argv); }
extern "C" VALUE RXbyak_je(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, je, argc, argv); }
extern "C" VALUE RXbyak_jz(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jz, argc, argv); }
extern "C" VALUE RXbyak_jne(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jne, argc, argv); }
extern "C" VALUE RXbyak_jnz(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jnz, argc, argv); }
extern "C" VALUE RXbyak_jbe(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jbe, argc, argv); }
extern "C" VALUE RXbyak_jna(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jna, argc, argv); }
extern "C" VALUE RXbyak_jnbe(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jnbe, argc, argv); }
extern "C" VALUE RXbyak_ja(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, ja, argc, argv); }
extern "C" VALUE RXbyak_js(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, js, argc, argv); }
extern "C" VALUE RXbyak_jns(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jns, argc, argv); }
extern "C" VALUE RXbyak_jp(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jp, argc, argv); }
extern "C" VALUE RXbyak_jpe(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jpe, argc, argv); }
extern "C" VALUE RXbyak_jnp(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jnp, argc, argv); }
extern "C" VALUE RXbyak_jpo(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jpo, argc, argv); }
extern "C" VALUE RXbyak_jl(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jl, argc, argv); }
extern "C" VALUE RXbyak_jnge(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jnge, argc, argv); }
extern "C" VALUE RXbyak_jnl(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jnl, argc, argv); }
extern "C" VALUE RXbyak_jge(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jge, argc, argv); }
extern "C" VALUE RXbyak_jle(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jle, argc, argv); }
extern "C" VALUE RXbyak_jng(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jng, argc, argv); }
extern "C" VALUE RXbyak_jnle(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jnle, argc, argv); }
extern "C" VALUE RXbyak_jg(int argc, const VALUE* argv, VALUE self) { RXBYAK_JUMP(self, jg, argc, argv); }




extern "C" 
VALUE RXbyak_method_missing(int argc, const VALUE* argv, VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    if (rx->is_reg(argv[0])) return argv[0];
    rb_raise(rb_eNoMethodError, "not supported method");
}

// exec the generated code

extern "C" 
VALUE RXbyak_exec(int argc, const VALUE* argv, VALUE self) {
    RXBYAK_GENERATOR(self, rx);
    if (argc<2) rb_raise(rb_eTypeError, "too few parameters");
    Check_Type(argv[0], T_FLOAT);
    Check_Type(argv[1], T_FLOAT);

    double result=0;

    void (*proc)(double*, const double*, const double*) = (void (*)(double*, const double*, const double*))rx->getCode();
    proc(&result, &RFLOAT_VALUE(argv[0]), &RFLOAT_VALUE(argv[1]));
    VALUE r = rb_float_new(result);

    return r;
}


// allocator

extern "C" 
void RXbyak_free(void* rx) {
    delete (RXbyakGenerator*)rx;
}

extern "C" 
VALUE RXbyak_alloc(VALUE klass) {
    RXbyakGenerator* rx = new RXbyakGenerator();
    return Data_Wrap_Struct(klass, 0, RXbyak_free, rx);
}


// initialization

extern "C" 
void Init_RXbyak(void) {
    VALUE rb_cRXbyak;
    rb_cRXbyak = rb_define_class("RXbyak", rb_cObject);
    rb_define_alloc_func(rb_cRXbyak, RXbyak_alloc);

    rb_define_method(rb_cRXbyak, "mov", RB_FUNC(RXbyak_mov), 2);
    rb_define_method(rb_cRXbyak, "movq", RB_FUNC(RXbyak_movq), 2);
    rb_define_method(rb_cRXbyak, "addsd", RB_FUNC(RXbyak_addsd), 2);
    rb_define_method(rb_cRXbyak, "subsd", RB_FUNC(RXbyak_subsd), 2);
    rb_define_method(rb_cRXbyak, "mulsd", RB_FUNC(RXbyak_mulsd), 2);
    rb_define_method(rb_cRXbyak, "divsd", RB_FUNC(RXbyak_divsd), 2);
    rb_define_method(rb_cRXbyak, "ret", RB_FUNC(RXbyak_ret), 0);

    rb_define_method(rb_cRXbyak, "add", RB_FUNC(RXbyak_add), 2);
    rb_define_method(rb_cRXbyak, "adc", RB_FUNC(RXbyak_adc), 2);
    rb_define_method(rb_cRXbyak, "sub", RB_FUNC(RXbyak_sub), 2);
    rb_define_method(rb_cRXbyak, "sbb", RB_FUNC(RXbyak_sbb), 2);

    rb_define_method(rb_cRXbyak, "aaa", RB_FUNC(RXbyak_aaa), 0);
    rb_define_method(rb_cRXbyak, "aad", RB_FUNC(RXbyak_aad), 0);
    rb_define_method(rb_cRXbyak, "aam", RB_FUNC(RXbyak_aam), 0);
    rb_define_method(rb_cRXbyak, "aas", RB_FUNC(RXbyak_aas), 0);
    rb_define_method(rb_cRXbyak, "daa", RB_FUNC(RXbyak_daa), 0);
    rb_define_method(rb_cRXbyak, "das", RB_FUNC(RXbyak_das), 0);
    rb_define_method(rb_cRXbyak, "popad", RB_FUNC(RXbyak_popad), 0);
    rb_define_method(rb_cRXbyak, "popfd", RB_FUNC(RXbyak_popfd), 0);
    rb_define_method(rb_cRXbyak, "pusha", RB_FUNC(RXbyak_pusha), 0);
    rb_define_method(rb_cRXbyak, "pushad", RB_FUNC(RXbyak_pushad), 0);
    rb_define_method(rb_cRXbyak, "pushfd", RB_FUNC(RXbyak_pushfd), 0);
    rb_define_method(rb_cRXbyak, "popa", RB_FUNC(RXbyak_popa), 0);
    rb_define_method(rb_cRXbyak, "cbw", RB_FUNC(RXbyak_cbw), 0);
    rb_define_method(rb_cRXbyak, "cdq", RB_FUNC(RXbyak_cdq), 0);
    rb_define_method(rb_cRXbyak, "clc", RB_FUNC(RXbyak_clc), 0);
    rb_define_method(rb_cRXbyak, "cld", RB_FUNC(RXbyak_cld), 0);
    rb_define_method(rb_cRXbyak, "cli", RB_FUNC(RXbyak_cli), 0);
    rb_define_method(rb_cRXbyak, "cmc", RB_FUNC(RXbyak_cmc), 0);
    rb_define_method(rb_cRXbyak, "cpuid", RB_FUNC(RXbyak_cpuid), 0);
    rb_define_method(rb_cRXbyak, "cwd", RB_FUNC(RXbyak_cwd), 0);
    rb_define_method(rb_cRXbyak, "cwde", RB_FUNC(RXbyak_cwde), 0);
    rb_define_method(rb_cRXbyak, "lahf", RB_FUNC(RXbyak_lahf), 0);
    rb_define_method(rb_cRXbyak, "lock", RB_FUNC(RXbyak_lock), 0);
    rb_define_method(rb_cRXbyak, "nop", RB_FUNC(RXbyak_nop), 0);
    rb_define_method(rb_cRXbyak, "sahf", RB_FUNC(RXbyak_sahf), 0);
    rb_define_method(rb_cRXbyak, "stc", RB_FUNC(RXbyak_stc), 0);
    rb_define_method(rb_cRXbyak, "std", RB_FUNC(RXbyak_std), 0);
    rb_define_method(rb_cRXbyak, "sti", RB_FUNC(RXbyak_sti), 0);
    rb_define_method(rb_cRXbyak, "emms", RB_FUNC(RXbyak_emms), 0);
    rb_define_method(rb_cRXbyak, "pause", RB_FUNC(RXbyak_pause), 0);
    rb_define_method(rb_cRXbyak, "sfence", RB_FUNC(RXbyak_sfence), 0);
    rb_define_method(rb_cRXbyak, "lfence", RB_FUNC(RXbyak_lfence), 0);
    rb_define_method(rb_cRXbyak, "mfence", RB_FUNC(RXbyak_mfence), 0);
    rb_define_method(rb_cRXbyak, "monitor", RB_FUNC(RXbyak_monitor), 0);
    rb_define_method(rb_cRXbyak, "mwait", RB_FUNC(RXbyak_mwait), 0);
    rb_define_method(rb_cRXbyak, "rdmsr", RB_FUNC(RXbyak_rdmsr), 0);
    rb_define_method(rb_cRXbyak, "rdpmc", RB_FUNC(RXbyak_rdpmc), 0);
    rb_define_method(rb_cRXbyak, "rdtsc", RB_FUNC(RXbyak_rdtsc), 0);
    rb_define_method(rb_cRXbyak, "wait", RB_FUNC(RXbyak_wait), 0);
    rb_define_method(rb_cRXbyak, "wbinvd", RB_FUNC(RXbyak_wbinvd), 0);
    rb_define_method(rb_cRXbyak, "wrmsr", RB_FUNC(RXbyak_wrmsr), 0);
    rb_define_method(rb_cRXbyak, "xlatb", RB_FUNC(RXbyak_xlatb), 0);
    rb_define_method(rb_cRXbyak, "popf", RB_FUNC(RXbyak_popf), 0);
    rb_define_method(rb_cRXbyak, "pushf", RB_FUNC(RXbyak_pushf), 0);

    rb_define_method(rb_cRXbyak, "pop", RB_FUNC(RXbyak_pop), 1);
    rb_define_method(rb_cRXbyak, "push", RB_FUNC(RXbyak_push), 1);
    rb_define_method(rb_cRXbyak, "dec", RB_FUNC(RXbyak_dec), 1);
    rb_define_method(rb_cRXbyak, "inc", RB_FUNC(RXbyak_inc), 1);
    rb_define_method(rb_cRXbyak, "div", RB_FUNC(RXbyak_div), 1);
    rb_define_method(rb_cRXbyak, "idiv", RB_FUNC(RXbyak_idiv), 1);
    rb_define_method(rb_cRXbyak, "imul", RB_FUNC(RXbyak_imul), 1);
    rb_define_method(rb_cRXbyak, "mul", RB_FUNC(RXbyak_mul), 1);
    rb_define_method(rb_cRXbyak, "neg", RB_FUNC(RXbyak_neg), 1);
    rb_define_method(rb_cRXbyak, "not", RB_FUNC(RXbyak_not), 1);

    rb_define_method(rb_cRXbyak, "L", RB_FUNC(RXbyak_label), 1);

    rb_define_method(rb_cRXbyak, "jmp", RB_FUNC(RXbyak_jmp), -1);
    rb_define_method(rb_cRXbyak, "jo", RB_FUNC(RXbyak_jo), -1);
    rb_define_method(rb_cRXbyak, "jno", RB_FUNC(RXbyak_jno), -1);
    rb_define_method(rb_cRXbyak, "jb", RB_FUNC(RXbyak_jb), -1);
    rb_define_method(rb_cRXbyak, "jnae", RB_FUNC(RXbyak_jnae), -1);
    rb_define_method(rb_cRXbyak, "jnb", RB_FUNC(RXbyak_jnb), -1);
    rb_define_method(rb_cRXbyak, "jae", RB_FUNC(RXbyak_jae), -1);
    rb_define_method(rb_cRXbyak, "je", RB_FUNC(RXbyak_je), -1);
    rb_define_method(rb_cRXbyak, "jz", RB_FUNC(RXbyak_jz), -1);
    rb_define_method(rb_cRXbyak, "jne", RB_FUNC(RXbyak_jne), -1);
    rb_define_method(rb_cRXbyak, "jnz", RB_FUNC(RXbyak_jnz), -1);
    rb_define_method(rb_cRXbyak, "jbe", RB_FUNC(RXbyak_jbe), -1);
    rb_define_method(rb_cRXbyak, "jna", RB_FUNC(RXbyak_jna), -1);
    rb_define_method(rb_cRXbyak, "jnbe", RB_FUNC(RXbyak_jnbe), -1);
    rb_define_method(rb_cRXbyak, "ja", RB_FUNC(RXbyak_ja), -1);
    rb_define_method(rb_cRXbyak, "js", RB_FUNC(RXbyak_js), -1);
    rb_define_method(rb_cRXbyak, "jns", RB_FUNC(RXbyak_jns), -1);
    rb_define_method(rb_cRXbyak, "jp", RB_FUNC(RXbyak_jp), -1);
    rb_define_method(rb_cRXbyak, "jpe", RB_FUNC(RXbyak_jpe), -1);
    rb_define_method(rb_cRXbyak, "jnp", RB_FUNC(RXbyak_jnp), -1);
    rb_define_method(rb_cRXbyak, "jpo", RB_FUNC(RXbyak_jpo), -1);
    rb_define_method(rb_cRXbyak, "jl", RB_FUNC(RXbyak_jl), -1);
    rb_define_method(rb_cRXbyak, "jnge", RB_FUNC(RXbyak_jnge), -1);
    rb_define_method(rb_cRXbyak, "jnl", RB_FUNC(RXbyak_jnl), -1);
    rb_define_method(rb_cRXbyak, "jge", RB_FUNC(RXbyak_jge), -1);
    rb_define_method(rb_cRXbyak, "jle", RB_FUNC(RXbyak_jle), -1);
    rb_define_method(rb_cRXbyak, "jng", RB_FUNC(RXbyak_jng), -1);
    rb_define_method(rb_cRXbyak, "jnle", RB_FUNC(RXbyak_jnle), -1);
    rb_define_method(rb_cRXbyak, "jg", RB_FUNC(RXbyak_jg), -1);

    rb_define_method(rb_cRXbyak, "method_missing", RB_FUNC(RXbyak_method_missing), -1);

    rb_define_method(rb_cRXbyak, "exec", RB_FUNC(RXbyak_exec), -1);
}

