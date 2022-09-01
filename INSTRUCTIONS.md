webserv
by fde-capu

# Instructions

## Build
`make` | `webserv` and `cgi_webserv`

## Tests

Run `webserv webserv-unit.conf` or `nginx nginx-as-webserv.conf`.
`general_tests.sh` runs several tests with wichever server is running.

#
# make
# netstat -tnl					// --tcp --numeric --listening // Nothing shows.
# (ps aux)						// all, show users, x = show parents
# (pkill nginx)
#-> nginx -c webserv.conf		// Listeners opened.
# netstat -tnl					// --tcp --numeric --listening // Nothing shows.
# (ps aux)						// all, show users, x = show parents
# lynx localhost:3490
# curl localhost:3490
# curl -vD- localhost:3490		// (-vD-: -v verbose, -D- all headers
#								// to stdout) Got 301.
# cat /etc/hosts				// See: better use 127.0.0.1
# // Show conf file.
#-> export S=127.0.0.1			// Server
#-> export P=3490				// Port
# curl -vD- $S:$P
# ls confs/html					// Check index.htm (not html).
# curl -vD- $S:$P/somesub/		// Subdirectories working.
# curl -vD- $S:$P/somesub		// 301 (lack of slash "somesub/")
# curl -LvD- $S:$P/somesub		// 301->200
# curl -vD- $SP:$P -H 'Host: krazything'	// krazything exists.
# curl -vD- $SP:$P -H 'Host: unxistent'		// Defaults to first server:3490 on .conf file.



### Util commands:

## List machine IPs:
# `getent hosts`

## See all open ports?
# `netstat -tnl` --tcp --numeric --listening

## curl showing everything about the negotiation and package
## (request and response headers):
# `curl --verbose -D-` # or better: `-vD-`
