.PHONY:all
all:server client

client:client.c
	gcc -o $@ $^ -static
server:server.c
	gcc -o $@ $^ -static
.PHONY:clean
clean:
	rm -f client server
