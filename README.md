# ARP_dining_philosophers
Dining philosophers exercise solved in c with pipes and sockets for the Advanced and Robots Programming course

# MAIN branch - unnamed pipes

## Short description

A server hosts a dining room for a maximum of 5 dining philosophers.

- the server receives commands from two processes, each one in a separate shell, the Produces and the Consumer.These processes are able to communicate to the server through unnamed pipes and signals.

- you can ask the Produces and the Consumer to:

a. make a philosopher enter the dining room, sit and eat for a random amount of time;

b. make a philsopher stop eating and exiting the room.

Philosophers are enqued on a fifo basis upon entering.

The server has its own shell in which it writes its status (philosophers sitting and eating, philosophers thinking).


## Compiling and execution

    The folder contains a makefile through which it is possible to compile all the files with the command:
    ```bash
    make
    ```
    After compiling, it is sufficient to execute the main file:
    ```bash
    ./diners
    ```

# SOCKET branch
## Short description

A server hosts a dining room for a maximum of 5 dining philosophers.

- the server receives commands from two processes executing indipendently: the Produces and the Consumer. These processes are able to communicate to the server through sockets, thus they can be executed on different machines.

- you can ask the Produces and the Consumer to:

a. make a philosopher enter the dining room, sit and eat for a random amount of time;

b. make a philsopher stop eating and exiting the room.

Philosophers are enqued on a fifo basis upon entering.

The server has its own shell in which it writes its status (philosophers sitting and eating, philosophers thinking).


## Compiling and execution

    In case the socket's client and server are executed on the same machine using the local host ip:
	    Execute the makefile, which compiles all the files with the command:
	    ```bash
	    make
	    ```
	    
	    After compiling, it is sufficient to execute the server file with the desired port to use for the socket communication:
	    ```bash
	    ./diners <port_no>
	    ```

	    In two different shells the clients producer.c and consumer.c must be executed separately along with the local host ip and the same port used by the server.
 	    ```bash
	    ./producer localhost <port_no>
	    ```
	    ```bash
	    ./consumer localhost <port_no>
	    ```
    
    In case the socket's client and server are executed on the different machines:
	    Execute the makefile both on the server's and clients' machines with the command:
	    ```bash
	    make
	    ```
	    
	    After compiling, it is sufficient to execute the server file with the desired port to use for the socket communication:
	    ```bash
	    ./diners <port_no>
	    ```

	    On the different machine the clients producer.c and consumer.c must be executed in two different shells along with the server's machine ip and the same port used by the server.
 	    ```bash
	    ./producer <server_ip> <port_no>
	    ```
	    ```bash
	    ./consumer <server_ip> <port_no>

	The clients could be execuded on different machines as well with a simular procedure.

	N.B. The program has only been tested on a local host configuration setup.

