##
# map_entire_file
#
CC = gcc
WINCC = x86_64-w64-mingw32-gcc
CFLAGS = -g -Wall -Wextra -Wswitch-enum
TARGETS = main.out main.exe
SRC = example.c map_files.h
TEST_FILE = ./test_file.txt

all: $(TARGETS)

main.out: $(SRC)
	$(CC) $(CFLAGS) -o main.out ./example.c

main.exe: $(SRC)
	$(WINCC) $(CFLAGS) -o main.exe ./example.c

clean:
	rm -rfv $(TARGETS)
run: all
	@echo "Running Linux binary:"
	./main.out $(TEST_FILE)
	@echo ""
	@echo "Running Windows binary with Wine:"
	wine ./main.exe $(TEST_FILE) || echo "Wine not installed or failed to run the Windows binary"
# end
