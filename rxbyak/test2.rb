require 'RXbyak'

rx = RXbyak.new
def rx.code
  mov ecx, [esp, 4]
  mov eax, 0
L:loop
  add eax, ecx
  sub ecx, 1
  jne :loop
  ret
end
rx.code

def sumup(x)
  s = 0
  x.times {|i| s+=i }
  s
end

N = 100000
T = 100
t1 = Time.now.to_f
T.times do
  z = sumup(N)
end
t2 = Time.now.to_f
T.times do
  z = rx.int_call(N)
end
t3 = Time.now.to_f

puts "ruby  : #{t2-t1} (sec)"
puts "rxbyak: #{t3-t2} (sec)"

