##
# map_entire_file
#
CC = gcc
WINCC = x86_64-w64-mingw32-gcc
CFLAGS = -g -Wall -Wextra -Wswitch-enum
TARGETS = main main.exe

all: $(TARGETS)

main: main.c
	$(CC) $(CFLAGS) -o main ./main.c

main.exe: main.c
	$(WINCC) $(CFLAGS) -o main.exe ./main.c

clean:
	rm -rfv $(TARGETS)
run: all
	@echo "Running Linux binary:"
	./main
	@echo ""
	@echo "Running Windows binary with Wine:"
	wine ./main.exe || echo "Wine not installed or failed to run the Windows binary"
# end
