Qt 5.15.2
C++ 14
Boost 1.67
GCC 8.3+
Artra Linux 1.7

start: ./http_server <address> <port> <doc root>
exm: 127.0.0.0 8080 /home/user/http-virtual-recorder

send request: curl http:://127.0.0.0:8080/recordings (get)
curl -X DELETE http://127.0.0.0:8080/recordings/id (delete)
