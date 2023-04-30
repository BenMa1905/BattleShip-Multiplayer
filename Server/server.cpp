#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctime>

//#include "server.h"

using namespace std;

// Función para manejar la jugada del jugador
void handle_play(int client_sockfd) {
    // Código para manejar la jugada del jugador
    cout << "Conexion establecida con "<< client_sockfd << endl;
}

int main() {
    // Se crea el socket del servidor
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        cerr << "Error al abrir el socket del servidor" << endl;
        exit(EXIT_FAILURE);
    }

    // Se especifica la dirección del servidor
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5000); // Puerto del servidor
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Se vincula el socket a la dirección del servidor
    if (bind(server_sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        cerr << "Error al vincular el socket del servidor a la dirección" << endl;
        exit(EXIT_FAILURE);
    }

    // Se pone el socket en modo de escucha
    if (listen(server_sockfd, 5) < 0) {
        cerr << "Error al poner el socket en modo de escucha" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Servidor iniciado en el puerto " << ntohs(server_address.sin_port) << endl;

    while (true) {
        // Se acepta una conexión entrante de un cliente
        int client_sockfd = accept(server_sockfd, NULL, NULL);
        if (client_sockfd < 0) {
            cerr << "Error al aceptar la conexión entrante del cliente" << endl;
            continue;
        }

        // Se crea un nuevo proceso para manejar la jugada del jugador
        pid_t pid = fork();
        cout<<"pid de conexion: "<< pid<< endl;
        if (pid < 0) {
            cerr << "Error al crear el proceso hijo" << endl;
            close(client_sockfd);
            continue;
        } else if (pid == 0) {
            // Este es el proceso hijo
            close(server_sockfd);
            handle_play(client_sockfd);

            
            // close(client_sockfd);
            exit(EXIT_SUCCESS);
        }
    }

    close(server_sockfd);
    return 0;
}
