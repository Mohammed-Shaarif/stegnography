all: 
	gcc -Wall -Werror -Wextra -pedantic -std=c99 -g -o test_encode test_encode.c encode.c

test:
	gcc -o test_encode test_encode.c encode.c

rm:
	rm -f test_encode

test_encode: test_encode.c encode.c types.c
	gcc -Wall -Werror -Wextra -pedantic -std=c99 -g -o test_encode test_encode.c encode.c


check:
	hd beautiful.bmp | head -5;echo "----"; hd output.bmp | head -5


run:
	make test;echo "Running test_encode with sample arguments";./test_encode -e beautiful.bmp secret.txt output.bmp;echo "Running test_encode with sample arguments";echo "compare";make check	
