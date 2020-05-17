#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX1 200000         //Dosyayı okuyup aktörleri ve filmleri içine koyacağımız dizilerin maksimum boyutu
#define MAX2 200            //Film veya aktörün isminin maksimum uzunluğu

//Buradaki dizileri global tanımlama sebebim dizi boyutlarının çok büyük olması ve bu boyutun fonksiyon içinde tanımlama için fazla olmasıdır.
char movies[MAX1][MAX2];    //Filmleri tutacak olan dizimiz
char actors[MAX1][MAX2];    //Aktörleri tutacak olan dizimiz

//Derste yazdığımız Graf Fonksiyonları başlangıcı
struct node{                                        
    int vertex;
    struct node *next;
};

struct Graph{
    int numVertices;
    struct node **adjLists;
};

struct node* createNode(int vertex){

    struct node *newNode = malloc(sizeof(struct node));
    newNode->vertex = vertex;
    newNode->next = NULL;
    return newNode;
}

struct Graph* createGraph(int vertices){

    int i;
    struct Graph *graph = malloc(sizeof(struct Graph));
    graph->numVertices = vertices;
    graph->adjLists = malloc(vertices * sizeof(struct node*));
    for(i = 0 ; i < vertices ; i++){
        graph->adjLists[i] = NULL;
    }
    return graph;
}

void addEdge(struct Graph* graph, int src, int dest){
    
    struct node *newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
    
    newNode = createNode(src);
    newNode->next = graph->adjLists[dest];
    graph->adjLists[dest] = newNode;
}
//Derste yazdıklarımızın sonu

//Graf yapımı yazdırdığım fonksiyon
void printGraph(struct Graph *myGraph, int movieCounter){

    struct Graph *g = myGraph;
    //Yazdırırken indekse göre yapılan movie & actor ayrımı raporda ayrıntılı biçimde açıklanmıştır.
    int i;
    for(i = 0 ; i < g->numVertices ; i++){
        if(i >= movieCounter){
            printf("%s -->  ", actors[i-movieCounter]);
        }
        else{
            printf("%s -->  ", movies[i]);
        }
        
        struct node *tmp = g->adjLists[i];
        while(tmp){
            if(tmp->vertex >= movieCounter){
                printf("%s ||", actors[(tmp->vertex)-movieCounter]);
            }
            else{
                printf("%s ||", movies[tmp->vertex]);
            }
            tmp = tmp->next;
        }
        printf("\n\n");
    }
}

//Okunan kelimenin film olup olmadığını döndüren fonksiyon
int isMovie(char tmp[MAX2]){

    int i = 0;
    while(tmp[i] != '\0' && tmp[i] != '('){
        i++;
    }
    if(tmp[i] == '('){  //Eğer '(' içeriyorsa filmdir.
        return 1;       
    }
    else{
        return 0;
    }
}

//Dosyadan okunan film veya aktörün ilgili dizide zaten olup olmadığı kontrol edilmektedir.
//Zaten var ise diziye koyulmaz çünkü graf yapısında aynı isimde iki node olmamalı.
int isThere(int movieOrActor, int i, char vertex[MAX2]){

    if(movieOrActor == 1){
        while(strcmp(movies[i], vertex) && i >= 0){
            i--;
        }
        if(strcmp(movies[i], vertex) == 0){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        while(strcmp(actors[i], vertex) && i >= 0){
            i--;
        }
        if(strcmp(actors[i], vertex) == 0){
            return 1;
        }
        else{
            return 0;
        }
    } 
}

//Dosyanın okunup movies ve actors dizilerinin doldurulması işleminin yapıldığı fonksiyon
void readFile(int *movieCounter, int *actorCounter, char nameOfFile[50]){

    FILE *fp = fopen(nameOfFile, "r");
    
    if(fp == NULL){
        printf("File does not exist!");
        exit(1);
    }

    char tmp = getc(fp);
    int i;
    char tmpWord[MAX2];
    i = 0;
    //İlk okunan değerin film olduğu bilindiğinden film dizisine atılır.
    while(tmp != '/'){
        tmpWord[i] = tmp;
        i++;
        tmp = getc(fp);
    }
    tmpWord[i] = 0;
    strcpy(movies[0], tmpWord);
    (*movieCounter)++;
    
    tmp = getc(fp);
    //Okunan her kelimenin film veya aktör olduğuna karar verilir ve
    //İlgili dizide eğer yoksa o diziye atanır.
    while(!feof(fp)){
        i = 0;
        while(!feof(fp) && tmp != '/' && tmp != '\n'){
            tmpWord[i] = tmp;
            i++;
            tmp = getc(fp);
        }
        tmpWord[i] = 0;
        
        if(isMovie(tmpWord)){
            strcpy(movies[(*movieCounter)++], tmpWord);
        }
        else{
            if(!isThere(0, *actorCounter, tmpWord)){
                strcpy(actors[*actorCounter], tmpWord);
                (*actorCounter)++;
            }  
        } 
        printf("\rNumber of Movies: %d     Number of Actors: %d", *movieCounter, *actorCounter);
        tmp = getc(fp);
    }
    fclose(fp);
}

//Parametre olarak word'un gerekli dizideki indisini bulup döndürür.
int findIndex(int movieOrActor, char word[MAX2], int n){
    int i = 0;
    if(movieOrActor == 1){
        while(i < n && strcmp(movies[i], word) != 0){
            i++;
        }
        if(strcmp(movies[i], word) == 0){
            return i;
        }
        else{
            return -1;
        }
    }
    else{
        while(i < n && strcmp(actors[i], word) != 0){
            i++;
        }
        if(strcmp(actors[i], word) == 0){
            return i;
        }
        else{
            return -1;
        }
    }
}

//Dosyayı tekrar okuyarak, oluşturulan grafa kenar bağlantılarının eklendiği fonksiyon.
struct Graph* createEdges(struct Graph *myGraph, int movieCounter, int actorCounter, char nameOfFile[50]){

    //Öncelikle dosyadan okunan filmin movies dizisindeki indisi tutulur ve bir sonraki film okunana kadar
    //okunan bütün aktörlerin actors dizisindeki indisleri ile film sayısı toplanarak oluşturulan sayı arasında
    //bir kenar bağlantısı oluşturulur.

    int movieIndex, actorIndex;
    static int edgeCounter = 0;

    FILE *fp = fopen(nameOfFile, "r");
    
    if(fp == NULL){
        printf("File does not exist!");
        exit(1);
    }

    char tmp = getc(fp);
    int i;
    char tmpWord[MAX2];
    i = 0;
    while(tmp != '/'){
        tmpWord[i] = tmp;
        i++;
        tmp = getc(fp);
    }
    tmpWord[i] = 0;
    //İlk okunan kelimenin film olduğu bilindiğinden bu indis tutulur.
    movieIndex = findIndex(1, tmpWord, movieCounter);

    tmp = getc(fp);

    while(!feof(fp)){
        i = 0;
        while(!feof(fp) && tmp != '/' && tmp != '\n'){
            tmpWord[i] = tmp;
            i++;
            tmp = getc(fp);
        }
        tmpWord[i] = 0;
        //Ardından okunan bütün kelimeler için film olup olmadığı kontrolü yapıldıktan sonra
        if(isMovie(tmpWord)){
            movieIndex = findIndex(1, tmpWord, movieCounter);
        }
        //aktör ise daha önce tutulan film indisi ile arasında kenar bağlantısı kurulur.
        else{
            actorIndex = findIndex(0, tmpWord, actorCounter);
            edgeCounter++;
            //Kenar eklenirken bulunan aktörün indisine film sayısının eklenme sebebi
            //Graf yapısında ilk node'ların filmlerden, sonraki nodeların ise oyunculardan oluşmasıdır.
            //Kullanıcı bu yapıyı grafı yazdır seçeneğini kullanarak daha net görebilir.
            addEdge(myGraph, movieIndex, actorIndex + movieCounter);
            printf("\rNumber of Edges: %d", edgeCounter);
        }
        tmp = getc(fp);
    }
    fclose(fp);

    return myGraph;
}

//Derste yazdığımız Kuyruk Yapısı Fonksiyonları başlangıcı
struct QNode{
    int value;
    struct QNode *next;
};

struct Queue{
    struct QNode *front, *rare; 
};

struct Queue *createQueue(){

    struct Queue *q;
    q = (struct Queue*)malloc(sizeof(struct Queue));
    if(q == NULL){
        exit(0);
    }
    else{
        q->front = q->rare = NULL;
        return q;
    }
}

void enqueue(struct Queue *q, int value){

    struct QNode *tmp;
    tmp = (struct QNode*)malloc(sizeof(struct QNode));
    if(tmp == NULL){
        exit(0);
    }
    tmp->value = value;
    tmp->next = NULL;
    if(q->front == NULL){
        q->front = q->rare = tmp;
    }
    else{
        q->rare->next = tmp;
        q->rare = tmp;
    }
}

int dequeue(struct Queue *q){

    int value;
    if(q->front == NULL){
        printf("Queue is empty!\n");
        return -1;
    }
    else{
        value = q->front->value;
        struct QNode *tmp;
        tmp = q->front;
        q->front = tmp->next;
        
        if(q->front == NULL){
            q->rare = NULL;
        }
        free(tmp);
        return value;
    }
}
//Derste yazdıklarımızın sonu

//Breath First Search algoritması defalarca çağırıldığı vakit
//Oluşan kirliliği gidermek adına yapılan bir temizleme fonksiyonudur.
void destroyQ(struct Queue *queue){
    while(queue->front != NULL){
        dequeue(queue);
    }
    free(queue);
}

//Kevin Bacon Sayısı İçin Breath First Search Algoritması
int BFS_FindKBacon(struct Graph *myGraph, int s, char findWord[MAX2], int movieCounter, int step){
    
    struct Graph *G = myGraph;
    //Aranan kişi zaten Kevin Bacon ise 0 döndürür.
    if(!strcmp("Bacon, Kevin", findWord)){
        return 0;
    }
    //Kaçıncı adımda olunduğunu ve ayrıca ziyaret edilip edilmeme durumunu tutan dizi
    int *visited = (int*)malloc(G->numVertices * sizeof(int));
    int i;
    for(i = 0 ; i < G->numVertices ; i++){
        visited[i] = -1;
    }
    //Algoritma gereği ilk eleman kuyruğa koyulur.
    struct Queue *queue = createQueue();
    enqueue(queue, s);
    visited[s] = 0;

    int v;
    while(queue->front != NULL){
        v = dequeue(queue);
        struct node *tmp = G->adjLists[v];
        //Kuyruktan çekilen her değer için kuyruğa o değerin graftaki ziyaret edilmemiş komşuları kuyruğa koyulur.
        while(tmp){
            if(visited[tmp->vertex] == -1){

                enqueue(queue, tmp->vertex);
                visited[tmp->vertex] = visited[v] + 1;
                //Geçici olarak tutulan compare değişkeni sürekli ulaşılan node ile
                //karşılaştırılır ve bulunup bulunmadığına karar verilir.
                char compare[MAX2];
                if(tmp->vertex >= movieCounter){
                    strcpy(compare, actors[tmp->vertex - movieCounter]);
                }
                else{
                    strcpy(compare, movies[tmp->vertex]);
                }
                if(!strcmp(compare, findWord)){
                    //Bulunduğunda ise direkt olarak adım sayısı döndürülmez
                    //Öncelikle recursive çağrılar yapılarak graf üzerinde Kevin Bacon'dan
                    //Aranan aktöre giden yol yazdırılır.
                    if(v >= movieCounter){
                        if(strcmp(actors[v - movieCounter], "Bacon, Kevin") == 0){
                            printf("[ %s ].\n", actors[v - movieCounter]);
                        }
                        else{
                            printf("[ %s ] that played in\n", actors[v - movieCounter]);
                        }
                    }
                    else{
                        printf("[ %s ] is the movie, that includes the actor named\n", movies[v]);
                    }
                    step++;

                    destroyQ(queue);
                    free(visited);
                    if(v != s){
                        char arr[MAX2];
                        if(v >= movieCounter){
                            strcpy(arr, actors[v- movieCounter]);
                        }
                        else{
                            strcpy(arr, movies[v]);
                        }
                        //Bu recursive çağrı her bir adım sonunda gelinen adımdan istenen kişiye ulaşmak için
                        //algoritmayı bir kere daha çağırır ve böylelikle adım adım gidilen yol yazdırılır.
                        return BFS_FindKBacon(myGraph, s, arr, movieCounter, step);
                    }
                    else{
                        return step;
                    }
                }
            }
            tmp = tmp->next;
        }
    }
    //Bütün oyuncular için bir frekans dizisi oluşturulup ekrana yazdırılmak istendiğinde
    //fonksiyona manuel olarak 'x' gönderilir ve bu karakter oyuncu listesinde olmadığından bütün grafın gezilmesi
    //sağlanır. Bütün graf gezildiğinde oluşan visited dizisi üzerinden ise frekanslar hesaplanır ve ekrana yazdırılır.
    if(!strcmp("x",findWord)){
        int distances[40] = {0};
        int sum = 0;
        for(i = 0 ; i < G->numVertices ; i++){
            if(visited[i] != -1){
                distances[visited[i]]++;
            }
        }
        printf("\n\n");
        //ikişer atlama sebebi BFS adımlarının birinin filmlerden birinin aktörlerden
        //Şeklinde örüntü ile ilerlemesidir.
        for(i = 0 ; i < 40 ; i += 2){
            if(distances[i] != 0){
                printf("%d\tof actors have\t%d\tdistance from Kevin Bacon.\n", distances[i], i/2);
                sum += distances[i];
            }       
        }
        printf("%d\tof actors have infinite distance from Kevin Bacon.\n", (G->numVertices - sum) - movieCounter);
    }

    //Temizleme işlemlerinin ardından
    //Bulunamadıysa -2 döndürülür.
    destroyQ(queue);
    free(visited);

    return -2;
}

int main(){
    
    int movieCounter = 0, actorCounter = 0;
    
    //Kullanıcı için kolay ve anlaşılır bir arayüz oluşturulması
    printf("\n--------------------------------------\nWELCOME TO KEVIN BACON FAN CLUB\n--------------------------------------\n");
    char nameOfFile[50];
    printf("Write the name of input file: ");
    scanf("%s", nameOfFile);

    printf("\nFile Reading Process: \n");
    readFile(&movieCounter, &actorCounter, nameOfFile); 
      
    struct Graph *myGraph = createGraph(movieCounter + actorCounter); 
    printf("\nEdge Adding Process: \n");
    myGraph = createEdges(myGraph, movieCounter, actorCounter, nameOfFile);
    
    int choice;
    do{
        printf("\n\n--------------------------------------\n1 - Print graph\n2 - Find all distances\n3 - Find one actor's distance\n4 - Quit\n--------------------------------------\n");
        printf("Make your choice: ");
        scanf("%d", &choice);
        //Graf yazdırma fonksiyonu ile grafın konsola basıldığı fonksiyon
        if(choice == 1){
            printf("\nGraph:\n\n");
            printGraph(myGraph, movieCounter);
        }
        //BFS Fonksiyonuna 'x' gönderilerek frekans dizisinin oluşturulup yazdırılıması
        //(Yukarıda fonksiyonun olduğu yerde daha net açıklanmıştır.)
        else if(choice == 2){
            printf("\nDistance Finding Process: \n");
            BFS_FindKBacon(myGraph,findIndex(0, "Bacon, Kevin", actorCounter) + movieCounter, "x", movieCounter, 0);
        }
        //Kullanıcıdan alınan aktör ismi ile birlikte mesafenin bulunup yolun yazırılması
        else if(choice == 3){
            int step = 0;
            char actor[MAX2];
            printf("\nName of actor: ");
            scanf(" %[^\n]s",actor);
            printf("[ %s ] played in\n", actor);
            int distance = BFS_FindKBacon(myGraph,findIndex(0, "Bacon, Kevin", actorCounter) + movieCounter, actor, movieCounter, step);
            if(distance == -2){
                printf("nothing or even he/she played some movies he/she has no connection with Kevin Bacon.");
            }
            else{
                printf("\nSo [ %s ] has [ %d ] distance from Kevin Bacon.\n",actor, distance/2);
            }
        }
        else{
            choice = 4;
        }

    }while(choice != 4);
    printf("\n-------------------\ndeveloped by Sadi\n-------------------\n");
    return 0;
}