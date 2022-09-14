webserv
by fde-capu

# Instructions

`make`
`make clean`
`ls`
	webserv: program
	cgi_webserv: simple cgi wrapper
	lynx: web browser
`curl http://localhost:3490`
`lynx http://localhost:3490`

## Tests

Run `webserv webserv-unit.conf` or `nginx nginx-as-webserv.conf`.
`general_tests.sh` runs several tests with wichever server is running.

#
# make
# netstat -tnl					// --tcp --numeric --listening // Nothing shows.
# (ps aux)						// all, show users, x = show parents
# ./webserv unit/webserv-unit.conf
# netstat -tnl					// --tcp --numeric --listening // Nothing shows.
# (ps aux)						// all, show users, x = show parents
# lynx localhost:3490
# curl localhost:3490
# curl -v localhost:3490		// -v verbose
# cat /etc/hosts				// See: better use 127.0.0.1
# // Show conf file.
#-> export S=127.0.0.1			// Server
#-> export P=3490				// Port
# curl -v $S:$P
# ls confs/html					// Check index.htm (not html).
# curl -v $S:$P/somesub/		// Subdirectories working.
# curl -v $S:$P/somesub			// 301 (lack of slash "somesub/")
# curl -Lv $S:$P/somesub		// 301->200
# curl -v $SP:$P -H 'Host: krazything'	// krazything exists.
# curl -v $SP:$P -H 'Host: unxistent'	// Defaults to first server:3490 on .conf file.

### Util commands:

## List machine IPs:
# `getent hosts`

## See all open ports?
# `netstat -tnl` --tcp --numeric --listening

## curl showing everything about the negotiation and package
## (request and response headers):
# `curl --verbose -D-` # or better: `-vD-`
