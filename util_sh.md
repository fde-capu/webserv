#
# make
# (make clean)
#-> source source-me-after-install-rc
# netstat -tnl					// --tcp --numeric --listening // Nothing shows.
# (ps aux)						// all, show users, x = show parents
# (pkill nginx)
#-> nginx -c webserv.conf			// Listeners opened.
# netstat -tnl					// --tcp --numeric --listening // Nothing shows.
# (ps aux)						// all, show users, x = show parents
# lynx localhost:3490
# curl localhost:3490
# curl -vD- localhost:3490		// (-vD-: -v verbose, -D- all headers to stdout) Got 301.
# cat /etc/hosts				// See: better use 127.0.0.1
#-> export S=127.0.0.1			// Server
#-> export P=3490				// Port
# curl -vD- $S:$P
# ls confs/html					// Check index.htm (not html).
# curl -vD- $S:$P/somesub/		// Subdirectories working.
# curl -vD- $S:$P/somesub		// 301 (lack of slash "somesub/")
# curl -LvD- $S:$P/somesub		// 301->200
# curl -vD- $SP:$P -H 'Host: krazything'	// krazything exists.
# curl -vD- $SP:$P -H 'Host: unxistent'		// Defaults to first server:3490 on .conf file.

#
#

## List machine IPs:
# `getent hosts`

## See all open ports?
# `netstat -tnl` --tcp --numeric --listening

## nginx with specific `.conf`; `-t` for testing.
# `nginx -c /home/coder/webserv/confs/nginx.conf -t`

## curl showing everything about the negotiation and package (request and response headers):
# `curl --verbose -D-` # or better: `-vD-`

## nginx reload (uses the same configuration file previous given by -c)
# `nginx -s reload`
