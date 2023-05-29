#include "libraries.h"

std::string transformArray(const char *array)
{

    char letter = array[0];
    int number = std::stoi(array + 1);

    std::string result = ",";
    result += std::to_string(letter - 'A');
    result += ",";
    result += std::to_string(number - 1);
    globalShot.setCoords(letter - 'A', number - 1);
    return result;
}

bool isValidInput(const string &input)
{
    if (input.length() < 2 || input.length() > 4)
    {
        return false;
    }

    char letter = input[0];
    int number = 0;
    try
    {
        number = stoi(input.substr(1));
    }
    catch (const invalid_argument &)
    {
        return false;
    }

    if (letter < 'A' || letter > 'O')
    {
        return false;
    }

    if (number < 1 || number > 15)
    {
        return false;
    }

    return true;
}

void handleFirstTurn()
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
        if (response[0] == '0') // empieza el cliente
        {
            do
            {
                string input;
                cin >> input;

                if (input == "exit")
                {
                    cout << ANSI_RED << "Juego finalizado." << ANSI_RESET << endl;
                    send(client_sockfd, "E,-1,-1", sizeof("E,-1,-1"), 0);
                    close(client_sockfd); // Cerrar el socket del cliente
                    exit(EXIT_SUCCESS);
                }

                if (!isValidInput(input))
                {
                    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
                    cout << ANSI_RED << "Entrada inválida. Por favor, ingresa una letra de la " << ANSI_RESET << ANSI_CYAN << "A" << ANSI_RESET << ANSI_RED << " a la " << ANSI_RESET << ANSI_CYAN << "O" << ANSI_RESET << ANSI_RED << " seguida de un número del " << ANSI_RESET << ANSI_CYAN << "1" << ANSI_RESET << ANSI_RED << " al " << ANSI_RESET << ANSI_CYAN << "15" << ANSI_RESET << ANSI_RED << "." << endl;
                    cout << ANSI_RED << "(ejemplo: " << ANSI_RESET << ANSI_CYAN << "A1" << ANSI_RESET << ANSI_RED << ", " << ANSI_RESET << ANSI_CYAN << "J13" << ANSI_RESET << ANSI_RED << ")." << ANSI_RESET << endl;
                    continue; // Repetir el bucle
                }

                string transformed = transformArray(input.c_str());
                sendShot(transformed);
                break; // Salir del bucle si el input es válido

            } while (true);
        }
        else if (response[0] == '1') // empieza el servidor
        {
            firstTurn = false;
            int bytes = recv(client_sockfd, response, sizeof(response), 0);
            cout << response << endl;
            if (bytes < 0)
            {
                cerr << "Error al recibir el resultado del servidor" << endl;
                exit(EXIT_FAILURE);
            }
            else
            {
                updateGlobalVariables(response);
                char letterAux = 'A' + lastXShoot;

                switch (response[0])
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
                    cout << ANSI_RED << "¡El servidor te ha disparado a (" << ANSI_CYAN << letterAux << "," << lastYShoot + 1 << ANSI_RED << ")" << " y ha fallado!" << ANSI_RESET << endl;
                    break;
                default:
                    cout << ANSI_RED << "El servidor ha mandado un carácter inválido." << ANSI_RESET << endl;
                    break;
                }
                cout << "----------------------------------------------------------------------------------------------------" << endl;
                game.printBoards();
            }
        }
        firstTurn = false;
    }
}

bool manageGame()
{
    int theTurn = 0;
    std::string input;

    do
    {
        turnCounter++;
        if (theTurn == 0)
        {
            do
            {
                string input;
                cin >> input;

                if (input == "exit")
                {
                    cout << ANSI_RED << "Juego finalizado." << ANSI_RESET << endl;
                    send(client_sockfd, "E,-1,-1", sizeof("E,-1,-1"), 0);
                    close(client_sockfd); // Cerrar el socket del cliente
                    exit(EXIT_SUCCESS);
                    return 0;
                }

                if (!isValidInput(input))
                {
                    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
                    cout << ANSI_RED << "Entrada inválida. Por favor, ingresa una letra de la " << ANSI_RESET << ANSI_CYAN << "A" << ANSI_RESET << ANSI_RED << " a la " << ANSI_RESET << ANSI_CYAN << "O" << ANSI_RESET << ANSI_RED << " seguida de un número del " << ANSI_RESET << ANSI_CYAN << "1" << ANSI_RESET << ANSI_RED << " al " << ANSI_RESET << ANSI_CYAN << "15" << ANSI_RESET << ANSI_RED << "." << endl;
                    cout << ANSI_RED << "(ejemplo: " << ANSI_RESET << ANSI_CYAN << "A1" << ANSI_RESET << ANSI_RED << ", " << ANSI_RESET << ANSI_CYAN << "J13" << ANSI_RESET << ANSI_RED << ")." << ANSI_RESET << endl;
                    continue; // Repetir el bucle
                }

                string transformed = transformArray(input.c_str());
                sendShot(transformed);
                theTurn = 1;
                break; // Salir del bucle si el input es válido

            } while (true);
        }
        else if (theTurn == 1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            theTurn = 0;
            game.printBoards();
        }

        if (finished)
        {
            return true;
        }

    } while (theTurn != 2 || !finished);

    return false;
}
