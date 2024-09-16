# Reti-di-Calcolatori

In questa repository è presente il progetto di Reti di Calcolatori, svolto nell'anno accademico 2023/2024.

Gli argomenti trattati per la realizzazione di questo progetto sono:
- Creazione e gestione dei Socket;
- Comunicazione Client-Server;
- Gestione delle connessioni;
- Gestione degli errori di rete.

Il file "Segreteria_Universitaria.pdf" fornisce informazioni più approfondite riguardo il progetto e riguardo le istruzioni di compilazione ed esecuzione.

# Segreteria Universitaria

Il progetto simula un sistema di gestione per la prenotazione di esami universitari. Il sistema è composto da tre componenti principali: la segreteria, lo studente e il server universitario, che interagiscono tramite comunicazione client/server parallela.

La segreteria:
- Inserisci gli esami nel server universitario;
- Inoltra le richieste di prenotazione degli studenti al server universitario;
- Fornisce agli studenti le date degli esami disponibili per un corso scelto.

Lo studente:
- Richiede alla segreteria informazioni su esami disponibili per un corso specifico;
- Invia una richiesta di prenotazione per un esame alla segreteria.

Il server universitario:
- Riceve e gestisce l'aggiunta di nuovi esami;
- Riceve e gestisce le prenotazioni degli esami;
- Assegna un numero di prenotazione progressivo per ogni richiesta e lo invia alla segreteria, che a sua volta lo inoltra allo studente.
