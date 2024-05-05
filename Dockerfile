FROM gcc:latest

COPY . /usr/src/socket_server
WORKDIR /usr/src/socket_server

RUN gcc -o server server.c data_storage.c -lpthread

CMD ["./server"]
