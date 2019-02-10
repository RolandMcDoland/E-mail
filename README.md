# E-mail
E-mail client (Java + JavaFX) and server for Linux based systems (C) - university project. The comments are written in Polish.
## General overview
### Client
The user types in the **username**. This opens a window with three **lists**: 
* **sent messages**
* **received messages**
* **contacts**
Clicking on any of the list positions will cause the **details** window to pop up. There are also three **actions** the user can choose to take, by pressing the buttons:
* **send/receive**- this option will cause all the e-mails to the current user to be received and put in the received list
* **new message**- this option will cause the new window to pop up, which after filling up the form and confirmation will send a new e-mail
* **new contact**- this option will cause the new window to pop up, which after filling up the form and confirmation will add a new contact
### Server
A **multi-threaded** program in C. One thread is waiting for new clients and allows them to **log on**. The other is iterating on the list of clients and first **recieves** the messages from every client and **processes** them, then **sends** messages to every client.
#### Compilation
gcc -pthread threadedServer.c -o a.out
## Technologies
* **TCP sockets**
* **JavaFX**
* **pthread**
## Authors
* **Mikołaj Frankowski** - [RolandMcDoland](https://github.com/RolandMcDoland)
* **Krzysztof Pasiewicz** - [Nebizar](https://github.com/Nebizar)
