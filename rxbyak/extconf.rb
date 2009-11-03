require "mkmf"
if RUBY_PLATFORM=~/mswin/
  $CFLAGS = "/EHsc /D_MSC_VER=1200 /DWIN32_LEAN_AND_MEAN /MT"
else
  $CFLAGS = "-g -fno-operator-names"
end
$LDFLAGS = "-lstdc++"
create_makefile("RXbyak")

