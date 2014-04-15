The kbserver project contains tools for interfacing evdev input
devices, such as keyboards and numpads to a network, useful for 
example to abstract between multiple sources of input to a system.

The projects latest development version can be checked out via
git from http://git.services.cbcdn.com/kbserver
Tagged releases are archived at http://dev.cbcdn.com/kbserver/

This repository contains the following tools:

kbserver
--------
kbserver reads events from input devices (if asked to,
it will also request exclusive access to them) and serves
configurable strings on a socket upon keypress.

kbactiond
---------
kbactiond accepts input via listening and/or client sockets
and builds/executes commands based on configurable tokens
