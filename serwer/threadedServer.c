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
#include <fcntl.h>

#define SERVER_PORT 8080
#define QUEUE_SIZE 15

//struktura zawierająca dane o klientach i wiadomości do wyslania do nich
struct client_data
{
    int sock_desc;
    char nick[16];
};

//struktura zawierająca dane, które zostaną przekazane do wątku
struct thread_data_t
{
    int sent;
    char message[4096];
    char recipiant[16];
};

//tablica zwierająca informacje o zalogowanych klientach
struct client_data client_list[256];
int last_id=0;

//zmienna zawierająca następną wiadomość do wysłania i jej odbiorcę
struct thread_data_t to_send[32];
int last_msg_id=0;

//uchwyt na mutex
pthread_mutex_t con_mutex = PTHREAD_MUTEX_INITIALIZER;

//funkcja licząca ilość znaków w stringu
int count_char(char msg[])
{
    int count=0;
    
    for(int i=0;i<4096;i++)
    {
        if(msg[i]=='/')
        {
            count++;
            
            if(count==5)
            {
                return count;
            }
        }
    }
    
    return count;
}

//funkcja zwracająca długość słowa
int wordLength(char word[])
{
    for(int i=0;i<4096;i++)
    {
        if(word[i]==0)
        {
            return i;
        }
    }
}

//funckja sprawdzająca czy użytkownik jest na liście zalogowanych i uaktualniająca jego deskryptor
int checkPresence(char name[], int fd)
{
    int is_found=0;
    
    for(int i=0;i<last_id;i++)
    {
        if(!strcmp(name,client_list[i].nick))
        {
            client_list[i].sock_desc=fd;
            is_found=1;
        }
    }
    
    return is_found;
}

//funkcja czyszcząca tablicę to_send
void cleanup()
{
    int counter=0;
    
    for(int i=0;i<32;i++)
    {
        if(!to_send[i].sent)
        {
            strcpy(to_send[counter].recipiant,to_send[i].recipiant);
            strcpy(to_send[counter].message,to_send[i].message);
            to_send[i].sent=0;
            counter++;
        }
                
        for(int j=0;j<16;j++)
        {
            to_send[i].recipiant[j]=0;
        }
    }
    
    last_msg_id=counter;
}

//funkcja znajdująca odpowiedni element w przesłanym tekście
char* findElement(char msg[])
{
    int i=2;
    int j=0;
    static char element[16];
    
    //dopóki nie natrafisz na znak / oddzielający różne elementy wiadomości przepisuj element
    while(msg[i]!='/')
    {
        element[j]=msg[i];
        i++;
        j++;
    }
    
    //uzupelnij reszte stringa pustymi znakami
    for(j;j<16;j++)
    {
        element[j]=0;
    }
    
    return element;
}

//funkcja wylogowująca użytkownika
void logout(char usr[])
{
    for(int i=0;i<last_id;i++)
    {
        if(!strcmp(usr,client_list[i].nick))
        {
            client_list[i].sock_desc=-1;
        }
    }
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
            char nicki[16]={0};
            
            //znalezienie użytkownika w wiadomości
            strcpy(nicki,findElement(msg));
            
            //sprawdzenie czy użytkownik jest już na liście zalogowanych 
            int find_result=checkPresence(nicki,sock_desci);
            //jeśli nie dopisanie użytkownika
            if(!find_result)
            {
                client_list[last_id].sock_desc=sock_desci;
                strcpy(client_list[last_id].nick,nicki);
                last_id++;
                printf("Zapisano uzytkownika\n");
            }
            
            break;
            
        //odebranie wiadomości i zapisanie jej do przekazania do odbiorcy
        case 'M':
            printf("Przygotowywanie wiadomości\n");
            
            //odbiorca wiadomości
            char rec[16]={0};
            
            strcpy(rec,findElement(msg));
            
            //wypełnienie struktury zawierającej wiadomość do wysłania
            strcpy(to_send[last_msg_id].message,msg);
            strcpy(to_send[last_msg_id].recipiant,rec);
            to_send[last_msg_id].sent=0;
            last_msg_id++;
            if(last_msg_id==32)
            {
                cleanup();
            }
            
            break;
            
        case 'W':
            printf("Wylogowywanie\n");
            
            //odbiorca wiadomości
            char user[16]={0};
            
            strcpy(user,findElement(msg));
            
            logout(user);
            
            break;
    }
}

//funkcja odpowiedzialna za odbieranie wiadomości
void receive(int fd)
{
    //liczba znaków '/' w odebranym buforze
    int char_number;
    int size=5;
    
    char rec_msg[4096]={0};
    char rec_buf[4096]={0};
        
    pthread_mutex_lock(&con_mutex);
    read(fd,rec_msg,4096);
    pthread_mutex_unlock(&con_mutex);
    
    if(rec_msg[0]=='\0')
    {
        return;
    }
    else if(rec_msg[0]=='W')
    {
        size=2;
    }
    
    char_number=count_char(rec_msg);
    
    strcpy(rec_buf,rec_msg);
    
    while(char_number!=size)
    {
        char rec_msg[4096]={0};
        
        pthread_mutex_lock(&con_mutex);
        read(fd,rec_msg,4096);
        pthread_mutex_unlock(&con_mutex);
        
        char_number+=count_char(rec_msg);
        
        strcat(rec_buf,rec_msg);
    }
    
    printf("Odebrano wiadomosc\n");
    
    handleInput(rec_buf,fd);
}

//funkcja opisującą zachowanie wątku - musi przyjmować argument typu (void *) i zwracać (void *)
void *MainThreadBehavior(void *t_data)
{
    //długość wiadomości
    int length;
    
    while(1)
    {
        pthread_detach(pthread_self());
        
        for(int i=0;i<last_id;i++)
        {
            //jeżeli klient nie jest odłączony
            if(client_list[i].sock_desc!=-1)
            {
                receive(client_list[i].sock_desc);
                
                //wysłanie wszystkich wiadomości do odbiorcy
                for(int j=0;j<last_msg_id;j++)
                {
                    //printf("Message: %s, count: %d\n",to_send[j].message,last_msg_id);
                    if(!strcmp(client_list[i].nick,to_send[j].recipiant))
                    {
                        if(!to_send[j].sent)
                        {
                            length=wordLength(to_send[j].message);
                            
                            int count=write(client_list[i].sock_desc,to_send[j].message,length);
                            printf("Wyslano wiadomosc\n");
                            
                            to_send[j].sent=1;
                        }
                    }
                }
            
            }
                    
        }
    }
    
    pthread_exit(NULL);
}

//funkcja obsługująca połączenie z nowym klientem
void handleConnection(int connection_socket_descriptor) {
    //bufor na wiadomosc od klienta
    char msg[4096]={0};

    //odczytanie danych i przekazanie ich do funkcji zajmującej się ich przetwarzaniem
    read(connection_socket_descriptor,msg,4096);
    printf("Odczytano dane\n");
    
    pthread_mutex_lock(&con_mutex);
    printf("Mutex zamkniety\n");
    handleInput(msg,connection_socket_descriptor);
    
    fcntl(connection_socket_descriptor, F_SETFL, O_NONBLOCK);
    pthread_mutex_unlock(&con_mutex);
    printf("Mutex otwarty\n");
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
   printf("Utworzono gniazdo\n");

   bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
   if (bind_result < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", argv[0]);
       exit(1);
   }
   printf("Dowiazano ardes IP i numer portu\n");

   listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
   if (listen_result < 0) {
       fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", argv[0]);
       exit(1);
   }
   printf("Ustawiono wielkosc kolejki\n");
   
    //wynik funkcji tworzącej wątek
    int create_result = 0;

    //uchwyt na wątek
    pthread_t thread2;
    
    //dane, które zostaną przekazane do wątku
    struct thread_data_t* t_data_m=malloc(sizeof(struct thread_data_t));

    create_result = pthread_create(&thread2, NULL, MainThreadBehavior, (void *)t_data_m);
    if (create_result){
       printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
       exit(-1);
    }
    printf("Utworzono watek\n");

    while(1)
    {
       connection_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
       if (connection_socket_descriptor < 0)
       {
           fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
           exit(1);
       }
       printf("Utworzono gniazdo polaczenia\n");
        
       handleConnection(connection_socket_descriptor);
    }

    close(server_socket_descriptor);
    return(0);
}
