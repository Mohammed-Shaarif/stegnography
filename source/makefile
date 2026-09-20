all: 
	gcc -Wall -Werror -Wextra -pedantic -std=c99 -g -o test_encode main.c encode.c decode.c common.c

test:
	gcc -o main main.c encode.c decode.c common.c

rm:
	rm -f test_encode;rm -f main;rm -f output.bmp

check:
	hd beautiful.bmp | head -5;echo "----"; hd output.bmp | head -5


run:
	make test;echo "Running test_encode with sample arguments";./main -e beautiful.bmp secret.txt output.bmp;echo "Running test_encode with sample arguments";echo "compare";make check;make rm	

checktest:
	./main -e beautiful.bmp secret.txt output.bmp