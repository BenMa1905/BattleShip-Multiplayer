// Dependencias del juego
#include "libraries.h"
#include "game.h"
// #include "server.h"





using namespace std;

// Función para manejar la jugada del jugador
void handle_play(int client_sockfd)
{
    // Código para manejar la jugada del jugador
    cout << "Conexion establecida con: " << client_sockfd << endl;
}

// funcion para controlar el usuario
void handle_user(int client_sockfd, pid_t pid)

{
    // definicion del usuario
    Player player(pid, client_sockfd);

    // inicializar el tablero
    Board player_board;
    Board server_board = Board().createRandomBoard();

    // enviar un mensaje de respuesta
    string response = "inicializando juego\n";
    send(client_sockfd, response.c_str(), response.length(), 0);

    // El primer mensaje que se recibe es el tablero del jugador
    char buffer[1024];
    int bytes = recv(client_sockfd, buffer, sizeof(buffer), 0);
    string message = string(buffer, bytes);

    // convertir el texto a un tablero
    player_board = textToBoard(message);

    // imprimir el tablero del servidor
    cout << "Tablero del servidor " << pid << endl;
    server_board.printBoard();

    // definicion de variables para el juego
    Game game(pid, player, player_board, server_board);

    // se determina quien empieza 0 jugador, 1 servidor
    int turn = rand() % 2;

    // enviar un mensaje de respuesta
    response = to_string(turn) + "\n";
    send(client_sockfd, response.c_str(), response.length(), 0);

    Position server_shot = randomPosition();
    char server_result;
    // se comprueba si el jugador empieza
    if (turn == 1)
    {
        // se envia un disparo aleatorio
        response =  "*," + to_string(server_shot.x) + "," + to_string(server_shot.y) + "\n";
        send(client_sockfd, response.c_str(), response.length(), 0);
    }

    while (true)
    {
        char buffer[1024];
        int bytes = recv(client_sockfd, buffer, sizeof(buffer), 0);

        if (bytes < 0)
        {
            cerr << "Error al recibir el mensaje de: " << client_sockfd << " pid: " << pid << endl;
            exit(EXIT_FAILURE);
            close(client_sockfd); // TODO: comprobar si es necesario cerrar el socket
        }
        

        // procesar el mensaje
        string message = string(buffer, bytes);
        cout << "\nMensaje recibido de " << client_sockfd << " pid: " << pid << "\nMensaje: " << message << endl;

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
        bool is_valid = is_message_valid(tokens, client_sockfd);
        if (!is_valid)
        {
            continue;
        }

        // procesar el mensaje
        char letter = tokens[0][0];
        int x = stoi(tokens[1]);
        int y = stoi(tokens[2]);

        cout << "Letra: " << letter << " x: "<< x << " y: "<< y << endl;
        // generar posicion de disparo del servidor
        server_shot = randomPosition();
        char server_result;
        // TODO: procesar el mensaje


        if(letter == '*'){
            cout << "Turno del jugador" << endl;
            // significa que es el primer turno del jugador
            // por lo tanto hay que verificar si el jugador le atino a algun barco
            // y enviar la respuesta al cliente del disparo del servidor
            char player_result = server_board.checkShot( x, y); // esta funcion modifica el tablero del servidor

            // enviar la respuesta al cliente, junto con el ataque del servidor
            // con el formato de "resultadoDisparo,x,y" con x e y las coordenadas del disparo del servidor
            response = string(1, player_result) + "," + to_string(server_shot.x) + "," + to_string(server_shot.y) + "\n";
            send(client_sockfd, response.c_str(), response.length(), 0);

            // se comprueba si el servidor le atino a algun barco del jugador
            server_result = player_board.checkShot(server_shot.x, server_shot.y);
            cout << "Disparo del servidor: " << server_result << " x: " << server_shot.x << " y: "<< server_shot.y<< endl;
        }else{
            cout << "Turno del turno server" << endl;
            // hay que comprobar si es cierto segun el tablero del jugador que el server tiene
            // almacenado, de no coincidr los resultados, se envia un mensaje de error al cliente
            // y se cierra la conexion
            if(server_result != letter && letter != 'X'){
                // enviar un mensaje de error
                response = "E,-1,-1\n";
                send(client_sockfd, response.c_str(), response.length(), 0);
                // close(client_sockfd);
                // exit(EXIT_FAILURE);
                break;
            }

            char player_result = server_board.checkShot( x, y); // esta funcion modifica el tablero del servidor

            // se comprueba si el servidor gano
            if (player_board.checkGameOver())
            {
                // enviar un mensaje de victoria al cliente
                response = "D,-1,-1\n";
                send(client_sockfd, response.c_str(), response.length(), 0);
                break;
                close(client_sockfd);
                exit(EXIT_SUCCESS);
            }

            // se comprueba si el jugador gano
            if (server_board.checkGameOver())
            {
                // enviar un mensaje de victoria al cliente
                response = "V,-1,-1\n";
                send(client_sockfd, response.c_str(), response.length(), 0);
                break;
                close(client_sockfd);
                exit(EXIT_SUCCESS);
            }
            
            // enviar la respuesta al cliente, junto con el ataque del servidor
            // con el formato de "resultadoDisparo,x,y" con x e y las coordenadas del disparo del servidor
            response = string(1,player_result) + "," + to_string(server_shot.x) + "," + to_string(server_shot.y) + "\n";
            send(client_sockfd, response.c_str(), response.length(), 0);

            // se comprueba si el servidor le atino a algun barco del jugador
            server_result = player_board.checkShot( server_shot.x, server_shot.y);
            cout << "Disparo del servidor: " << server_result << " x: " << server_shot.x << " y: "<< server_shot.y<< endl;
        }

        // imprimir los tableros
        cout << "Tablero del jugador" << endl;
        player_board.printBoard();
        cout << "Tablero del servidor" << endl;
        server_board.printBoard();
        
    }
    close(client_sockfd);
    exit(EXIT_SUCCESS);
}

int main()
{
    std::srand(std::time(nullptr));
    // Se crea el socket del servidor
    int server_port = find_port(5000);

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

        // Se crea un nuevo proceso para manejar la jugada del jugador
        pid_t pid = fork();
        cout << "pid de conexion: " << pid << endl;
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

            handle_user(client_sockfd, pid);

            // close(client_sockfd);
            // exit(EXIT_SUCCESS);
        }
    }

    close(server_sockfd);
    return 0;
}