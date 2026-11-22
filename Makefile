all: ssi

ssi: ssi.c linked_list.h linked_list
	gcc -g ssi.c -lreadline -lhistory -ltermcap linked_list -o ssi 

linked_list: linked_list.c
	gcc -c linked_list.c -o linked_list

clean:
	rm -f *.o
	rm -f ssi
	rm -f linked_list
