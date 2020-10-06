#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 
#include <semaphore.h> //blibioteca para semaforos
  
#define N 16 //tamanho do buffer
#define max 100000//quantidade de números
//qntdProd e qntdCons define a quantidade de produtores e consumidores a serem criados.
int qntdProd = 1, qntdCons = 2, condicao = 0, cond = 0; //quantidade threads produtoras e consumidoras
pthread_mutex_t buffer_mutex;
int buffer[N];
int prod_pos=0, cons_pos=0;
sem_t buffer_cheio, buffer_vazio;


int testePrimo(int numTeste){ //função para calcular se o número é primo ou não, retornando 1 caso seja primo.
    for (int y = 2; y <= numTeste - 1; y++) {
        if (numTeste % y == 0)
            //printf("Não é primo: %d\n", numTeste);
            return 0;
    }
    //printf("É primo: %d\n", numTeste);
    return 1;

}

int generate_random(int l, int r, int count) { //Esse função gera um numero aleatório entre 0 e r
    int i;
    for (i = 0; i < count; i++) {
        int rand_num = (rand() % (r - l + 1)) + l;
    return rand_num; 
   }
}


void *funcaoProd(void *vargp) {
    while(cond == 0){ 
        sem_wait(&buffer_vazio);
        pthread_mutex_lock(&buffer_mutex); //fecha o sinal 
        for(int i=0; i < N; i++){
            if(buffer[i] == 0){
                buffer[i] = generate_random(1, 10000000, 1);
            }
        }
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&buffer_cheio);
    }
    pthread_mutex_unlock(&buffer_mutex);
    sem_post(&buffer_vazio);
} 

void *funcaoCons(void *vargp) 
{   
    while(cond == 0){
        sem_wait(&buffer_cheio);
        pthread_mutex_lock(&buffer_mutex);
        for(int i=0; i < N; i++){
            if(buffer[i] != 0){
                testePrimo(buffer[i]);
                buffer[i] = 0;
                i = N;
            }
        }
        condicao++;
        if (condicao == max){
            cond = 1;
        }
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&buffer_vazio);
    }
    //produto a ser consumido
}
 
int main(){
    printf("Começou!\n");
    srand(time(0)); //hora atual como semente do numero aleatório
    for(int i= 0; i < N; i++){
        buffer[i] = 0;
    }
    pthread_t threadProd[qntdProd], threadCons[qntdCons];
    pthread_mutex_init(&buffer_mutex, NULL);
    sem_init(&buffer_cheio, 0, N); //inicializando semaforo criado 
    sem_init(&buffer_vazio, 0, 0);
    clock_t start = clock();  
    for (int i = 0; i < qntdProd; i++){
        pthread_create(&(threadProd[i]), NULL, funcaoProd, NULL); //criação da thread
    }
    for (int i = 0; i < qntdCons; i++){
        pthread_create(&(threadCons[i]), NULL, funcaoCons, NULL); //criação da thread
    }
    for (int i = 0; i < qntdProd; i++){
        pthread_join(threadProd[i], NULL); //aqui a thread espera o término
    }
    for (int i = 0; i < qntdCons; i++){
         pthread_join(threadProd[i], NULL); //aqui a thread espera a término
    }
    clock_t ends = clock();
    printf("Tempo de execucao: %.5f seg\n\n",(double) (ends - start) / CLOCKS_PER_SEC); 
    printf("Terminou!\n");
}
