kbserver
--------
kbserver reads events from input devices (if asked to,
it will also request exclusive access to them) and serves
configurable strings on a socket upon keypress.

kbactiond
---------
kbactiond accepts input via listening and/or client sockets
and builds/executes commands based on configurable tokens
