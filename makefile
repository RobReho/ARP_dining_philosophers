all: queue consumer producer diners diners_lib 
#philosopher

queue: fifo_queue.h
	gcc -o queue fifo_queue.h

consumer: consumer.c
	gcc -o con consumer.c -lrt

producer: producer.c
	gcc -o pro producer.c -lrt

diners: diners.c
	gcc -o diners diners.c -lrt

diners_lib: diners.h
	gcc -o diners_lib diners.h

.PHONY: clean

clean:
		rm -f queue.o
		rm -f con.o
		rm -f pro.o
		rm -f diners.o
		rm -f diners_lib.o
