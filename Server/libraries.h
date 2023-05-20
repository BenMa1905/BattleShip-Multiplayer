#include <iostream>          // Para std::cout, std::endl
#include <string>            // Para std::string
#include <vector>            // Para std::vector
#include <unistd.h>          // Para close()
#include <sys/socket.h>      // Para socket(), bind(), listen(), accept(), connect()
#include <netinet/in.h>      // Para sockaddr_in
#include <arpa/inet.h>       // Para htons(), inet_pton()
#include <netdb.h>           // Para getaddrinfo(), freeaddrinfo()
