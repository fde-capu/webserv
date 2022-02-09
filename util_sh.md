## List machine IPs:
# `getent hosts`
## See all open ports?
# `netstat -tunlp`
## nginx with specific `.conf`; `-t` for testing.
# `nginx -c /home/coder/webserv/test-confs/nginx.conf -t`
## curl showing everything about the negotiation and package (request and response headers):
# `curl --verbose -D-` # or better: `-vD-`
## nginx reload (uses the same configuration file previous given by -c)
# `nginx -s reload`
