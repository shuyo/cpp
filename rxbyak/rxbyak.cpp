#include <ruby.h>
#define RB_FUNC(f) reinterpret_cast<VALUE (*)(...)>(f)

#include "xbyak/xbyak.h"


class RXbyakGenerator : public Xbyak::CodeGenerator {
private:
	const ID id_eax, id_esp, id_xmm0;
	const Xbyak::Reg32& id2operand(const VALUE& x) {
    	Check_Type(x, T_SYMBOL);
		ID id = rb_to_id(x);

		if (id==id_eax) return eax;
		if (id==id_esp) return esp;

		rb_raise(rb_eTypeError, "not valid operand");
	}
	const Xbyak::Mmx& id2mmx(const VALUE& x) {
    	Check_Type(x, T_SYMBOL);
		ID id = rb_to_id(x);

		if (id==id_xmm0) return xmm0;

		rb_raise(rb_eTypeError, "not valid operand. need :mm[0-7] / :xmm[0-7]");
	}

	Xbyak::Address ary2address(const VALUE& address) {
        size_t len = RARRAY_LEN(address);
        const VALUE* ary = RARRAY_PTR(address);
        if (len<=0 || len>3) rb_raise(rb_eTypeError, "number of address parameters must be between 1 and 3");

        const Xbyak::Reg32e pointer = id2operand(ary[0]);
        if (len==1) return ptr[pointer];

        int offset = FIX2INT(ary[1]);
        if (len==2) return ptr[pointer + offset];

        const Xbyak::Reg32e offset_r = id2operand(ary[2]);
        return ptr[pointer + offset_r * offset];
    }

public:
    RXbyakGenerator() : 
    	id_eax(rb_intern("eax")),
    	id_esp(rb_intern("esp")),
    	id_xmm0(rb_intern("xmm0"))
    {}

    void _mov_reg_long(const VALUE& dist, long x) {
		const Xbyak::Operand reg1 = id2operand(dist);
        mov(reg1, x);
    }

    void _movq_reg_address(const VALUE& dist, VALUE address) {
		const Xbyak::Mmx reg1 = id2mmx(dist);
		const Xbyak::Address addr = ary2address(address);
        movq(reg1, addr);
    }
/*
    void _movq(VALUE dist, int offset) {
		const Mmx reg1 = id2operand(dist);
        movq(reg1, ptr[esp+offset]);
    }
*/
    void _ret() {
        ret();
    }
};

extern "C" 
void RXbyak_free(void* buf) {
    delete (RXbyakGenerator*)buf;
}

extern "C" 
VALUE RXbyak_alloc(VALUE klass) {
    RXbyakGenerator* buf = new RXbyakGenerator();
    return Data_Wrap_Struct(klass, 0, RXbyak_free, buf);
}

extern "C" 
VALUE RXbyak_call(int argc, const VALUE* argv, VALUE self) {
    RXbyakGenerator* buf;
    Data_Get_Struct(self, RXbyakGenerator, buf);

    //int (*proc)() = (int (*)())buf->getCode();
	//VALUE r = INT2FIX(proc());

    if (argc<2) rb_raise(rb_eTypeError, "too few parameters");
	Check_Type(argv[0], T_FLOAT);
	Check_Type(argv[1], T_FLOAT);

	double result=0;

	void (*proc)(double*, const double*, const double*) = (void (*)(double*, const double*, const double*))buf->getCode();
	proc(&result, &RFLOAT(argv[0])->value, &RFLOAT(argv[1])->value);
	VALUE r = rb_float_new(result);

	return r;
}

extern "C" 
VALUE RXbyak_mov(VALUE self, VALUE a1, VALUE a2) {
    RXbyakGenerator* buf;
    Data_Get_Struct(self, RXbyakGenerator, buf);

    switch (TYPE(a1)) {
    case T_ARRAY:
        switch (TYPE(a2)) {
        case T_ARRAY:
        case T_SYMBOL:
        case T_FIXNUM:
            rb_raise(rb_eStandardError, "not yet support");
            break;
        default:
            rb_raise(rb_eArgError, "hoge");
            break;
        }
        break;
    case T_SYMBOL:
        switch (TYPE(a2)) {
        case T_ARRAY:
        case T_SYMBOL:
            rb_raise(rb_eStandardError, "not yet support");
            break;
        case T_FIXNUM:
            buf->_mov_reg_long(rb_to_id(a1), NUM2LONG(a2));
            break;
        default:
            rb_raise(rb_eArgError, "hoge");
            break;
        }
        break;
    default:
        rb_raise(rb_eArgError, "hoge");
        break;
    }
    return Qnil;
}

extern "C" 
VALUE RXbyak_movq(VALUE self, const VALUE a1, const VALUE a2) {
    RXbyakGenerator* buf;
    Data_Get_Struct(self, RXbyakGenerator, buf);

    switch (TYPE(a1)) {
    case T_ARRAY:
        switch (TYPE(a2)) {
        case T_ARRAY:
        case T_SYMBOL:
        case T_FLOAT:
            rb_raise(rb_eStandardError, "not yet support");
            break;
        default:
            rb_raise(rb_eArgError, "hoge");
            break;
        }
        break;
    case T_SYMBOL:
        switch (TYPE(a2)) {
        case T_ARRAY:
            buf->_movq_reg_address(a1, a2);
            break;
        case T_SYMBOL:
        case T_FLOAT:
            rb_raise(rb_eStandardError, "not yet support");
            break;
        default:
            rb_raise(rb_eArgError, "hoge");
            break;
        }
        break;
    default:
        rb_raise(rb_eArgError, "hoge");
        break;
    }
    return Qnil;
}


extern "C" 
VALUE RXbyak_ret(VALUE self) {
    RXbyakGenerator* buf;
    Data_Get_Struct(self, RXbyakGenerator, buf);
    buf->_ret();
    return Qnil;
}

extern "C" 
void Init_RXbyak(void) {
	VALUE rb_cRXbyak;
	rb_cRXbyak = rb_define_class("RXbyak", rb_cObject);
	rb_define_alloc_func(rb_cRXbyak, RXbyak_alloc);

	rb_define_method(rb_cRXbyak, "mov", RB_FUNC(RXbyak_mov), 2);
	rb_define_method(rb_cRXbyak, "movq", RB_FUNC(RXbyak_movq), 2);
	rb_define_method(rb_cRXbyak, "ret", RB_FUNC(RXbyak_ret), 0);
	rb_define_method(rb_cRXbyak, "call", RB_FUNC(RXbyak_call), -1);
}

