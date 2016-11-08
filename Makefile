CC ?= gcc
CFLAGS = -std=gnu11 -O2 
DBGFLAGS = -W -Wall -Werror -Wextra -pedantic -pedantic-errors -ggdb3 -std=gnu11

all:
	$(CC) $(CFLAGS) -o mdrgpalu src/mdrgpalu.c
debug:
	$(CC) $(DBGFLAGS) -o mdrgpalu src/mdrgpalu.c
clean:
	rm -rf mdrgpalu src/*.o

.PHONY: clean all debug