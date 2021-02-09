
SHELL			= /bin/bash
PROJECT_NAME		= CronuSwim
PROJECT_PATH		= $(shell pwd)
HOME_PATH		= $(shell echo ~)

BIN_PATH		= bin
BIN_RELEASE_PATH	= $(BIN_PATH)/release
BIN_TEST_PATH		= $(BIN_PATH)/test

MAIN_RELEASE_FILE	= $(BIN_RELEASE_PATH)/$(PROJECT_NAME).ino.elf
MAIN_TEST_FILE		= $(BIN_TEST_PATH)/$(PROJECT_NAME)_test

BUILD_PATH		= build
BUILD_TEST_PATH		= $(BUILD_PATH)/test_obj

VPATH			= src test

SRC_FILES		= $(wildcard src/*)
ALL_CPP			= $(notdir $(wildcard */*.cpp))
ALL_OBJ			= $(ALL_CPP:%.cpp=$(BUILD_TEST_PATH)/%.o)
DEP			= $(ALL_OBJ:.o=.d)

EMLEARN_DIR		= $(shell echo $(HOME_PATH)/.local/lib/python3.*/site-packages/emlearn/)

CPPFLAGS_TEST		= -Isrc -I$(EMLEARN_DIR)
CXXFLAGS_TEST		= -Wall -Wextra -Werror

ARDUINO_COMPILER_FLAGS	= -Isrc -I$(EMLEARN_DIR)
ARDUINO_FQBN		= arduino:mbed:nano33ble
ARDUINO_PORT		= /dev/ttyACM*

# ------------------------------------------------------------------------------

.PHONY : all

all: test upload

# ------------------------------------------------------------------------------

.PHONY : compile upload test

compile: $(MAIN_RELEASE_FILE)

# compile the code if necessary
upload: $(MAIN_RELEASE_FILE)
	arduino-cli upload --port $(ARDUINO_PORT) --fqbn $(ARDUINO_FQBN) --input-dir $(BIN_RELEASE_PATH) -v

test: $(MAIN_TEST_FILE)
	./$(MAIN_TEST_FILE)
	
	
$(MAIN_RELEASE_FILE): $(SRC_FILES) $(PROJECT_NAME).ino
	arduino-cli compile \
	--warnings all \
	--build-property compiler.cpp.extra_flags="$(ARDUINO_COMPILER_FLAGS)" \
	--build-path $(PROJECT_PATH)/$(BUILD_PATH) \
	--fqbn $(ARDUINO_FQBN) $(PROJECT_NAME).ino \
	--output-dir $(BIN_RELEASE_PATH)

$(MAIN_TEST_FILE): $(ALL_OBJ) | $(BIN_TEST_PATH)
	$(CXX) $(CXXFLAGS_TEST) -o $(MAIN_TEST_FILE) $(ALL_OBJ)
$(BUILD_TEST_PATH)/%.o: %.cpp $(BUILD_TEST_PATH)/%.d | $(BUILD_TEST_PATH)
	$(CXX) $(CPPFLAGS_TEST) $(CXXFLAGS_TEST) -MT $@ -MMD -MP -MF $(BUILD_TEST_PATH)/$*.d -c -o $@ $<
$(BUILD_TEST_PATH): ; @mkdir -p $@
$(BIN_TEST_PATH): ; @mkdir -p $@

$(DEP):

include $(wildcard $(DEP))


# ------------------------------------------------------------------------------
	
.PHONY : project_name

project_name:
	@echo "$(PROJECT_NAME)"

# ------------------------------------------------------------------------------

.PHONY : clean

clean: 
	rm -rf $(BUILD_PATH)/ $(BIN_PATH)/

