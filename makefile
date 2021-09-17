test:
	mkdir -p ./local/
	gcc -o ./local/main.test ./tests/main.c ./*.c
	./local/main.test