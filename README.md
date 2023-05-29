# BattleShip-Multiplayer
Proyecto en c++ para un battleship online, con estructura cliente-servidor que admite varios usuarios, usando el protocolo TCP/IP.
## Librerias
cstdlib
sys/types.h
netinet/in.h
netdb.h
ctime
string
vector
sys/socket.h
arpa/inet.h
unistd.h
thread
chrono
iostream
# USO
Para hacer uso del programa primero debemos compilar con lo siguiente:

## Compilar:
Ejecutar comando "make" en el directorio raiz

## Ejecutar:
Para ejecutar se debe en el directorio raiz, y usar los siguiente
Servidor:
./server puerto

Cliente:
./client puerto

## Jugar:
Los comandos admitidos para jugar son:
exit: comando para finalizar el juego por el usuario, para poder salir en cualquier instante de la partida.
coordenadas: Es un texto con una letra y un numero juntos ejemplo "A1", "B7" para indicar la coordenada de disparo.

Desarrollado por:
- Alex Felipe Tardón Bernales
- Benjamín Alejandro Machuca Villaseñor


