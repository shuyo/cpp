require 'RXbyak'
rx = RXbyak.new
def rx.code
  mov eax, [esp, 8]
  movq xmm0, [eax]
  mov eax, [esp, 12]
  movq xmm1, [eax]
  divsd xmm0, xmm1
  mov eax, [esp, 4]
  movq [eax], xmm0
  ret
end
rx.code

p rx.eax

puts rx.exec(256.0, 256.0)
puts rx.exec(123.45, 678.9)
