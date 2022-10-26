webserv
by fde-capu

# Instructions

# netstat -tnl					// --tcp --numeric --listening // Nothing shows.
# (ps aux)						// all, show users, x = show parents

`make webserv`
`make lynx`
`ls`
	webserv: program
	lynx: web browser

`curl http://localhost:3490`
`lynx http://localhost:3490`

`make siege`
`siege -b -c 42 http://127.0.0.1:3490/empty_page`

## Tests

Run `webserv unit/webserv-unit.conf`.
`general_tests.sh` runs several tests with wichever server is running.

# ./webserv unit/webserv-unit.conf
# netstat -tnl					// --tcp --numeric --listening
# (ps aux)						// all, show users, x = show parents
# lynx localhost:3490
# curl localhost:3490
# curl -v localhost:3490		// -v verbose
# cat /etc/hosts				// See: better use 127.0.0.1
# // Configuration file: unit/webserv-unit.conf
# export S=127.0.0.1			// Server
# export P=3490					// Port
# curl -v $S:$P
# ls confs/html					// Check index.htm (not html).
# // Procees to unit/general_tests.sh

### Util commands reference:

## List machine IPs:
# `getent hosts`

## See all open ports?
# `netstat -tnl` --tcp --numeric --listening

## curl showing all about the negotiation and package
## (request and response headers):
# `curl --verbose -D-` # or better: `-vD-`
