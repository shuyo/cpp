#include <ruby.h>
#define RB_FUNC(f) reinterpret_cast<VALUE (*)(...)>(f)

#include "xbyak/xbyak.h"


class RXbyakGenerator : public Xbyak::CodeGenerator {
    public:
    RXbyakGenerator() {
        mov(eax, 55);
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
VALUE RXbyak_call(VALUE self) {
    RXbyakGenerator* buf;
    Data_Get_Struct(self, RXbyakGenerator, buf);

    //int (*proc)(int) = (int (*)(int))a.getCode();
    int (*proc)() = (int (*)())buf->getCode();
	VALUE r = INT2FIX(proc());
	return r;
}

extern "C" 
VALUE hoge(VALUE self) {
	int x = 3;
	VALUE r = INT2FIX(x);
	return r;
}

extern "C" 
void Init_RXbyak(void) {
	VALUE rb_cRXbyak;
	rb_cRXbyak = rb_define_class("RXbyak", rb_cObject);
	rb_define_alloc_func(rb_cRXbyak, RXbyak_alloc);

	rb_define_method(rb_cRXbyak, "hoge", RB_FUNC(hoge), 0);
	//rb_define_method(rb_cRXbyak, "new", RB_FUNC(RXbyak_new), 0);
	rb_define_method(rb_cRXbyak, "call", RB_FUNC(RXbyak_call), 0);
}

