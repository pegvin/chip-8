CC:=gcc
C_FLAGS:=--std=c99 -Isrc/ -Imwindow/include/ -MMD -MP -Wall
LD_FLAGS:=
BUILD_TARGET:=debug
GEN_COMPILE_COMMANDS:=false # Make sure you have 'bear' installed, and also make sure when generating you don't use -j4 or something flags

ODIR     = build
BIN      = chip-8
SRCS_C   = $(wildcard src/*.c) mwindow/impl.c
OBJECTS  = $(SRCS_C:.c=.o)
OBJECTS := $(patsubst %,$(ODIR)/%,$(OBJECTS))
DEPENDS := $(OBJECTS:.o=.d)

ifeq ($(BUILD_TARGET),debug)
	C_FLAGS+=-O0 -g
else
	ifeq ($(BUILD_TARGET),release)
		C_FLAGS+=-O3 -fdata-sections -ffunction-sections -flto
		LD_FLAGS+=-Wl,--gc-sections -flto # on mac this is replaced with -dead_strip below.
	else
$(error Invalid Build Target: "$(BUILD_TARGET)")
	endif
endif

ifeq ($(OS),Windows_NT)
	ifeq ($(call lc,$(BUILD_TARGET)),debug)
		LD_FLAGS+=-mconsole
	else
		LD_FLAGS+=-mwindows
	endif
	C_FLAGS+=-DTARGET_WINDOWS=1
	BIN=chip-8.exe
else
	UNAME_S:=$(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		# On Linux Use Address Sanitizers in Debug Mode
		ifeq ($(BUILD_TARGET),debug)
			C_FLAGS+=-fsanitize=address -fsanitize=undefined
			LD_FLAGS+=-fsanitize=address -fsanitize=undefined
		endif
		C_FLAGS+=-DTARGET_LINUX=1
		LD_FLAGS+=-ldl -lX11 -lXi -lm
	endif
	ifeq ($(UNAME_S),Darwin)
		C_FLAGS+=-DTARGET_OSX=1
		ifeq ($(BUILD_TARGET),release)
			LD_FLAGS:=$(subst --gc-sections,-dead_strip,$(LD_FLAGS)) # replace --gc-sections with -dead_strip
		endif
	endif
endif

-include $(DEPENDS)

$(ODIR)/%.o: %.c
	@echo "CC  -" $<
	@mkdir -p "$$(dirname "$@")"
ifeq ($(GEN_COMPILE_COMMANDS),true)
	@bear --append -- $(CC) -c $< -o $@ $(C_FLAGS)
else
	@$(CC) -c $< -o $@ $(C_FLAGS)
endif

$(BIN): $(OBJECTS)
	@echo Linking $@
ifeq ($(GEN_COMPILE_COMMANDS),true)
	@bear --append -- $(CC) -o $@ $(OBJECTS) $(LD_FLAGS)
else
	@$(CC) -o $@ $(OBJECTS) $(LD_FLAGS)
endif

# make all
all: $(BIN)

.PHONY: run
.PHONY: clean

# make run
run: $(BIN)
	@./$(BIN)

# make clean
clean:
	@$(RM) -r $(BIN) $(ODIR) $(DEPENDS)
	@echo - Cleaned


