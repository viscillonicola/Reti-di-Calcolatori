#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include "wrapper.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1024
#define MAX_DATE_LENGTH 20
#define MAX_EXAM_LENGTH 100

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);    //Creazione del socket client

    if(client_socket == -1) {
        perror("Client socket error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_aton(SERVER_IP, &server_addr.sin_addr);

    if(connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }

    int client_type = 1;    //Tipo di client (fisso a 1)

    send(client_socket, &client_type, sizeof(client_type), 0);

    int request_type = 1;   //Tipo di richiesta (fisso a 1)

    send(client_socket, &client_type, sizeof(request_type), 0);

    char exam_name[MAX_EXAM_LENGTH];    //Buffer per il nome dell'esame
    char input_date[100];               //Buffer per l'input della data
    char dates[3][MAX_DATE_LENGTH];     //Matrice per le date

    printf("Enter exam name: ");
    fgets(exam_name, sizeof(exam_name), stdin);

    exam_name[strcspn(exam_name, "\n")] = '\0';     //Rimuove il carattere di nuova riga dalla fine del nome dell'esame

    send(client_socket, exam_name, sizeof(exam_name), 0);   //Invia il nome dell'esame al server

    int dates_count = 0;    //Contatore per il numero di date inserite dall'utente

    printf("Enter exam dates (max. 3 - ex. 01/02/2024): \n");

    for(int i = 0; i < 3; i++) {    //Ciclo per ottenere le date dall'utente
        printf("Date %d: ", i + 1);
        fgets(input_date, sizeof(input_date), stdin);
        input_date[strcspn(input_date, "\n")] = '\0';   //Rimuove il carattere di nuova riga dalla fine della data inserita

        if(strlen(input_date) > 0) {    //Controllo se la data inserita non è vuota
            strcpy(dates[dates_count], input_date);     //Copia la data nella matrice delle date
            dates_count++;      //Incrementa il contatore delle date inserite
        }
    }

    send(client_socket, &dates_count, sizeof(dates_count), 0);      //Invia il numero di date al server

    for(int i = 0; i < 3; i++) {    //Ciclo per inviare le date al server
        if(strlen(dates[i]) > 0) {      //Controlla se la data non è vuota
            int string_length = strlen(dates[i]);

            send(client_socket, &string_length, sizeof(string_length), 0);  //Invia la lunghezza della stringa al server
            send(client_socket, dates[i], string_length, 0);    //Invia la data al server
        }
    }

    int response;   //Variabile per la risposta del server
    recv(client_socket, &response, sizeof(response), 0);    //Riceve la risposta dal server

    if(response == 1)  
        printf("Exam added successfully\n");
    else
        printf("Error adding exam\n");
    
    close(client_socket);

    return 0;
}