#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <locale.h>

int processos[100];

//cria um processo e coloca no array de processos
void startShell(char * comando, char * argv[]);

//verifica se um processo existe e remove do array de processos
void waitShell();

//verifica se o processo existe e finaliza-o
void waitForShell(char * arg2, char * argumentos[]);

//chamar o start e o waitFor daquele processo
void runShell();

//cria o processo, define o temporizador, caso o tempo definido seja ultrapassado, mate o processo, se não termine com sucesso
void watchdogShell();

//move o contexto do processo para o lugar destino
void chdirShell(char * arg1, char * argumentos[]);

//obtém pathAtual
void pwdShell();

//limpa as entradas
void limpaEntradas(char * buffer, char * palavras[], char * arg1, char * arg2, char * argumentos[]);

int main(){
    //inicializa as variaveis
    char buffer[4096];
    char * palavras[100];
    char * arg1;
    char * arg2;
    char * argumentos[98];
    
    //configura para aceitar os caracteres PT-BR
    setlocale(LC_ALL,"Portuguese");

    while(1){
        //força a impressão na tela do myshell>
        fflush(stdout);
        //imprime myshell em negrito e verde
        printf("\033[0;32m\e[1mmyshell>\033[0m ");

        //força a impressão na tela do myshell>
        fflush(stdout);
        
        //lê o stdin (entrada do teclado) e armazena no buffer
        //falta tratar erro caso receba mais de 4096 caracteres
        fgets(buffer, 4096, stdin);
        
        //pega a primeira palavra e separa
        palavras[0] = strtok(buffer, " \t\n");
        //se a palavra for NULL ele volta ao começo do shell
        if(palavras[0] == NULL){
            continue;
        }

        for(int i = 1; i < 101; i++){
            palavras[i] = strtok(0," \t\n");
            if(palavras[i] == NULL){
                break;
            }
            if(i == 100){
                printf("myshell: Quantidade de parâmetros além do permitido\n");
            }
        }

        //seta os argumentos para serem usados mais facilmente
        arg1 = palavras[0];
        arg2 = palavras[1];
        for(int i = 0; i < 98; i++){
            if(palavras[i] == NULL){
                break;
            }
            argumentos[i] = palavras[i+2];
        }
        
        //escolhe qual instrução seguir
        if(strcmp(arg1, "start") == 0){
            startShell(arg2, argumentos);
        }else if(strcmp(arg1, "wait") == 0){
            waitShell(argumentos);
        }else if (strcmp(arg1, "waitfor") == 0){
            waitForShell(arg2, argumentos);
        }else if(strcmp(arg1, "run") == 0){
            runShell();
        }else if(strcmp(arg1, "watchdog") == 0){
            watchdogShell();
        }else if(strcmp(arg1, "chdir") == 0){
            chdirShell(arg2, argumentos);
        }else if(strcmp(arg1, "pwd") == 0){
            pwdShell();
        }else if(strcmp(arg1, "exit") == 0 || strcmp(arg1, "quit") == 0 ){
            printf("myshell: até mais!!\n");
            return 0;
        }else{
            printf("myshell: comando desconhecido: %s \n", arg1);
        }

    //prepara as variaveis que armazenam os argumentos para uma nova execução
    limpaEntradas(buffer, palavras, arg1, arg2, argumentos);
    }
    return 0;
}

// Função para abrir novos programas  com as chamadas fork() e exec()
void startShell(char * comando, char * argv[]){
    
    // recebe o PID do filho
    int retval = fork(); 

    printf("[Processo filho ID: %5d]\n[Processo pai ID: %5d]\n", retval, getpid());
    
    //Se retval for menor que 0 houve erro ao criar o processo filho
    if(retval < 0){
         printf("Erro\n");
    }
    else if(retval > 0){    //se retval processo pai e recebeu o ID do filho
       //  adicionarProcesso(retval);
         printf("Processo criado com sucesso.\n");
    }else{ // SOU o processo filho
        if(argv[0] == NULL){
            char * argumentosp[] = {NULL};
            execvp(comando,argumentosp); //comandos = nome do programa
            }
        else{
            execvp(comando, argv); //comandos = nome do programa, argv =  argumentos que acompanham o programa
            }
        }  
        
    //   printf("Programa executado\n");
    
}

void waitShell (){
   

}

void waitForShell(char * arg2,char * argumentos[]){
    
    //tratamentos necessarios
    int pid = atoi(arg2);
    int status;
    int sucesso = waitpid(pid, &status, 0);
    if(sucesso < 0){
        printf("Erro ao fechar o processo filho. \n");
    }else{
        if(WIFEXITED(status)){
            printf("filho %s terminou normalmente. \n", arg2);
        }
    }
}

void runShell(){

}

//cria o processo, define o temporizador, caso o tempo definido seja ultrapassado, mate o processo, se não termine com sucesso
void watchdogShell(){
    
}

//move o contexto do processo para o lugar destino
void chdirShell(char * arg2, char * argumentos[]){
    if(arg2 != NULL){
        if(argumentos[0] == NULL){
            int sucesso = chdir(arg2);
            if(sucesso == 0){
                pwdShell();
            }else{
                //TRATAMENTO DE ERRO NECESSARIO
                printf("myshell: não foi possivel acessar %s\n", arg2);
            }
        }else{
            printf("myshell: muitos argumentos para o chdir\n");
        }
    }else{
        printf("myshell: insira o diretório destino!! tente: chdir <diretórioDestino>\n");
    }
    
}

//obtém pathAtual e imprime na tela
void pwdShell(){
    char pathAtual[4096];
    int * sucess = getcwd(pathAtual, 4096);
    printf("myshell: %s\n", pathAtual);
}

void limpaEntradas(char * buffer, char * palavras[], char * arg1, char * arg2, char * argumentos[]){
    buffer[0] = '\0';
    
    for(int i = 99; i >= 0; i--){
        palavras[i] = NULL; 
    }
    for(int i = 97; i >= 0; i--){
        argumentos[i] = NULL; 
    }
    arg1 = NULL;
    arg2 = NULL;
}