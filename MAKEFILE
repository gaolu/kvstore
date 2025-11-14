CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpthread

all: server client

server: server.c kv_store.c thread_pool.c
	$(CC) $(CFLAGS) -o server server.c kv_store.c thread_pool.c $(LDFLAGS)

client: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f server client
