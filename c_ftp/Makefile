clientSrc=comm.c client.c
serverSrc=comm.c server.c
cli=client
ser=server
cc=gcc

.PHONY:all
all:$(ser) $(cli)

$(cli):$(clientSrc)
	$(cc) -o $@ $^
$(ser):$(serverSrc)
	$(cc) -o $@ $^

.PHONY:clean
clean:
	rm -f $(cli) $(ser) *.o

