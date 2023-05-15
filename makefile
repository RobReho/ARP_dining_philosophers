all: queue consumer producer diners dinerslib
#philosopher

queue: fifo_queue.h
	gcc -o queue fifo_queue.h

consumer: consumer.c
	gcc -o consumer consumer.c

producer: producer.c
	gcc -o producer producer.c

diners: diners.c
	gcc -o diners diners.c

dinerslib: diners.h
	gcc -o dinerslib diners.h

.PHONY: clean

clean:
		rm -f queue.o
		rm -f consumer.o
		rm -f producer.o
		rm -f diners.o
		rm -f dinerslib.o
	