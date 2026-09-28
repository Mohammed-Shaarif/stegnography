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


#test cases
# ☐ Main help menu works.
# ☐ Invalid operation is handled.
# ☐ Encoding help menu works.
# ☐ Encoding works with default output filename.
# ☐ Encoding works with user-provided .bmp filename.
# ☐ Invalid output extension is handled.
# ☐ Decoding help menu works.
# ☐ Decoding works with default output filename.
# ☐ Decoding works with user-provided filename.
# ☐ Missing source files are handled.
# ☐ Invalid image extensions are handled.
# ☐ Insufficient image capacity is handled.
# ☐ Generated BMP file is valid and not corrupted.
# ☐ Decoded file is generated correctly.
# ☐ Original and decoded file contents match.

test1:
	./main -h;echo "----";sleep 3;
test2:
	./main -x;echo "----";sleep 3;
test3:
	./main -e;echo "----";sleep 3;
test4:
	./main -e beautiful.bmp secret.txt;echo "----";sleep 3;
test5:
	./main -e beautiful.bmp secret.txt output.bmp;echo "----";sleep 3
test6:
	./main -e beautiful.bmp secret.txt output.png;echo "----";sleep 3
test7:
	./main -d;echo "----";sleep 3;
test8:
	./main -d output.bmp;echo "----";sleep 3;
test9:
	./main -d output.bmp decoded_secret.txt;echo "----";sleep 3;
test10:
	./main -e demo.bmp secret.txt output.bmp;echo "----";sleep 3
test11:
	cat decoded_secret.txt;echo "----";cat secret.txt;echo "----";sleep 3;


rmall:
	rm -f main;rm -f output.bmp;rm -f decoded_secret.txt
