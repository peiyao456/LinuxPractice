.PHONY:all
all: client tserver
lib=-lpthread
client:client.c
	gcc -o $@ $^ $(lib) -static
tserver:tserver.c
	gcc -o $@ $^ $(lib) -static
.PHONY:clean
clean:
	rm -f server client
