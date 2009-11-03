require "mkmf"
if RUBY_PLATFORM=~/mswin/
  $CFLAGS = "-EHsc -DWIN32_LEAN_AND_MEAN -Zi"
  #$CFLAGS += " /D_MSC_VER=1200"
else
  $CFLAGS = "-g -fno-operator-names"
  $LDFLAGS = "-lstdc++"
end
create_makefile("RXbyak")

