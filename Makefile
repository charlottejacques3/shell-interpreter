all: ssi inf args

ssi: ssi.c linked_list.h linked_list
	gcc ssi.c -lreadline -lhistory -ltermcap linked_list -o ssi 

inf: inf.c
	gcc inf.c -o inf

args: args.c
	gcc args.c -o args

linked_list: linked_list.c
	gcc -c linked_list.c -o linked_list

clean:
	rm -f *.o
	rm -f ssi
	rm -f inf
	rm -f args
	rm -f linked_list
