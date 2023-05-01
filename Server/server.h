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
std::vector<std::string> split(std::string str, char delimiter);
bool is_message_valid(std::vector<std::string> tokens);

/**
 * funcion que comprueba si un mensaje es valido
 * @param message mensaje a comprobar
 * @return true si es valido, false si no
 */
bool is_message_valid(std::vector<std::string> tokens, int client_sockfd)
{
    if (tokens.size() != 3)
    {
        std::cerr << "El mensaje debe tener 3 tokens" << std::endl;
        // si el mensaje no tiene 3 tokens, no es valido
        // se solicita al cliente que envie un mensaje valido
        std::string response = "E,-1,-1";
        send(client_sockfd, response.c_str(), response.length(), 0);
        return false;
    }

    // se comprueba que el primer token sea una letra
    if (tokens[0].length() != 1)
    {
        std::cerr << "El primer token debe ser una letra" << std::endl;
        // si el primer token no es una letra, no es valido
        // se solicita al cliente que envie un mensaje valido
        std::string response = "R,-1,-1";
        send(client_sockfd, response.c_str(), response.length(), 0);
        return false;
    }

    return true;
}

/**
 * toma un mensaje y lo divide en un vector de strings
 * separados por el delimitador
 * @param str mensaje a dividir
 * @param delimiter delimitador
 * @return vector de strings
 */
std::vector<std::string> split(std::string str, char delimiter)
{
    std::vector<std::string> result;
    std::string word = "";
    for (auto x : str)
    {
        if (x == delimiter)
        {
            result.push_back(word);
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    result.push_back(word);
    return result;
}

int find_port(int initial_port)
{
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
    server_address.sin_port = htons(port);

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