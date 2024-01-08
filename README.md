# UDP Server

Project completed during the fourth session of my software development program.

## Instructions

You are required to implement a server application that listens on a UDP port and responds to requests made to it. The server should be able to respond to a client (provided) whether the client makes requests from the same computer (127.0.0.1) or from a remote computer connected by an IP network.

### Ping Command
- Client command: `ping`
- Server response: `pong`

### Echo Command
- Client command: `echo Here is a message`
- Server response: `Here is a message`

### Date Command
- Client command: `date`
- Server response: `Mon Apr 15 23:50:47 2013`

### User Command
- Client command: `user`
- Server response: `username`

### Exec Command
- Client command: `exec ver`
- Server response: `Microsoft Windows [Version 10.0.10586]`

### Bye Command
- Client command: `bye`
- Server response: (none)

### Operation on Linux
You must make your code portable (using `#ifdef` / `#endif` if necessary and/or other relevant tricks) so that it compiles and works without modifications under Linux as well.
