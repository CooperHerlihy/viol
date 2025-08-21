all: viol

viol: src/*.c src/*.h
	gcc -Werror -Wall -Wextra -pedantic -Wconversion -Wshadow -Wunreachable-code -Wformat=2 -fsanitize=thread -fsanitize=undefined -std=c23 -g -O0 -o build/viol src/*.c

clean:
	rm -rf build
