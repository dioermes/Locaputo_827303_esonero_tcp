#ifndef PROTOCOL_H
#define PROTOCOL_H

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

// === Costanti ===
#define SERVER_PORT 56700
#define BUFFER_SIZE 512

// === Codici di stato ===
#define STATUS_OK 0
#define STATUS_CITY_NOT_FOUND 1
#define STATUS_INVALID_REQUEST 2

// === Strutture di protocollo ===
typedef struct {
    char type;        // 't', 'h', 'w', 'p'
    char city[64];    // nome città
} weather_request_t;

typedef struct {
    unsigned int status;
    char type;
    int value;
} weather_response_t;

// === Prototipi funzioni server ===
int get_temperature(void);
int get_humidity(void);
int get_wind(void);
int get_pressure(void);

#endif // PROTOCOL_H
