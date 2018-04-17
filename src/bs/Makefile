# Makefile
# bushaofeng. 2011.8.27
# ver=1.0.0

OUTPUT		= ./output
OUTPUT_INC	= $(OUTPUT)/include
OUTPUT_LIB	= $(OUTPUT)/lib

INC_PATH	= output/include
LIB_PATH	= output/lib

include $(sys).mk

INCLDIR		= 	./

FPIC            =       -fPIC
WARN_LEVL       =       -Wall
FDEBUG          =       -g
SHARED			=		-shared
#STD_MODE		=		-std=c99
STD_MODE		=		-std=gnu99

EXTRA_OPT       =       $(FDEBUG)
EXTRA_OPT       +=      $(WARN_LEVL)
EXTRA_OPT       +=      $(FPIC)
EXTRA_OPT       +=      $(STD_MODE)
#EXTRA_OPT		+=		$(SHARED)
#EXTRA_OPT		+=		$(STD_MODE)

OBJDIR = ./
SRCS = $(wildcard ./*.c)
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS)) 


TARGET=libbs.a
$(TARGET):$(OBJS)
	mkdir -p $(OUTPUT_INC)
	mkdir -p $(OUTPUT_LIB)
	echo $(OBJS)
	ar -r libbs.a $(OBJS)
	cp -p *.h $(OUTPUT_INC)
	cp -p *.a $(OUTPUT_LIB)

$(OBJS):%.o: %.c
	$(CC) -c $(LDFLAGS) $(EXTRA_OPT) -I$(INCLDIR) $^ -o $@

clean:
	rm -rf $(OUTPUT) *.a *.o
