#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>


// funciones
int find_port(int initial_port);

int find_port(int initial_port){
    int port = initial_port;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error al crear el socket del servidor" << std::endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port=htons(port);

    // intenta vincular el socket a la direccion del servidor
    int bind_result = bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
    while (bind_result < 0)
    {
        port++;
        server_address.sin_port = htons(port);
        bind_result = bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
    }

    // cuando se logra vincular el socket a la direccion del servidor, se cierra el socket
    // y se retorna el puerto para abrir el socket del servidor en el main
    close(sockfd);
    return port;
}