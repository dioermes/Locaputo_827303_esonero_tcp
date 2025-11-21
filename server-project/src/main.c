#include "protocol.h"

// === Funzioni di generazione dati casuali (ora int) ===
int get_temperature(void) { return rand() % 51 - 10; }   // -10 → 40 °C
int get_humidity(void)    { return rand() % 81 + 20; }   // 20 → 100 %
int get_wind(void)        { return rand() % 101; }       // 0 → 100 km/h
int get_pressure(void)    { return rand() % 101 + 950; } // 950 → 1050 hPa

int is_valid_city(const char* city) {
    const char* valid_cities[] = {
        "bari","roma","milano","napoli","torino",
        "palermo","genova","bologna","firenze","venezia"
    };
    for (int i = 0; i < 10; i++)
        if (strcasecmp(city, valid_cities[i]) == 0)
            return 1;
    return 0;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

#if defined WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(1,1), &wsa);
#endif

    int port = SERVER_PORT;
    if (argc == 3 && strcmp(argv[1], "-p") == 0)
        port = atoi(argv[2]);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Errore creazione socket");
        return 1;
    }

    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore bind");
        return 1;
    }

    listen(server_socket, 5);
    printf("Server meteo in ascolto sulla porta %d...\n", port);

    while (1) {
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) continue;

        weather_request_t req;
        recv(client_socket, (char*)&req, sizeof(req), 0);

        printf("Richiesta '%c %s' dal client ip %s\n",
               req.type, req.city, inet_ntoa(client_addr.sin_addr));

        weather_response_t resp = {0};
        if (!is_valid_city(req.city)) {
            resp.status = STATUS_CITY_NOT_FOUND;
        } else {
            switch (req.type) {
                case 't': resp.value = get_temperature(); resp.status = STATUS_OK; break;
                case 'h': resp.value = get_humidity();    resp.status = STATUS_OK; break;
                case 'w': resp.value = get_wind();        resp.status = STATUS_OK; break;
                case 'p': resp.value = get_pressure();    resp.status = STATUS_OK; break;
                default:  resp.status = STATUS_INVALID_REQUEST; break;
            }
        }
        resp.type = req.type;
        send(client_socket, (const char*)&resp, sizeof(resp), 0);
        closesocket(client_socket);
    }

#if defined WIN32
    WSACleanup();
#endif
    closesocket(server_socket);
    return 0;
}
