#include "class.h"

void send_board()
{
    string board_string = game.boardToString(game.clientBoard);
    cout << ANSI_YELLOW << "Enviando el tablero al servidor..." << ANSI_RESET << endl;
    std::string message = board_string;
    if (send(client_sockfd, message.c_str(), message.length(), 0) < 0)
    {
        cerr << "Error al enviar el tablero del cliente" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << ANSI_GREEN << "Tablero enviado al servidor" << ANSI_RESET << endl;
    }
}

void sendShot(string shot)
{

    if (firstTurn)
    {
        std::string message = "*" + shot;
        if (send(client_sockfd, message.c_str(), message.length(), 0) < 0)
        {
            cerr << "Error al enviar el disparo del cliente" << endl;
            exit(EXIT_FAILURE);
        }
        firstTurn = false;
    }
    else
    {
        std::string message = serverShot + shot;
        if (send(client_sockfd, message.c_str(), message.length(), 0) < 0)
        {
            cerr << "Error al enviar el disparo del cliente" << endl;
            exit(EXIT_FAILURE);
        }
    }
}
