#!/usr/bin/make -f
.PHONY: all clean

# add encoder sources
SRCS        += source/encoder/flac.c
SRCS        += source/encoder/opus.c

# add spu sources
SRCS        += source/spu/spu.c

# add libsd sources
SRCS        += source/sd/sd_cli.c
SRCS        += source/sd/sd_drv.c
SRCS        += source/sd/sd_file.c
SRCS        += source/sd/sd_ioset.c
SRCS        += source/sd/sd_main.c
SRCS        += source/sd/sd_mstr.c
SRCS        += source/sd/sd_sub1.c
SRCS        += source/sd/sd_sub2.c
SRCS        += source/sd/sd_wk.c

# get paths from pkg-config
OPUS_INCDIR := $(shell pkg-config --cflags opus)

CFLAGS      = --std=gnu99 -g -O2 -Wall -Wextra -Wshadow -Isource -Isource/lib $(OPUS_INCDIR)
LDFLAGS     = -lFLAC -lopusenc

RAVEN       = raven
RAVEN_SRCS  = source/raven.c
RAVEN_SRCS  += $(SRCS)
RAVEN_OBJS  = $(RAVEN_SRCS:.c=.o)

PLAYSE      = playse
PLAYSE_SRCS = source/playse.c
PLAYSE_SRCS += $(SRCS)
PLAYSE_OBJS = $(PLAYSE_SRCS:.c=.o)

all: $(RAVEN) $(PLAYSE)

$(RAVEN): $(RAVEN_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(PLAYSE): $(PLAYSE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	-$(RM) $(RAVEN_OBJS)
	-$(RM) $(PLAYSE_OBJS)
	-$(RM) $(RAVEN)
	-$(RM) $(PLAYSE)
