#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#define MAX_EXAMS 50                
#define MAX_DATE_LENGTH 20
#define MAX_EXAM_LENGTH 100
#define MAX_LINE_LENGTH 256

//Struttura Exam per memorizzare il nome dell'esame e le date
typedef struct {
    char exam_name[MAX_EXAM_LENGTH];
    char dates[3][MAX_DATE_LENGTH];     //Max. 3 date
} Exam;

//Array di esami
Exam exams[MAX_EXAMS];

int exams_count = 0;        //Contatore del numero di esami presenti
int booking_number = 0;     //Numero di prenotazioni effettuate

void clearExamsFile();
void saveExams();
void loadExams();
void secretariatRequest(int client_socket);
void studentRequest(int client_socket);

int main() {
    clearExamsFile();

    int server_socket, client_socket;       //Variabili per i socket del server e del client
    struct sockaddr_in server_addr, client_addr;        //Strutture per gli indirizzi del server e del client
    socklen_t client_addr_len = sizeof(client_addr);    //Lunghezza dell'indirizzo del client
    server_socket = socket(AF_INET, SOCK_STREAM, 0);    //Creazione del socket del server

    if (server_socket == -1) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;           //Impostazione della famiglia di indirizzi Internet
    server_addr.sin_addr.s_addr = INADDR_ANY;   //Impostazione dell'indirizzo IP del server
    server_addr.sin_port = htons(1024);         //Impostazione della porta del server

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding error");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 1024...\n");

    loadExams();        //Caricamente degli esami da file

    while (1) {     //Ciclo infinito per gestire le connessioni dei client
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);       //Accettazione di una connessione in entrata
        
        if (client_socket == -1) {
            perror("Client socket error");
            continue;
        }

        int client_type;        //Variabile per il tipo di client (quindi segreteria o studente)
        recv(client_socket, &client_type, sizeof(client_type), 0);      //Ricezione del tipo di client

        if (client_type == 1) {                 //Controlla se il client è la segreteria
            secretariatRequest(client_socket);  //Gestione della richiesta della segreteria
            saveExams();
        } else if (client_type == 2) {          //Controlla se il client è uno studente
            studentRequest(client_socket);      //Gestione della richiesta dello studente
        }

        close(client_socket);       //Chiusura del socket del client
    }

    close(server_socket);

    return 0;
}

void clearExamsFile() {
    FILE *exams_file = fopen("exams.txt", "w");

    if(exams_file == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    fclose(exams_file);
}

//Funzione per salvare gli esami sul file "exams.txt"
void saveExams() {
    FILE *exams_file = fopen("exams.txt", "w");     //Apertura del file degli esami

    if (exams_file == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < exams_count; i++) {         //Ciclo per scrivere gli esami nel file
        fprintf(exams_file, "%s\n", exams[i].exam_name);

        for (int j = 0; j < 3; j++) {               //Ciclo per scrivere le date dell'esame nel file
            if (strlen(exams[i].dates[j]) != 0) {           //Controlla se la data non è vuota
                fprintf(exams_file, "%s\n", exams[i].dates[j]);
            }
        }

        fprintf(exams_file, "\n");      //Riga vuota per separare gli esami nel file
    }

    fclose(exams_file);
}

//Funzione per caricare gli esami da file "exams.txt"
void loadExams() {
    FILE *exams_file = fopen("exams.txt", "r");     //Apertura del file degli esami
    
    if (exams_file == NULL) {
        perror("Error opening file for reading");
        return;
    }

    char line[MAX_LINE_LENGTH];         //Buffer per leggere una linea del file
    int index = -1;                     //Indice per gli esami
    int index_date = 0;                 //Indice per le date dell'esame

    while (fgets(line, MAX_LINE_LENGTH, exams_file) != NULL) {      //Ciclo per leggere ogni linea del file
        line[strcspn(line, "\n")] = '\0';       //Rimozione del carattere di nuova riga dalla fine della linea

        if (strlen(line) > 0) {         //Controlla se la linea non è vuota
            if (index == -1) {          //Se non si sta già leggendo un esame
                index++;
                exams_count++;

                strncpy(exams[index].exam_name, line, MAX_EXAM_LENGTH - 1);     //Copia del nome dell'esame nella struttura
                
                exams[index].exam_name[MAX_EXAM_LENGTH - 1] = '\0';     //Aggiunta del terminatore di stringa
                index_date = 0;
            } else {                    //Se si sta giò leggendo un esame
                if (index_date < 3) {           //Controlla se ci sono ancora date da leggere
                    strncpy(exams[index].dates[index_date], line, MAX_DATE_LENGTH - 1);     //Copia della data nella struttura
                    
                    exams[index].dates[index_date][MAX_DATE_LENGTH - 1] = '\0';
                    index_date++;
                }
            }
        } else {        //Se la linea è vuota
            index = -1;
        }
    }

    fclose(exams_file);
}

//Funzione per gestire le richieste della segreteria
void secretariatRequest(int client_socket) {
    int request_type;       //Variabile per il tipo di richiesta
    recv(client_socket, &request_type, sizeof(request_type), 0);        //Ricezione del tipo di richiesta

    if (request_type == 1) {                    //Se la richiesta è per aggiungere un nuovo esame
        char exam_name[MAX_EXAM_LENGTH];        //Buffer per il nome dell'esame
        char dates[3][MAX_DATE_LENGTH];         //Buffer per le date dell'esame
        recv(client_socket, exam_name, sizeof(exam_name), 0);       //Ricezione del nome dell'eaame dal client

        int dates_count = 0;            //Contatore per il numero di date ricevute
        recv(client_socket, &dates_count, sizeof(dates_count), 0);      //Ricezione del numero di date

        for (int i = 0; i < dates_count; i++) {          //Ciclo per trovare le date
            int date_length;
            recv(client_socket, &date_length, sizeof(date_length), 0);         //Ricezione della lunghezza della data

            if (date_length > 0) {              //Se la lunghezza della data è valida
                char date[MAX_DATE_LENGTH];     //Buffer per la data
                recv(client_socket, date, date_length, 0);      //Ricezione della data dal client

                date[date_length] = '\0';       //Aggiunta del terminatore di stringa
                
                strncpy(exams[exams_count].dates[i], date, MAX_DATE_LENGTH - 1);    //Copia della data nella struttura
                
                exams[exams_count].dates[i][MAX_DATE_LENGTH - 1] = '\0';        //Aggiunta del terminatore di stringa
            } else {        //Se la lunghezza della data non è valida
                exams[exams_count].dates[i][0] = '\0';      //Impostazione della data come vuota
            }
        }

        strcpy(exams[exams_count].exam_name, exam_name);        //Copia del nome dell'esame nella struttura
       
        exams_count++;

        int response = 1;       //Risposta per la conferma della ricezione 
        send(client_socket, &response, sizeof(response), 0);        //Invio della conferma al client
    }
}

//Funzione per gestire le richieste degli studenti
void studentRequest(int client_socket) {
    int request_type;
    recv(client_socket, &request_type, sizeof(request_type), 0);

    if (request_type == 2) {        //Se la richiesta è per prenotare un esame
        char exam_name[MAX_EXAM_LENGTH];        //Buffer per il nome dell'esame
        recv(client_socket, exam_name, sizeof(exam_name), 0);

        int dates_count = 0;        //Contatore per il numero di date disponibili
        char available_dates[3][MAX_DATE_LENGTH];       //Buffer per le data disponibili

        for (int i = 0; i < exams_count; i++) {         //Ciclo per cercare l'esame richiesto
            if (strcmp(exams[i].exam_name, exam_name) == 0) {       //Se l'esame è stato trovato
                for (int j = 0; j < 3; j++) {           //Ciclo per copiare le date disponibili
                    if (strcmp(exams[i].dates[j], "") != 0) {       //Se la data è disponibile
                        strncpy(available_dates[dates_count], exams[i].dates[j], MAX_DATE_LENGTH - 1);      //Copia della data
                        
                        available_dates[dates_count][MAX_DATE_LENGTH - 1] = '\0';       //Aggiunta del terminatore di stringa
                        dates_count++;
                    }
                }
                
                break;      //Uscita dal ciclo una volta trovato l'esame
            }
        }

        send(client_socket, &dates_count, sizeof(dates_count), 0);      //Invio del numero di date disponibili al client

        for (int i = 0; i < 3; i++) {       //Ciclo per inviare le date disponibili al client
            int string_length = strlen(available_dates[i]) + 1;     //Lunghezza della stringa
            
            send(client_socket, &string_length, sizeof(string_length), 0);      //Invio della lunghezza della stringa
            send(client_socket, available_dates[i], string_length, 0);          //Invio della data
        }

        int choice;     //Scelta dell'utente
        recv(client_socket, &choice, sizeof(choice), 0);        //Ricezione della scelta dell'utente

        if (choice >= 1 && choice <= dates_count) {     //Se la scelta è valida
            booking_number++;           //Incremento del numero di prenotazioni
            send(client_socket, &booking_number, sizeof(booking_number), 0);    //Invio del numero di prenotazione al client
        
            //Invia il nome dell'esame e la data di prenotazione
            send(client_socket, exam_name, sizeof(exam_name), 0);
            send(client_socket, available_dates[choice - 1], strlen(available_dates[choice - 1]) + 1, 0);
        
            printf("Booking number %d for exam '%s' on date %s\n", booking_number, exam_name, available_dates[choice - 1]);

        } else {        //Se la scelta non è valida
            int confirmation = 0;       //Conferma di fallimento
            send(client_socket, &confirmation, sizeof(confirmation), 0);        //Invio della conferma al client
        }
    }
}