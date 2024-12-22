#ifndef PROTOCOLLO_UDP_H
#define PROTOCOLLO_UDP_H

#if defined WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;  // Compatibilità con Windows
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Default configuration
#define DEFAULT_PORT 57015
#define DEFAULT_IP "127.0.0.1"
#define BUFFER_SIZE 1024

#if defined WIN32
#define CLOSESOCKET closesocket
#define CLEARWINSOCK WSACleanup()
#else
#define CLOSESOCKET close
#define CLEARWINSOCK
#endif

// Prototipi delle funzioni di generazione password
void generate_numeric(char *password, int length);
void generate_alpha(char *password, int length);
void generate_mixed(char *password, int length);
void generate_secure(char *password, int length);
void generate_unambiguous(char *password, int length);

#endif // PROTOCOLLO_UDP_H
