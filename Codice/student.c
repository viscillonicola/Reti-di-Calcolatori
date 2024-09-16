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
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);        //Crea un socket client

    if(client_socket == -1) {
        perror("Client socket error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;                     //Struttura per l'indirizzo del server
    server_addr.sin_family = AF_INET;                   //Famiglia di indirizzi Internet
    server_addr.sin_port = htons(SERVER_PORT);          //Porta del server in formato di rete
    inet_aton(SERVER_IP, &server_addr.sin_addr);        //Converte l'indirizzo IP in formato binario

    if(connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }

    int client_type = 2;                                                //Tipo di client (fisso a 2)
    send(client_socket, &client_type, sizeof(client_type), 0);          //Invia il tipo di client al server

    int request_type = 2;                                               //Tipo di richiesta al server (fisso a 2)
    send(client_socket, &request_type, sizeof(request_type), 0);        //Invia il tipo di richiesta al server

    char exam_name[MAX_EXAM_LENGTH];        

    printf("Enter exam name to check available dates: ");
    fgets(exam_name, sizeof(exam_name), stdin);

    exam_name[strcspn(exam_name, "\n")] = '\0';                         //Rimuove il carattere di nuova riga dalla fine del nome dell'esame

    send(client_socket, exam_name, sizeof(exam_name), 0);               //Invia il nome dell'esame al server

    int dates_count;                                                    //Variabile per il conteggio delle date disponibili
    recv(client_socket, &dates_count, sizeof(dates_count), 0);          //Riceve il numero di date disponibili dal server

    if(dates_count > 0) {                                               //Se ci sono date disponibili
        printf("Available dates for exam %s: \n", exam_name);

        for(int i = 0; i < dates_count; i++) {                                  //Ciclo per ricevere e stampare le date disponibili
            int string_length;                                                  //Variabile per la lunghezza della data
            recv(client_socket, &string_length, sizeof(string_length), 0);      //Riceve la lunghezza della stringa della data dal server

            char date[MAX_DATE_LENGTH];                                         //Buffer per la data
            recv(client_socket, date, string_length, 0);

            date[string_length] = '\0';

            printf("%d. %s\n", i + 1, date);
        }

        int choice;         //Variabile per la scelta dell'utente

        printf("Enter the number corresponding to the chosen date: ");
        scanf("%d", &choice);
        getchar();          //Utilizziamo questo comando per consumare il carattere di riga residuo che potrebbe essere
                            //rimasto nel buffer dopo aver utilizzato la scanf

        send(client_socket, &choice, sizeof(choice), 0);        //Invia la scelta dell'utente al server

        int booking_number;                                                     //Variabile per il numero di prenotazione
        recv(client_socket, &booking_number, sizeof(booking_number), 0);        //Riceve il numero di prenotazione dal server

        if(booking_number > 0) {        //Se la prenotazione è avvenuta con successo
            char booked_exam[MAX_EXAM_LENGTH];
            char booked_date[MAX_DATE_LENGTH];

            //Ricevi il nome dell'esame e la data di prenotazione
            recv(client_socket, booked_exam, sizeof(booked_exam), 0);
            recv(client_socket, booked_date, sizeof(booked_date), 0);
            
            printf("Booking successful for exam %s on date %s. Booking number: %d\n", booked_exam, booked_date, booking_number);
            send(client_socket, &booking_number, sizeof(booking_number), 0);        //Invia il numero di prenotazione al server
        } else {
            printf("Error during booking\n");
        }
    } else {
        printf("No available dates for exam %s\n", exam_name);
    }

    close(client_socket);

    return 0;
}