Build prerequistes
------------------
	- A Linux system (needs linux/input.h)

To compile, run make.

Config file format
------------------
Configuration for kbserver instances in stored in config files,
containing one directive per line.
A # at the beginning of a line makes that line a comment.

Valid directives:
	device /dev/input/<devicenode>
		Path to the evdev input to read from
	bind <host|ip>
		IP or hostname to bind to
	port <port>
		Port to listen on
	send_raw <true|false>
		If a key is not mapped, resort to sending
		the raw 16-bit scancode
	exclusive <true|false>
		Request exclusive access to the device
	map <scancode> <stringspec>
		Map a key scancode to a string to be output

The stringspec format used for defining mappings allows for
the use of arbitrary bytes such as newlines and other control
sequences by treating all characters between single quotes (')
as literal strings and everything else as ascii-encoded byte numerals.
Example: 'Hello to the' 32 'World!' 13 10
C equivalent: "Hello to the World!\r\n"

Command line options
--------------------
The following options are recognized by kbserver:
	-f <configfile>
		Config file to use for this instance
	-v[v[v[v]]]
		Set debug verbosity
		0: nothing
		1: errors
		2: warnings
		3: informational
		4: debug
