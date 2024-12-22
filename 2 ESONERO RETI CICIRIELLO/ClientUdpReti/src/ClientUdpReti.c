#include "protocollo_udp.h"
#include <ws2tcpip.h>

//fun msg
void print_help() {
    printf("Password Generator Help Menu\n");
    printf("Commands:\n");
    printf(" h        : show this help menu\n");
    printf(" n LENGTH : generate numeric password (digits only)\n");
    printf(" a LENGTH : generate alphabetic password (lowercase letters)\n");
    printf(" m LENGTH : generate mixed password (lowercase letters and numbers)\n");
    printf(" s LENGTH : generate secure password (uppercase, lowercase, numbers, symbols)\n");
    printf(" u LENGTH : generate unambiguous secure password (no similar-looking characters)\n");
    printf(" q        : quit application\n");
    printf("\nLENGTH must be between 6 and 32 characters\n");
}

int main() {
#if defined WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Error at WSAStartup()\n");
        return -1;
    }
#endif

    // sockt
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        CLEARWINSOCK;
        return -1;
    }

    //getaddrinfo
    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo("passwdgen.uniba.it", NULL, &hints, &res) != 0) {
        fprintf(stderr, "Error: Cannot resolve hostname passwdgen.uniba.it\n");
        CLOSESOCKET(client_socket);
        CLEARWINSOCK;
        return -1;
    }//endifgettaddr

    struct sockaddr_in *server_addr = (struct sockaddr_in *)res->ai_addr;
    server_addr->sin_port = htons(DEFAULT_PORT);

    printf("Connected to the server passwdgen.uniba.it:%d\n", DEFAULT_PORT);
    print_help();

    char buffer[BUFFER_SIZE];
    while (1) {
        printf("\nEnter request (type length) or 'h' for help or 'q' to quit: ");
        fflush(stdout);
        fgets(buffer, BUFFER_SIZE, stdin);

        if (buffer[0] == 'q') break;
        if (buffer[0] == 'h') {
            print_help();
            continue;
        }

        //sendmsg
        int bytes_sent = sendto(client_socket, buffer, strlen(buffer), 0,
                                (struct sockaddr *)server_addr, sizeof(*server_addr));
        if (bytes_sent < 0) {
            perror("Failed to send request");
            continue;
        }

        socklen_t server_len = sizeof(*server_addr);
        int bytes_received = recvfrom(client_socket, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr *)server_addr, &server_len);
        if (bytes_received < 0) {
            perror("Failed to receive response");
            continue;
        }

        buffer[bytes_received] = '\0';
        printf("Password: %s\n", buffer);
    }

    freeaddrinfo(res);
    CLOSESOCKET(client_socket);
    CLEARWINSOCK;
    printf("Disconnected\n");
    return 0;
}
