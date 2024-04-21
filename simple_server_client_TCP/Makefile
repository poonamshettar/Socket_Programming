x=gcc
OBJS=server client
all:$(OBJS)
server:server.c
	${x} $^ -o $@
client:client.c
	${x} $^ -o $@
clean:
	rm -rf $(OBJS)
