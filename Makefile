RAYLIB := raylib
RAYGUI := raygui
RAYLIB_INC := $(RAYLIB)/include
RAYGUI_INC := $(RAYGUI)/include
RAYLIB_LIB := $(RAYLIB)/lib/libraylib.a

APP   := audio-player
FLAGS := -Wall -Wextra -s -I $(RAYLIB_INC) -isystem $(RAYGUI_INC)

SRC_DIR   := src
BUILD_DIR := build
BIN_DIR   := bin
OUTPUT    := $(BIN_DIR)/$(APP)

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: release

release: FLAGS += -O2
release: $(OUTPUT)

debug: FLAGS += -g -O0
debug: $(OUTPUT)

$(OUTPUT): $(OBJS)
	@mkdir -p $(BIN_DIR)
	g++ $(FLAGS) -o $@ $^ $(RAYLIB_LIB)
	@if [ "$(FLAGS)" != "-g -O0" ]; then strip $@; fi

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	g++ $(FLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	$(MAKE) -C $(RAYLIB_INC) clean
