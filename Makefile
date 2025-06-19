#!/usr/bin/make -f
.PHONY: all clean

TARGET      = raven
SRCS        = source/raven.c
OBJS        = $(SRCS:.c=.o)

# add libspu sources
SRCS        += source/spu/libspu.c

# add libsd sources
SRCS        += source/sd/sd_cli.c
SRCS        += source/sd/sd_drv.c
SRCS        += source/sd/sd_file.c
SRCS        += source/sd/sd_ioset.c
SRCS        += source/sd/sd_main.c
SRCS        += source/sd/sd_sub1.c
SRCS        += source/sd/sd_sub2.c
SRCS        += source/sd/sd_wk.c
SRCS        += source/sd/se_tbl.c

CFLAGS      = --std=gnu99 -O2 -Wall -Wextra -Isource

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LINK.c) -o $@ $^

.SUFFIXES: .c

.c.o:
	@echo compile $<
	@$(COMPILE.c) $< -o $*.o

clean:
	-$(RM) $(OBJS)
	-$(RM) $(TARGET)
