About kbactiond
---------------
kbactiond listens or connects to multiple network input sources
in order to build and execute local commands. This is especially
useful for quickly setting up control interfaces using off-the-shelf
input hardware such as USB numpads.

Build prerequisites
-------------------
	- A POSIX-compliant system 

To compile, run make.

Config file format
------------------
Configuration for kbactiond instances is stored in config files,
containing one directive per line.
A # at the beginning of a line makes that line a comment.

Valid directives:
	connect <host|ip> <port>
		Connect to a server emitting data
	listen <host|ip> <port>
		Bind and listen on a port
	timeout <seconds>
		Timeout after which to clear the input buffer
	token <token:stringspec>,<commandpart:stringspec>,<tokentype>
		Define a token to listen for

The stringspec format used for defining mappings allows for
the use of arbitrary bytes such as newlines and other control
sequences by treating all characters between single quotes (')
as literal strings and everything else as ascii-encoded byte numerals.
Example: 'Hello to the' 32 'World!' 13 10
C equivalent: "Hello to the World!\r\n"

Valid token types:
	START
		Start a new command buffer with the commandpart, clearing 
		any existing command buffer contents
	APPEND
		Append the commandpart to the processing buffer
		or start a new command buffer if there is none
	PARAM
		Add a parameter to an existing command
		No-op if no active command buffer
	DO
		Execute the current command buffer
	EXEC
		Directly execute the commandpart, not modifying
		the current command buffer

Command line options
--------------------
The following options are recognized by kbactiond:
	-f <configfile>
		Config file to use for this instance
	-v[v[v[v]]]
		Set debug verbosity
		0: nothing
		1: errors
		2: warnings
		3: informational
		4: debug
