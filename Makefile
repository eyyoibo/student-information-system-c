# =============================================================================
# Student Information System - Makefile
# =============================================================================
# Usage:
#   make          - Build the main program (sis.exe)
#   make test     - Build and run all tests
#   make clean    - Remove compiled files
#   make all      - Build both main program and tests
# =============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
INCLUDES = -I src

# Source files (in src/ directory)
SRC_DIR = src
TEST_DIR = tests

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/utils.c $(SRC_DIR)/student.c $(SRC_DIR)/professor.c \
          $(SRC_DIR)/course.c $(SRC_DIR)/enrollment.c $(SRC_DIR)/grade.c $(SRC_DIR)/menu.c $(SRC_DIR)/curriculum.c

# Test source files
TEST_SOURCES = $(TEST_DIR)/test_main.c $(SRC_DIR)/utils.c $(SRC_DIR)/student.c $(SRC_DIR)/professor.c \
               $(SRC_DIR)/course.c $(SRC_DIR)/enrollment.c $(SRC_DIR)/grade.c $(SRC_DIR)/menu.c $(SRC_DIR)/curriculum.c \
               $(TEST_DIR)/test_utils.c $(TEST_DIR)/test_student.c $(TEST_DIR)/test_professor.c \
               $(TEST_DIR)/test_course.c $(TEST_DIR)/test_enrollment.c $(TEST_DIR)/test_grade.c $(TEST_DIR)/test_menu.c

# Output executables
MAIN_EXE = sis.exe
TEST_EXE = tests.exe

# Default target: build main program
all: $(MAIN_EXE) $(TEST_EXE)

# Build main program
$(MAIN_EXE): $(SOURCES)
	$(CC) $(CFLAGS) -o $(MAIN_EXE) $(SOURCES)
	@echo ">> Main program built successfully: $(MAIN_EXE)"

# Build and run tests
test: $(TEST_EXE)
	@echo ">> Running tests..."
	./$(TEST_EXE)

$(TEST_EXE): $(TEST_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_EXE) $(TEST_SOURCES)
	@echo ">> Test executable built: $(TEST_EXE)"

# Clean compiled files
clean:
	@echo ">> Cleaning compiled files..."
	-del /Q $(MAIN_EXE) $(TEST_EXE) *.o 2>nul || rm -f $(MAIN_EXE) $(TEST_EXE) *.o
	@echo ">> Clean complete."

# Run the main program
run: $(MAIN_EXE)
	./$(MAIN_EXE)

# Phony targets (not actual files)
.PHONY: all test clean run
