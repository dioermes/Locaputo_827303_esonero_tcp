#include "protocol.h"
#include <ctype.h>

void usage(const char* progname) {
    printf("Uso: %s [-s server] [-p port] -r \"type city\"\n", progname);
}

int main(int argc, char* argv[]) {
#if defined WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(1,1), &wsa);
#endif

    char server_ip[64] = "127.0.0.1";
    int port = SERVER_PORT;
    weather_request_t req = {0};

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            strcpy(server_ip, argv[++i]);
        }
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {

            if (sscanf(argv[++i], "%c %63s", &req.type, req.city) != 2) {
                usage(argv[0]);
                return 1;
            }


            for (int k = 0; req.city[k]; k++)
                req.city[k] = tolower((unsigned char)req.city[k]);


            req.city[0] = toupper((unsigned char)req.city[0]);
            // -------------------------------
        }
    }

    if (req.type == 0 || strlen(req.city) == 0) {
        usage(argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

#if defined WIN32
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
#else
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
#endif

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connessione fallita");
        return 1;
    }

    // invio della richiesta
    send(sock, (const char*)&req, sizeof(req), 0);

    // ricezione della risposta
    weather_response_t resp;
    int n = recv(sock, (char*)&resp, sizeof(resp), 0);
    if (n <= 0) {
        printf("Errore ricezione dati\n");
        return 1;
    }

    printf("Ricevuto risultato dal server ip %s. ", server_ip);

    if (resp.status == STATUS_OK) {
        switch (resp.type) {
            case 't':
                printf("%s: Temperatura = %d gradi\n", req.city, resp.value);
                break;
            case 'h':
                printf("%s: Umidità = %d%%\n", req.city, resp.value);
                break;
            case 'w':
                printf("%s: Vento = %d km/h\n", req.city, resp.value);
                break;
            case 'p':
                printf("%s: Pressione = %d hPa\n", req.city, resp.value);
                break;
        }
    }
    else if (resp.status == STATUS_CITY_NOT_FOUND) {
        printf("Citta' non disponibile\n");
    }
    else {
        printf("Richiesta non valida\n");
    }

    closesocket(sock);

#if defined WIN32
    WSACleanup();
#endif
    return 0;
}
