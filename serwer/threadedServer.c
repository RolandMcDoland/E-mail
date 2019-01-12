#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define SERVER_PORT 1234
#define QUEUE_SIZE 5

//struktura zawierająca dane o klientach i wiadomości do wyslania do nich
struct client_data
{
    int sock_desc;
    char nick[16];
};

//struktura zawierająca dane, które zostaną przekazane do wątku
struct thread_data_t
{
    int sock_desc;
    char message[4096];
    char recipiant[16];
};

//tablica zwierająca informacje o zalogowanych klientach
struct client_data client_list[256];
int last_id=0;

//zmienna zawierająca następną wiadomość do wysłania i jej odbiorcę
struct thread_data_t to_send;

//zmienna zawierająca wiadomość do wyslania w następnej iteracji i jej odbiorcę
struct thread_data_t to_send_next;

//funkcja znajdująca odpowiedni element w przesłanym tekście
char* findElement(char msg[])
{
    int i=2;
    int j=0;
    static char element[16];
            
    while(msg[i]!='/')
    {
        element[j]=msg[i];
        i++;
        j++;
    }
    
    return element;
}

//funkcja przetwarzająca dane wysłane przez klienta
void handleInput(char msg[],int sock_desci)
{
    //w zależności od akcji wybranej przez klienta
    switch(msg[0])
    {
        //dodanie do tablicy zalogowanych uzytkownikow
        case 'L':
            printf("Logowanie\n");
            
            //nazwa użytkownika który się loguje
            char nicki[16];
            
            strcpy(nicki,findElement(msg));
            
            client_list[last_id].sock_desc=sock_desci;
            strcpy(client_list[last_id].nick,nicki);
            
            //powiadomienie uzytkownika o zalogowaniu
            strcpy(to_send_next.message,"M/None/Serwer/Zalogowano");
            strcpy(to_send_next.recipiant,nicki);
            break;
            
        //odebranie wiadomości i zapisanie jej do przekazania do odbiorcy
        case 'M':
            printf("Przygotowywanie wiadomości\n");
            
            //odbiorca wiadomości
            char rec[16];
            
            strcpy(rec,findElement(msg));
            
            //wypełnienie struktury zawierającej wiadomość do wysłania
            strcpy(to_send_next.message,msg);
            strcpy(to_send_next.recipiant,rec);
            break;
    }
}

//funkcja opisującą zachowanie wątku - musi przyjmować argument typu (void *) i zwracać (void *)
void *ThreadBehavior(void *t_data)
{
    pthread_detach(pthread_self());
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;
    char msg[4096];

    //odczytanie danych i przekazanie ich do funkcji zajmującej się ich przetwarzaniem
    read((*th_data).sock_desc,msg,4096);
    handleInput(msg,(*th_data).sock_desc);

    pthread_exit(NULL);
}

//funkcja obsługująca połączenie z nowym klientem
void handleConnection(int connection_socket_descriptor) {
    //wynik funkcji tworzącej wątek
    int create_result = 0;

    //uchwyt na wątek
    pthread_t thread1;

    //dane, które zostaną przekazane do wątku
    struct thread_data_t* t_data;
    
    //wypełnienie pól struktury
    t_data->sock_desc=connection_socket_descriptor;

    create_result = pthread_create(&thread1, NULL, ThreadBehavior, (void *)t_data);
    if (create_result){
       printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
       exit(-1);
    }
    
    //zmienna przechowująca deskryptor gniazda odbiorcy
    int recipiant_fd=-1;
    
    //pętla wyszukująca wśród zalogowanych użytkowników odbiorcy wiadomości
    for(int i=0;i<=last_id;i++)
    {
       if(strcmp(client_list[i].nick,to_send.recipiant))
       {
           recipiant_fd=client_list[i].sock_desc;
       }
    }
    
    //przesłanie wiadomości do odbiorcy
    char msg[4096];
    strcpy(msg,to_send.message);
    
    write(recipiant_fd,msg,2048);
    
    //oczekiwanie na zakończenie wątku
    pthread_join(thread1,NULL);
    
    to_send=to_send_next;
}

int main(int argc, char* argv[])
{
   printf("Zaczynam\n");
   int server_socket_descriptor;
   int connection_socket_descriptor;
   int bind_result;
   int listen_result;
   char reuse_addr_val = 1;
   struct sockaddr_in server_address;

   //inicjalizacja gniazda serwera
   
   memset(&server_address, 0, sizeof(struct sockaddr));
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = htonl(INADDR_ANY);
   server_address.sin_port = htons(SERVER_PORT);

   server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
   if (server_socket_descriptor < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda..\n", argv[0]);
       exit(1);
   }
   setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

   bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
   if (bind_result < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", argv[0]);
       exit(1);
   }

   listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
   if (listen_result < 0) {
       fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", argv[0]);
       exit(1);
   }

   while(1)
   {
       connection_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
       if (connection_socket_descriptor < 0)
       {
           fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
           exit(1);
       }

       handleConnection(connection_socket_descriptor);
   }

   close(server_socket_descriptor);
   return(0);
}
