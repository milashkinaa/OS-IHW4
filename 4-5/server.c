#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_ROOMS 30
#define PORT 12345

int rooms[MAX_ROOMS] = {0};

int firstAvailableRoom() {
    for(int i = 0; i < MAX_ROOMS; i++) {
        if(rooms[i] == 0) return i;
    }
    return -1;
}

void handleRequest(char* buffer) {
    if(strcmp(buffer, "book") == 0) {
        int room = firstAvailableRoom();
        if(room == -1) {
            strcpy(buffer, "queue");
        } else {
            rooms[room] = 1;
            sprintf(buffer, "booked %d", room);
        }
    } else if (strncmp(buffer, "leave", 5) == 0) {
        int room;
        sscanf(buffer, "leave %d", &room);
        if(room >= 0 && room < MAX_ROOMS) {
            rooms[room] = 0;
        }
        strcpy(buffer, "left");
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int len, n;
    len = sizeof(cliaddr);

    char buffer[1024];

    while(1) {
        n = recvfrom(sockfd, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        handleRequest(buffer);
        sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (struct sockaddr *) &cliaddr, len);
    }

    return 0;
}
