# 2-Stage-Communication-Protocol
2-stage communication protocol is designed between server and client, where concurrent server accepts connection from multiple clients and serves them concurrently based on both TCP and UDP connection.

## Steps to run:
1. open terminal from folder in linux
2. run command "make all" without quotes
3. run ./server.o <port_no>
4. open new terminal from same folder
5. run client.o <server_ip> <server_port>
6. run command "make clean" without quotes to remove compiled files
