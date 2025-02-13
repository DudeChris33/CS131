# FILES := Lab1_stack_base.cpp
# FILES := Lab1_stack_mutex.cpp
FILES := Lab1_stack_cas.cpp
OUTPUT_FILE := output.o
ARGS := -Wall -std=c++11 -pthread
THREAD_NUM := 16

build: $(FILES)
	g++ $(ARGS) $(FILES) -o $(OUTPUT_FILE)

run: build
	./$(OUTPUT_FILE) $(THREAD_NUM)

build_datarace: $(FILES)
	g++ $(ARGS) $(FILES) -o $(OUTPUT_FILE) -fsanitize=thread -fPIE -pie

test_datarace: build_datarace
	./$(OUTPUT_FILE) $(THREAD_NUM)

clean: $(OUTPUT_FILE)
	rm -f $(OUTPUT_FILE)