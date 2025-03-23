all: proyecto
proyecto: main.c
	@gcc -pthread -o proyecto main.c
clean:
	rm -f proyecto