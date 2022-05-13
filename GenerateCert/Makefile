all: ca-cert server-cert client-cert

ca-cert:
	openssl genrsa -out ca.key 2048
	openssl req -x509 -sha256 -new -nodes -key ca.key -days 365 -out ca.crt

server-cert:
	openssl genrsa -out server.key 2048
	openssl req -new -key server.key -out server.csr
	openssl x509 -sha256 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 365

client-cert:
	openssl genrsa -out client.key 2048
	openssl req -new -key client.key -out client.csr
	openssl x509 -sha256 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 365


