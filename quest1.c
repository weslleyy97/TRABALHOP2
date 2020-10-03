#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 
  
// A normal C function that is executed as a thread  
// when its name is specified in pthread_create() 
// K = 2^n

volatile int lock = 0;
int mudarVar = 0, rangeNumeros = 0, argumento = 0;
char numeros[1000000000], resultadoComum = 0;

int generate_random(int l, int r, int count) { //Esse função gera um numero aleatório entre 0 e r
    int i;
    for (i = 0; i < count; i++) {
        int rand_num = (rand() % (r - l + 1)) + l;
    return rand_num; 
   }
}

void *aquire(){
    while(__sync_lock_test_and_set(&lock, 1)) while(lock);
    //sessão critica 
    return NULL;
}


void *release(){
   __sync_lock_release(&lock);
   return NULL;
}


void *myThreadFun(void *vargp) 
{   
    aquire();
    char valorSoma = 0;
    for(int i = rangeNumeros; i < (argumento + rangeNumeros); i++){
        char temp = numeros[i] + valorSoma;
        valorSoma = (char)temp;
    }
    rangeNumeros = rangeNumeros + argumento;
    resultadoComum = resultadoComum + valorSoma; //valor da soma resultado da soma de arrays
    release();
} 


int main() 
{
    int K = 32; 
    srand(time(0)); //hora atual como semente do numero aleatório
    int cem = 100, zero = 0, duzentos = 200, um = 1;
    pthread_t thread_id[K]; //cria identificador do tipo pthread_t
    for(int i = 0; i < sizeof(numeros); i++){
        char novoNumero = (generate_random(zero, duzentos, um) - cem);
        numeros[i] = (char)novoNumero;
    }
    printf("Before Thread\n");
    argumento = (sizeof(numeros) / K);
    clock_t start = clock();  
    for(int j = 0; j < K; j++){
        pthread_create(&(thread_id[j]), NULL, myThreadFun, NULL); //criação da thread
    }
    for(int j = 0; j < 5; j++){ 
        pthread_join(thread_id[j], NULL); //aqui a thread espera a filha
    }
    printf("Resultado comum: %d\n", resultadoComum);
    printf("After Thread\n");
    clock_t ends = clock();
    printf("Tempo de execucao: %.5f seg\n\n",(double) (ends - start) / CLOCKS_PER_SEC);
}

// Passar array de dois inteiros, começa do j até N/K