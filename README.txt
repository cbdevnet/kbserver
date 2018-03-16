The kbserver project contains tools for interfacing evdev input
devices, such as keyboards and numpads to a network, useful for 
example to abstract between multiple sources of input to a system.

This projects home is at https://github.com/cbdevnet/kbserver
Should this location change, a new one will be announced somewhere
on https://dev.cbcdn.com/

For more information on the configuration of the provided tools,
see the README file in the respective directories.

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
