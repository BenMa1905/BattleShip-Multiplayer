// Dependencias del juego
#include "libraries.h"
#include "game.h"
// #include "server.h"

using namespace std;

// Función para manejar la jugada del jugador
void handle_play(int client_sockfd)
{
    // Código para manejar la jugada del jugador
    std::cout << "Conexion establecida con: " << client_sockfd << std::endl;
}

// funcion para controlar el usuario
void handle_user(Player player)
{
    // inicializar el tablero
    Board player_board;
    Board server_board = Board().createRandomBoard();

    // enviar un mensaje de respuesta
    string response;

    // El primer mensaje que se recibe es el tablero del jugador
    char buffer[1024];
    int bytes = recv(player.socket_fd, buffer, sizeof(buffer), 0);
    string message = string(buffer, bytes);

    // convertir el texto a un tablero
    player_board = textToBoard(message);

    // imprimir el tablero del servidor
    // std::cout << "Tablero del servidor " << player.id << std::endl;
    // server_board.printBoard();

    // definicion de variables para el juego
    Game game(player.id, player, player_board, server_board);

    // se determina quien empieza 0 jugador, 1 servidor
    int turn = rand() % 2;

    // enviar un mensaje de respuesta
    response = to_string(turn);
    send(player.socket_fd, response.c_str(), response.length(), 0);

    Position server_shot = randomPosition();
    char server_result;
    // se comprueba si el servidor empieza
    if (turn == 1)
    {
        // se imprime quien empieza
        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Empieza el servidor" << std::endl;
        // se envia un disparo aleatorio
        response = "*," + to_string(server_shot.x) + "," + to_string(server_shot.y) + "\0";
        send(player.socket_fd, response.c_str(), response.length(), 0);

        char server_result = player_board.checkShot(server_shot.x, server_shot.y); // esta funcion modifica el tablero del servidor

        // se comprueba si el jugador le atino a algun barco del servidor
        if (server_result != 'X' && server_result != 'O')
        {
            Ship ship = server_board.getShip(server_shot.x, server_shot.y);
            if (ship.name != "Agua")
            {
                // se comprueba si el jugador hundio algun barco del servidor
                if (!ship.isSunk())
                {
                    // si no se hundio se imprime que le pego al barco
                    std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del jugador toca " << ship.name << std::endl;
                }
                else
                {
                    // si se hundio se imprime que hundio el barco
                    std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del jugador hunde " << ship.name << std::endl;
                }
            }
        }
    }
    else
    {
        // se imprime quien empieza
        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Empieza el cliente" << std::endl;
    }

    while (true)
    {
        char buffer[1024];
        int bytes = recv(player.socket_fd, buffer, sizeof(buffer), 0);
        if (bytes < 0)
        {
            std::cerr << "Error al recibir el mensaje de: " << player.socket_fd << " pid: " << player.id << std::endl;
            exit(EXIT_FAILURE);
            close(player.socket_fd); // TODO: comprobar si es necesario cerrar el socket
        }

        // procesar el mensaje
        string message = string(buffer, bytes);
        std::cout << "\nMensaje recibido de " << player.socket_fd << " pid: " << player.id << "\nMensaje: " << message << std::endl;
        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Mensaje recibido " << message << std::endl;

        // cerrar la conexion si el cliente envia "exit"
        if (message == "exit")
        {
            break;
        }

        // los mensajes recibidos son: "letra,x,y" o "exit"
        // la letra indica el tipo de mensaje o el resultado de la jugada
        // x,y son las coordenadas de la jugada.
        // fallo: X
        // acierto: [letra del barco]
        // gana servidor: D
        // gana jugador: V
        // primer turno: *
        // error: E

        // procesar el mensaje
        std::vector<std::string> tokens = split(message, ',');
        // comprobar si el mensaje es valido
        bool is_valid = is_message_valid(tokens, player.socket_fd);
        if (!is_valid)
        {
            continue;
        }

        // procesar el mensaje
        char letter = tokens[0][0];
        int x = stoi(tokens[1]);
        int y = stoi(tokens[2]);

        std::cout << "Letra: " << letter << " x: " << x << " y: " << y << std::endl;
        // generar posicion de disparo del servidor
        server_shot = randomPosition();
        char server_result;
        // TODO: procesar el mensaje

        if (letter == '*')
        {
            // std::cout << "Turno del jugador" << std::endl;
            // significa que es el primer turno del jugador
            // por lo tanto hay que verificar si el jugador le atino a algun barco
            // y enviar la respuesta al cliente del disparo del servidor
            char player_result = server_board.checkShot(x, y); // esta funcion modifica el tablero del servidor

            // se comprueba si el jugador le atino a algun barco del servidor
            if (player_result != 'X' && player_result != 'O')
            {
                Ship ship = server_board.getShip(x, y);
                if (ship.name != "Agua")
                {
                    // se comprueba si el jugador hundio algun barco del servidor
                    if (!ship.isSunk())
                    {
                        // si no se hundio se imprime que le pego al barco
                        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del jugador toca " << ship.name << std::endl;
                    }
                    else
                    {
                        // si se hundio se imprime que hundio el barco
                        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del jugador hunde " << ship.name << std::endl;
                    }
                }
            }
            // enviar la respuesta al cliente, junto con el ataque del servidor
            // con el formato de "resultadoDisparo,x,y" con x e y las coordenadas del disparo del servidor
            response = string(1, player_result) + "," + to_string(server_shot.x) + "," + to_string(server_shot.y) + "\0";

            send(player.socket_fd, response.c_str(), response.length(), 0);

            // se comprueba si el servidor le atino a algun barco del jugador
            server_result = player_board.checkShot(server_shot.x, server_shot.y);
            // std::cout << "Disparo del servidor: " << server_result << " x: " << server_shot.x << " y: " << server_shot.y << std::endl;
            std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del servidor x: " << server_shot.x << " y: " << server_shot.y << std::endl;

            // se comprueba si el servidor le atino a algun barco del servidor
            if (server_result != 'X' && server_result != 'O')
            {
                Ship ship = server_board.getShip(x, y);
                if (ship.name != "Agua")
                {
                    // se comprueba si el servidor hundio algun barco del servidor
                    if (!ship.isSunk())
                    {
                        // si no se hundio se imprime que le pego al barco
                        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del jugador toca " << ship.name << std::endl;
                    }
                    else
                    {
                        // si se hundio se imprime que hundio el barco
                        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del jugador hunde " << ship.name << std::endl;
                    }
                }
            }
        }
        else
        {
            // std::cout << "Turno del turno server" << std::endl;
            // hay que comprobar si es cierto segun el tablero del jugador que el server tiene
            // almacenado, de no coincidr los resultados, se envia un mensaje de error al cliente
            // y se cierra la conexion
            if (server_result != letter && letter != 'X' && letter != 'O')
            {
                // enviar un mensaje de error
                std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Error en el mensaje recibido conexion finalizada  letter:" << server_result << std::endl;
                response = "E,-1,-1";
                send(player.socket_fd, response.c_str(), response.length(), 0);
                // close(player.socket_fd);
                // exit(EXIT_FAILURE);
                break;
            }

            char player_result = server_board.checkShot(x, y); // esta funcion modifica el tablero del servidor

            // se comprueba si el jugador le atino a algun barco del servidor
            if (player_result != 'X' && player_result != 'O')
            {
                Ship ship = server_board.getShip(x, y);
                if (ship.name != "Agua")
                {
                    // se comprueba si el jugador hundio algun barco del servidor
                    if (!ship.isSunk())
                    {
                        // si no se hundio se imprime que le pego al barco
                        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del jugador toca " << ship.name << std::endl;
                    }
                    else
                    {
                        // si se hundio se imprime que hundio el barco
                        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del jugador hunde " << ship.name << std::endl;
                    }
                }
            }

            // se comprueba si el servidor gano
            if (player_board.checkGameOver())
            {
                // enviar un mensaje de victoria al cliente
                std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Gana cliente" << std::endl;
                response = "D,-1,-1";
                send(player.socket_fd, response.c_str(), response.length(), 0);
                break;
                close(player.socket_fd);
                exit(EXIT_SUCCESS);
            }

            // se comprueba si el jugador gano
            if (server_board.checkGameOver())
            {
                // enviar un mensaje de victoria al cliente
                std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Gana servidor" << std::endl;
                response = "V,-1,-1";
                send(player.socket_fd, response.c_str(), response.length(), 0);
                break;
                close(player.socket_fd);
                exit(EXIT_SUCCESS);
            }

            // enviar la respuesta al cliente, junto con el ataque del servidor
            // con el formato de "resultadoDisparo,x,y" con x e y las coordenadas del disparo del servidor
            response = string(1, player_result) + "," + to_string(server_shot.x) + "," + to_string(server_shot.y) + "\0";
            send(player.socket_fd, response.c_str(), response.length(), 0);

            // se comprueba si el servidor le atino a algun barco del jugador
            server_result = player_board.checkShot(server_shot.x, server_shot.y);
            // std::cout << "Disparo del servidor: " << server_result << " x: " << server_shot.x << " y: " << server_shot.y << std::endl;
            std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del servidor x: " << server_shot.x << " y: " << server_shot.y << std::endl;

            // se comprueba si el servidor le atino a algun barco del servidor
            if (server_result != 'X' && server_result != 'O')
            {
                Ship ship = server_board.getShip(x, y);
                if (ship.name != "Agua")
                {
                    // se comprueba si el servidor hundio algun barco del servidor
                    if (!ship.isSunk())
                    {
                        // si no se hundio se imprime que le pego al barco
                        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del servidor toca " << ship.name << std::endl;
                    }
                    else
                    {
                        // si se hundio se imprime que hundio el barco
                        std::cout << "Juego [" << player.client_ip << ":" << player.client_port << "]: Disparo del servidor hunde " << ship.name << std::endl;
                    }
                }
            }
        }

        // imprimir los tableros (para debug)
        // std::cout << "Tablero del jugador" << std::endl;
        // player_board.printBoard();
        // std::cout << "Tablero del servidor" << std::endl;
        // server_board.printBoard();
    }
    close(player.socket_fd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
    std::srand(std::time(nullptr));

    // se revisa si se ingreso el puerto por parametro
    int server_port;

    if (argc < 2)
    {
        cerr << "Uso: ./server <puerto>" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        server_port = atoi(argv[1]);
    }

    // Se crea el socket del servidor

    // * está creando un socket del tipo TCP/IP
    // *(conexión orientada a la transmisión de datos)
    // * utilizando direcciones IPv4 y el protocolo predeterminado.
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0)
    {
        cerr << "Error al crear el socket del servidor" << endl;
        exit(EXIT_FAILURE);
    }

    // Se configura la dirección del servidor
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Se vincula el socket a la dirección del servidor
    if (bind(server_sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        cerr << "Error al vincular el socket del servidor a la dirección" << endl;
        exit(EXIT_FAILURE);
    }

    // Se pone el socket en modo de escucha
    if (listen(server_sockfd, 5) < 0)
    {
        cerr << "Error al poner el socket en modo de escucha" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Esperando conexiones..." << endl;

    cout << "Servidor iniciado en el puerto " << ntohs(server_address.sin_port) << endl;

    while (true)
    {
        // Se acepta una conexión entrante de un cliente
        int client_sockfd = accept(server_sockfd, NULL, NULL);
        if (client_sockfd < 0)
        {
            cerr << "Error al aceptar la conexión entrante del cliente" << endl;
            continue;
        }

        // Obtiene la información de la dirección del cliente
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        getpeername(client_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

        // Convierte la dirección IP a una cadena legible
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

        // Obtiene el número de puerto del cliente
        int client_port = ntohs(client_addr.sin_port);

        // Imprime la dirección IP y el puerto del cliente
        cout << "Juego nuevo [" << client_ip << ":" << client_port << "]" << endl;

        // Se crea un nuevo proceso para manejar la jugada del jugador
        pid_t pid = fork();
        // cout << "pid de conexion: " << pid << endl;

        // se guarda la informacion del cliente en la clase
        Player player(pid, client_sockfd, client_ip, client_port);
        if (pid < 0)
        {
            cerr << "Error al crear el proceso hijo" << endl;
            close(client_sockfd);
            continue;
        }
        else if (pid == 0)
        {
            // Este es el proceso hijo
            close(server_sockfd);

            handle_user(player);

            // close(client_sockfd);
            // exit(EXIT_SUCCESS);
        }
    }

    close(server_sockfd);
    return 0;
}