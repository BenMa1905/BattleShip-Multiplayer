#include "libraries.h"

// ! Definicion de colores para imprimir
const std::string ANSI_RESET = "\u001b[0m";
const std::string ANSI_BLACK = "\u001b[30m";
const std::string ANSI_RED = "\u001b[31m";
const std::string ANSI_GREEN = "\u001b[32m";
const std::string ANSI_YELLOW = "\u001b[33m";
const std::string ANSI_BLUE = "\u001b[34m";
const std::string ANSI_MAGENTA = "\u001b[35m";
const std::string ANSI_CYAN = "\u001b[36m";
const std::string ANSI_WHITE = "\u001b[37m";

// Disparo que el cliente le envio al servidor (client board)
char serverShot;
int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
int lastXShoot = -1;
int lastYShoot = -1;
char lastServerShot = '*'; // disparo que el servidor le envio al cliente (server board)
bool firstTurn = true;     // variable para saber si es el primer turno del cliente
bool victory = false;      // variable para saber si el cliente gano
int turnCounter = 0;       // variable para saber cuantos turnos han pasado
bool waiting = true;       // variable para saber si el cliente esta esperando a que el servidor le envie un mensaje
bool finished = false;     // variable para saber si el juego termino
int turn = 2;              // variable para saber si es el turno del cliente o del servidor
char showLastCharacter;
#include "class.h"
#include "messages.h"
#include "listener.h"
#include "game.h"

int main(int argc, char const *argv[])
{
    std::srand(static_cast<unsigned int>(std::time(NULL)));

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

    srand(time(nullptr));
    // * INICIO DEL PROGRAMA
    // Se crea el socket del cliente
    if (client_sockfd < 0)
    {
        cerr << "Error al abrir el socket del cliente" << endl;
        exit(EXIT_FAILURE);
    }

    // Se especifica la dirección del servidor
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port); // Puerto del servidor
    server_address.sin_addr.s_addr = INADDR_ANY;  // Dirección IP del servidor

    // Se establece la conexión con el servidor
    if (connect(client_sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        cerr << "Error al establecer la conexión con el servidor" << endl;
        exit(EXIT_FAILURE);
    }
    game.run();
    send_board();
    handleFirstTurn();
    std::thread serverThread(listenToServer); // Corrección aquí
    do
    {
        finished = manageGame();
    } while (finished == false);

    serverThread.join();
    if (victory)
    {
        cout << ANSI_RED << "Juego ha terminado" << endl;
        cout << "HAS PERDIDO" << endl;
        cout << "El servidor ha hundido todos tus barcos en " << ANSI_CYAN << turnCounter << ANSI_RED << " turnos" << ANSI_RESET << endl;
        exit(EXIT_SUCCESS);
    }
    else
    {
        cout << ANSI_GREEN << "Juego ha terminado" << endl;
        cout << "HAS GANADO" << endl;
        cout << "Has hundido todos los barcos del servidor en " << ANSI_CYAN << turnCounter << ANSI_GREEN << " turnos" << ANSI_RESET << endl;
        exit(EXIT_SUCCESS);
    }

    return 0;
}
