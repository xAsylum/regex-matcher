.PHONY: help all clean commands

FLAGS := -Wall -I.
CXX=g++-15 -std=c++20

SRC:=C_Application
BUILD:=Build
EXEC:=parser

SOURCES := $(SRC)/main.cpp

MK_FILES = $(shell find $(SRC) -type f -name '*.mk')
-include $(MK_FILES)

OBJECTS = $(patsubst %.cpp,$(BUILD)/%.o,$(SOURCES))

help:
	@echo "Available targets:"
	@echo "  all   - Build the $(EXEC) executable (default)"
	@echo "  clean - Remove the $(BUILD) directory and executable"
	@echo "  commands - Create compile_commands.json for clangd"

all: $(EXEC)
	
$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD) $(EXEC)

commands:
	make clean
	bear -- make all