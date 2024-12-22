#include "protocollo_udp.h"

//fungeneretanum
void generate_numeric(char *password, int length) {
    const char *digits = "0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = digits[rand() % 10];
    }
    password[length] = '\0';
}

void generate_alpha(char *password, int length) {
    const char *letters = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; i++) {
        password[i] = letters[rand() % 26];
    }
    password[length] = '\0';
}

void generate_mixed(char *password, int length) {
    const char *chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % 36];
    }
    password[length] = '\0';
}

void generate_secure(char *password, int length) {
    const char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&";
    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % 70];
    }
    password[length] = '\0';
}

void generate_unambiguous(char *password, int length) {
    const char *chars = "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnpqrstuvwxyz234679";
    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % 52];
    }
    password[length] = '\0';
}
//endfun
int main() {
#if defined WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return -1;
    }
#endif

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        CLEARWINSOCK;
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        CLOSESOCKET(server_socket);
        CLEARWINSOCK;
        return -1;
    }
    //connserver
    printf("Server listening on port %d\n", DEFAULT_PORT);
    fflush(stdout);

    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recvfrom(server_socket, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received < 0) {
            perror("Failed to receive data");
            continue;
        }
        //request
        buffer[bytes_received] = '\0';
        printf("New request from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        //scanrequest
        char type;
        int length;
        if (sscanf(buffer, "%c %d", &type, &length) != 2 || length < 6 || length > 32) {
            const char *error_message = "Invalid request\n";
            sendto(server_socket, error_message, strlen(error_message), 0,
                   (struct sockaddr *)&client_addr, client_len);
            continue;
        }
        //generatepassw
        char password[33];
        switch (type) {
            case 'n': generate_numeric(password, length); break;
            case 'a': generate_alpha(password, length); break;
            case 'm': generate_mixed(password, length); break;
            case 's': generate_secure(password, length); break;
            case 'u': generate_unambiguous(password, length); break;
            default:
                sendto(server_socket, "Invalid type\n", 13, 0,
                       (struct sockaddr *)&client_addr, client_len);
                continue;
        }
        //sendpsw
        sendto(server_socket, password, strlen(password), 0,
               (struct sockaddr *)&client_addr, client_len);
    }

    CLOSESOCKET(server_socket);
    CLEARWINSOCK;
    return 0;
}
