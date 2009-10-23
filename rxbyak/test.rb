require 'RXbyak'
rx = RXbyak.new
rx.mov :eax, [:esp, 8]
rx.movq :xmm0, [:eax]
rx.mov :eax, [:esp, 12]
rx.movq :xmm1, [:eax]
rx.divsd :xmm0, :xmm1
rx.mov :eax, [:esp, 4]
rx.movq [:eax], :xmm0
rx.ret

puts rx.call(256.0, 256.0)
puts rx.call(123.45, 678.9)
