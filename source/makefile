all: 
	gcc -Wall -Werror -Wextra -pedantic -std=c99 -g -o test_encode test_encode.c encode.c

test:
	gcc -o test_encode test_encode.c encode.c

rm:
	rm -f test_encode

test_encode: test_encode.c encode.c types.c
	gcc -Wall -Werror -Wextra -pedantic -std=c99 -g -o test_encode test_encode.c encode.c

