all:
	$(GCC_PREFIX)gcc -o mdrgpalu -Wall -ggdb mdrgpalu.c
clean:
	rm -f mdrgpalu
