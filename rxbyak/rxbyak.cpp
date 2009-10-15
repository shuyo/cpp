#include <ruby.h>
#define RB_FUNC(f) reinterpret_cast<VALUE (*)(...)>(f)

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
	rb_define_method(rb_cRXbyak, "hoge", RB_FUNC(hoge), 0);
}

