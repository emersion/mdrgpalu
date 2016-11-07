all:
	$(GCC_PREFIX)gcc -W -Wall -Werror -Wextra -pedantic -pedantic-errors -std=gnu11 -o mdrgpalu -ggdb mdrgpalu.c
clean:
	rm -f mdrgpalu
