all: 
	gcc -o main main.c encode.c decode.c common.c

rm:
	rm -f main;rm -f output.bmp

check:
	hd beautiful.bmp | head -5;echo "----"; hd output.bmp | head -5


encode: 
	./main -e beautiful.bmp secret.txt output.bmp

decode:
	./main -d output.bmp decoded_secret.txt; cat decoded_secret.txt

