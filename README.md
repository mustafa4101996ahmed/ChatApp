# ChatApp
Multi-threaded Client-Server Chat App on Linux

This is a chat application that allows communication between multiple clients. The server needs
to be launched first, and can be started using the command

## ./server

Clients can then be launched with the following command

## ./client localhost 8000

and it will connect to the server. After a username is selected, the chat can continue as usual.
All clients will receive messages from all other clients. Chat can proceed as it normally does. A
participant can hit Ctrl+C to exit the chat, and the same command can be used to close the
server.