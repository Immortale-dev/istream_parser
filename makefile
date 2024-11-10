# v1.0.1
.PHONY: test custom lib force clear

CC=g++
OPT=-g
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c++20
SRCPATH:=src/
TEST_SRCPATH:=test/src/
SRCS:=$(wildcard $(SRCPATH)*.cpp)
TEST_SRCS:=$(wildcard $(TEST_SRCPATH)*.cpp)
OBJS:=$(SRCS:%.cpp=%.o)
HDRS:=$(wildcard $(SRCPATH)*.h)
HDPP:=$(wildcard $(SRCPATH)*.hpp)

LIBS=$(wildcard inc/*)
LIBS_A:=$(addsuffix /liboutput.a,$(LIBS))

# Creates a list of all modules include flags
modulesinc=$(foreach d,$(wildcard $(1:=/inc/*)),$(call modulesinc,$d) -I$(d:=/src))
MOD_INC:=$(call modulesinc,.)

# Include LIBS flags
flagsinc=$(foreach d,$(wildcard $(1:=/inc/*)),$(call flagsinc,$d) $(d:=/makefile.flags))
-include $(call flagsinc,.)
-include makefile.flags

LDFLAGS:=$(LIBS_LD)
INCL=-Isrc -Itest -Iinc $(LIBS_INC) $(MOD_INC)

test: EXTRACFLAGS:=$(EXTRA_TF)
test: test.exe

custom: mtest.exe

lib: liboutput.a

clear:
	$(foreach mod,$(LIBS),$(MAKE) -C $(mod) clear $(newline))
	$(RM) src/*.o *.exe liboutput.a test/*.o test/src/*.o

liboutput.a: $(LIBS_A) $(OBJS) $(HDRS) $(HDPP)
	$(RM) liboutput.a
	ar -qcT liboutput.a $(LIBS_A) $(OBJS)

$(LIBS_A): force
	$(MAKE) -C $(dir $@) CFLAGS="$(CFLAGS)" OPT="$(OPT)" lib

test.exe: liboutput.a test/test.o
	$(CC) -o test.exe test/test.o liboutput.a $(LDFLAGS)

mtest.exe: liboutput.a test/mtest.o
	$(CC) -o mtest.exe test/mtest.o liboutput.a $(LDFLAGS)

test/test.o: $(TEST_SRCS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $< -o $@ $(INCL) $(OPT)

define newline


endef
