all: serveur client

CFLAGS=-Wall -Wextra

serveur:
	@gcc -o ./../app/serveur $(CFLAGS) serveur.c

client:
	@gcc -o ./../app/client $(CFLAGS) client.c

clean:
	@rm -f ./../app/client ./../app/serveur