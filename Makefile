EE_BIN = triangle.elf
EE_OBJS = triangle.o
EE_LIBS = -ldma -lgraph -lkernel

all: $(EE_BIN)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
