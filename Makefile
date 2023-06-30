CC:=gcc
C_FLAGS:=--std=c99 -Isrc/ -MMD -MP -Wall -DLOG_USE_COLOR=1
LD_FLAGS:=

SDL2_STATIC_LINK:=0
SDL2_LDFLAGS:=
SDL2_CFLAGS:=-DSDL_MAIN_HANDLED=1
BUILD_TARGET:=debug

ODIR     = build
BIN      = chip-8
SRCS_C   = $(wildcard src/*.c) $(wildcard src/**/*.c) $(wildcard src/**/**/*.c)
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
	ifeq ($(SDL2_STATIC_LINK),1)
		SDL2_LDFLAGS+=-static-libstdc++ -Wl,-Bstatic -lSDL2main -lSDL2 -Wl,-Bdynamic
	else
		SDL2_LDFLAGS+=-lSDL2main -lSDL2
	endif

	LD_FLAGS+=-lopengl32
	SDL2_LDFLAGS+=$(addprefix -l,winmm gdi32 imm32 ole32 oleaut32 shell32 version uuid setupapi)
	ifeq ($(call lc,$(BUILD_TARGET)),debug)
		LD_FLAGS+=-mconsole
	else
		LD_FLAGS+=-mwindows
	endif
	BIN=chip-8.exe
else
	UNAME_S:=$(shell uname -s)
	_libs:=m pthread

	ifeq ($(UNAME_S),Linux)
		ifeq ($(SDL2_STATIC_LINK),1)
			SDL2_LDFLAGS+=-Wl,-Bstatic -lSDL2 -Wl,-Bdynamic -lX11 -lXext -lXi -lXfixes -lXrandr -lXcursor
		else
			SDL2_LDFLAGS+=-lSDL2
		endif

		_libs+=dl
		# On Linux Use Address Sanitizers in Debug Mode
		ifeq ($(BUILD_TARGET),debug)
			C_FLAGS+=-fsanitize=address -fsanitize=undefined
			LD_FLAGS+=-fsanitize=address -fsanitize=undefined
		endif
	endif
	ifeq ($(UNAME_S),Darwin)
		ifeq ($(BUILD_TARGET),release)
			LD_FLAGS:=$(subst --gc-sections,-dead_strip,$(LD_FLAGS)) # replace --gc-sections with -dead_strip
		endif
		LD_FLAGS+=$(addprefix -framework , OpenGL Cocoa) -lz
		SDL2_LDFLAGS:=-lSDL2
	endif

	LD_FLAGS+=$(addprefix -l,$(_libs))
endif

-include $(DEPENDS)

$(ODIR)/%.o: %.c
	@echo "CC  -" $<
	@mkdir -p "$$(dirname "$@")"
	@$(CC) -c $< -o $@ $(C_FLAGS) $(SDL2_CFLAGS)

$(BIN): $(OBJECTS)
	@echo Linking $@
	@$(CC) -o $@ $(OBJECTS) $(LD_FLAGS) $(SDL2_LDFLAGS)

# make all SDL2_STATIC_LINK=0
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


