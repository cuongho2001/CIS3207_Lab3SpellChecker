spellcheck: main.c
	gcc dictionary.h dictionary.c main.c queue.h queue.c -lpthread -o spellcheck
