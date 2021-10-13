test: test-build
	cd ./local/test && ./main

test-build: test-clean
	mkdir -p ./local/test/
	gcc -g -o ./local/test/main ./tests/main.c ./*.c
	#clang -o ./local/test/main ./tests/main.c ./*.c

test-clean:
	rm -rf ./local/test