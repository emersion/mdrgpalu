CC ?= gcc
CFLAGS = -std=gnu11 -O2
DBGFLAGS = -W -Wall -Werror -Wextra -pedantic -pedantic-errors -ggdb3 -std=gnu11
PREFIX = /usr/local

all:
	$(CC) $(CFLAGS) -o mdrgpalu src/mdrgpalu.c
debug:
	$(CC) $(DBGFLAGS) -o mdrgpalu src/mdrgpalu.c
clean:
	rm -rf mdrgpalu src/*.o
install:
	install -m 0755 mdrgpalu $(DESTDIR)$(PREFIX)/bin

.PHONY: clean all debug install
