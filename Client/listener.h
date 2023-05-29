void updateGlobalVariables(const char *response)
{
    int x, y;
    char serverShot;

    sscanf(response, "%c,%d,%d", &serverShot, &x, &y);
    if (y > 14)
    {
        y = y / 10;
    }

    lastXShoot = x;
    lastYShoot = y;
    lastServerShot = serverShot;
}

void manageMessageReceived(char *response)
{

    updateGlobalVariables(response);
    showLastCharacter = response[0];
    char letterAux = 'A' + lastXShoot;

    switch (showLastCharacter)
    {
    case 'X':
    case 'O':
        cout << ANSI_RED << "¡Disparaste a (" << ANSI_CYAN << 'A' + globalShot.getCoordX() << "," << globalShot.getCoordY() + 1 << ")" << ANSI_RED << " y has fallado!" << ANSI_RESET << endl;
        break;
    case 'P':
        cout << ANSI_RED << "¡Le has dado a un Portaaviones enemigo!" << ANSI_RESET << endl;
        break;
    case 'B':
        cout << ANSI_RED << "¡Le has dado a un Buque enemigo!" << ANSI_RESET << endl;
        break;
    case 'L':
        cout << ANSI_RED << "¡Le has dado a una Lancha enemiga!" << ANSI_RESET << endl;
        break;
    case 'S':
        cout << ANSI_RED << "¡Le has dado a un Submarino enemigo!" << ANSI_RESET << endl;
        break;
    case 'D':
        cout << ANSI_RED << "¡Has perdido!" << ANSI_RESET << endl;
        victory = false;
        finished = true;
        break;
    case 'V':
        cout << ANSI_RED << "¡Has ganado!" << ANSI_RESET << endl;
        finished = true;
        victory = true;
        break;
    case 'F':
        cout << ANSI_RED << "Saliendo ..." << ANSI_RESET << endl;
        exit(EXIT_SUCCESS);
        break;
    case 'E':
        cout << ANSI_RED << "ERROR" << ANSI_RESET << endl;
        exit(EXIT_FAILURE);
        break;
    case '*':
        cout << ANSI_RED << "¡El servidor te ha disparado a (" << ANSI_CYAN << letterAux << "," << lastYShoot + 1 << ")" << ANSI_RED << " y has fallado!" << ANSI_RESET << endl;
        break;
    default:
        cout << ANSI_RED << "El servidor ha mandado un carácter inválido." << ANSI_RESET << endl;
        break;
    }
    cout << "----------------------------------------------------------------------------------------------------" << endl;
    serverShot = game.updateClientCell(lastXShoot, lastYShoot);

    game.updateServerCell(globalShot.getCoordX(), globalShot.getCoordY(), lastServerShot);
    game.printBoards();
}

void listenToServer()
{
    while (true)
    {
        char response[1024];
        int bytes = recv(client_sockfd, response, sizeof(response), 0);
        if (bytes < 0)
        {
            cerr << "Error al recibir el resultado del servidor" << endl;
            exit(EXIT_FAILURE);
        }
        else
        {
            waiting = false;
            manageMessageReceived(response);
            memset(response, 0, sizeof(response));
        }
    }
}
