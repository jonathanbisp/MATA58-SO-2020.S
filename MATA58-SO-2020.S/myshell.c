#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <locale.h>

//cria um processo e coloca no array de processos
void startShell(char * argumentos[]);

//verifica se um processo existe e remove do array de processos
void waitShell(char * argumentos[]);

//verifica se o processo existe e finaliza-o
void waitForShell(char * argumentos[]);

//chamar o start e o waitFor daquele processo
void runShell(char * comando[]);

//cria o processo, define o temporizador, caso o tempo definido seja ultrapassado, mate o processo, se não termine com sucesso
void watchdogShell(char * argumentos[]);

//move o contexto do processo para o lugar destino
void chdirShell(char * argumentos[]);

//obtém pathAtual
void pwdShell(char * argumentos[], int * flag);

//limpa as entradas
void limpaEntradas(char * buffer, char * palavras[], char * arg1, char * argumentos[]);

//limpa a tela
void clear(char * argumentos[]);

//função complemento para watchdog
void proc_exit();

//guarda quantos processos foram criados atráves de um start
int qtdProc = 0, pidForWait, flagw = 0;

int main(){
    //inicializa as variaveis

    
    //configura para aceitar os caracteres PT-BR
    setlocale(LC_ALL,"Portuguese");

    while(1){
        char buffer[4096];
        char * palavras[100];
        char * arg1;
        char * argumentos[99];
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
                printf("myshell: quantidade de parâmetros além do permitido\n");
            }
        }

        //seta os argumentos para serem usados mais facilmente
        arg1 = palavras[0];
        
        for(int i = 0; i < 99; i++){
            if(palavras[i] == NULL){
                break;
            }
            argumentos[i] = palavras[i+1];
        }
        
        //escolhe qual instrução seguir
        if(strcmp(arg1, "start") == 0){
            startShell(argumentos);
        }else if(strcmp(arg1, "wait") == 0){
            waitShell(argumentos);
        }else if (strcmp(arg1, "waitfor") == 0){
            waitForShell(argumentos);
        }else if(strcmp(arg1, "run") == 0){
            runShell(argumentos);
        }else if(strcmp(arg1, "watchdog") == 0){
            watchdogShell(argumentos);
        }else if(strcmp(arg1, "chdir") == 0){
            chdirShell(argumentos);
        }else if(strcmp(arg1, "pwd") == 0){
            pwdShell(argumentos, 0);
        }else if(strcmp(arg1, "clear") == 0 || strcmp(arg1, "clr") == 0 || strcmp(arg1, "cls") == 0){
            clear(argumentos);
        }else if(strcmp(arg1, "exit") == 0 || strcmp(arg1, "quit") == 0 ){
            printf("myshell: até mais!!\n");
            return 0;
        }else{
            printf("myshell: comando desconhecido: %s \n", arg1);
        }

    //prepara as variaveis que armazenam os argumentos para uma nova execução
    limpaEntradas(buffer, palavras, arg1, argumentos);
    }
    return 0;
}

// Função para abrir novos programas  com as chamadas fork() e exec()
void startShell(char * comando[]){
    if(comando[0] != NULL){
        // recebe o PID do filho
        int retval = fork(); 
        
        //Se retval for menor que 0 houve erro ao criar o processo filho
        if(retval < 0){
            //adicionar errno
            printf("myshell: erro ao criar o processo. status: %s.\n", strerror(errno));
        }
        else if(retval > 0){    //se retval processo pai e recebeu o ID do filho
            qtdProc++;
            pidForWait = retval;
            printf("myshell: processo %d iniciado\n", retval);
        }else{ // SOU o processo filho
        int erro = execvp(comando[0], comando); //comandos = nome do programa, argv =  argumentos que acompanham o programa
            if(erro == -1){
                    //errno para setar
                    printf("myshell: não foi possivel executar o comando: %s. status: %s\n", comando[0], strerror(errno));
                    exit(1);
            }
        } 
    }else{
        printf("myshell: quantidade de argumentos inválida, tente: start <comando> <argumentos>\n");
    }
}

void waitShell(char * argumentos[]){
    if(argumentos[0] == NULL){
        if(qtdProc==0){
            printf("myshell: não há processos em execução.\n");
        }else{
            int wstat;
            int waitSucesso = wait(&wstat);
            if(waitSucesso < 0){
                printf("myshell: erro ao esperar o processo %d. Status: %s \n", pidForWait, strerror(errno));
            }else{
                if(WIFEXITED(wstat)){
                    printf("myshell: o processo %d terminou com status %d. \n", pidForWait, WEXITSTATUS(wstat));
                }else if (WIFSIGNALED(wstat)){
                    printf("myshell: o processo %d terminou porque nenhum sinal foi detectado. Status %d \n", pidForWait, WTERMSIG(wstat));
                }else if(WIFSTOPPED(wstat)){
                    printf("myshell: o processo %d parou com o status %d. \n", pidForWait, WSTOPSIG(wstat));
                }
            }
            qtdProc--; 
        }
    }else{
        printf("myshell: muitos argumentos, tente: wait\n");
    }
}

void waitForShell(char * argumentos[]){
    if(argumentos[0] != NULL){
        if(argumentos[1] == NULL){
            //tratamentos necessarios
            int pid = atoi(argumentos[0]); //tratamento de erro necessario
            // se o atoi tiver
            if(pid == 0){
                printf("myshell: argumento inválido\n");
            }else{
            
                int status;
                //wai
    
                int sucesso = waitpid(pid, &status, 0);
                
                if(sucesso < 0){
                    printf("myshell: erro ao esperar o processo %s. Status: %s \n", argumentos[0], strerror(errno));
                }else{
                    if(WIFEXITED(status)){
                        printf("myshell: o processo %s terminou com status %d. \n", argumentos[0], WEXITSTATUS(status));
                    }else if(WIFSIGNALED(status)){
                        printf("myshell: o processo %s terminou porque nenhum sinal foi detectado. Status %d \n", argumentos[0], WTERMSIG(status));
                    }else if(WIFSTOPPED(status)){
                        printf("myshell: o processo %s parou com o status %d. \n", argumentos[0], WSTOPSIG(status));
                    }
                }
                qtdProc--; 
            }
        }else{
            printf("myshell: quantidade de argumentos inválida, tente: waitfor <numeroProcesso>\n");
        }

    }else{
            printf("myshell: quantidade de argumentos inválida, tente: waitfor <numeroProcesso>\n");
        }
}

void runShell(char * comando[]){

    if(comando[0] == NULL){
        printf("myshell: quantidade de argumentos inválida, tente: run <nomeDoPrograma> <argumento1> <argumento2> \n");
    }else{
        int wstatus;
        signal(SIGINT, SIG_DFL);
        int pid = fork();
        if (pid < 0){
            printf("myshell: erro ao abrir processo. Status: %s.\n", strerror(errno));
        }
        else if (pid == 0){
            //execvp retorna -1 se não conseguir executar o comando, isso impede o run ovo
            int erro = execvp(comando[0], comando);
            if(erro == -1){
                //errno para setar
                printf("myshell: não foi possivel executar o comando: %s. status: %s\n", comando[0], strerror(errno));
                exit(1);
            }
        } 
        signal(SIGINT, SIG_IGN);
        wait(&wstatus);
        if (WIFEXITED(wstatus)){
            printf("myshell: processo %d finalizado com status %d. \n", pid, WEXITSTATUS(wstatus));
        }else if(WIFSIGNALED(wstatus)){
            printf("myshell: o processo %d terminou porque nenhum sinal foi detectado. Status %d \n", pid, WTERMSIG(wstatus));
        }else if(WIFSTOPPED(wstatus)){
            printf("myshell: o processo %d parou com o status %d. \n", pid, WSTOPSIG(wstatus));
        }  
    }
}

//cria o processo, define o temporizador, caso o tempo definido seja ultrapassado, mate o processo, se não termine com sucesso
void watchdogShell(char * argumentos[]){
    if(argumentos[0] != NULL){
        if(argumentos[1]!= NULL){
            int tempo = atoi(argumentos[0]);
            if(tempo == 0){
                printf("myshell: argumento de tempo inválido, tente: whatchdog <tempo> <comando> <argumentos>\n");
            }else{
                char * argsWatchDog[98];
                for(int i = 0; i < 98; i++){
                    if(argumentos[i+1] == NULL){
                        break;
                    }else{
                        argsWatchDog[i] = argumentos[i+1];
                    }
                }
                pidForWait = fork();

                if(pidForWait < 0){
                    printf("myshell: erro ao abrir processo. Status: %s.\n", strerror(errno));
                }else if(pidForWait > 0){
                    signal(SIGCHLD,proc_exit);
                    sleep(tempo);
                    int StatKill =  kill(pidForWait, SIGKILL);
                    if(StatKill < 0 && flagw == 0){
                       printf("myshell: erro ao matar o processo %d. Código de erro: %d. \n", pidForWait, StatKill); 
                       flagw = 0;
                    }
                    
                }else{
                    int erro = execvp(argsWatchDog[0], argsWatchDog);
                    if(erro == -1){
                        //errno para setar
                        printf("myshell: não foi possivel executar o comando: %s\n", argsWatchDog[0]);
                        exit(1);
                    }
                }
         //continuar aqui dentro wallace
            }
        }else{
            printf("myshell: sem comando, tente: whatchdog <tempo> <comando> <argumentos>\n");
        } 
    }else{
        printf("myshell: sem argumentos, tente: whatchdog <tempo> <comando> <argumentos>\n");
    }
    
    
}

//move o contexto do processo para o lugar destino
void chdirShell(char * argumentos[]){
    if(argumentos[0] != NULL){
        if(argumentos[1] == NULL){
            int sucesso = chdir(argumentos[0]);
            if(sucesso == 0){
                pwdShell(argumentos, (int *)1);
            }else{
                //TRATAMENTO DE ERRO NECESSARIO
                printf("myshell: não foi possivel acessar %s. Status: %s\n", argumentos[0], strerror(errno));
            }
        }else{
            printf("myshell: muitos argumentos para o chdir\n");
        }
    }else{
        printf("myshell: insira o diretório destino!! tente: chdir <diretório_destino>\n");
    }
    
}

//obtém pathAtual e imprime na tela
void pwdShell(char * argumentos[], int * flag){
    if(flag == 0){
        if(argumentos[0] != NULL){
            printf("myshell: quantidade de argumentos inválida, tente: pwd\n");
        }else{
            char pathAtual[4096];
            char * sucess = getcwd(pathAtual, 4096);
            if(sucess == NULL){
                printf("myshell: erro ao localizar path atual. Status: %s\n", strerror(errno));
            }else{
                printf("myshell: %s\n", sucess);
            }   
        } 
    }else{
        char pathAtual[4096];
        char * sucess = getcwd(pathAtual, 4096);
        if(sucess == NULL){
            printf("myshell: erro ao localizar path atual. Status: %s\n", strerror(errno));
        }else{
            printf("myshell: %s\n", sucess);
        }   
    }
}

void proc_exit(){
    int wstatus;
    wait(&wstatus);
        if (WIFEXITED(wstatus)){
            printf("myshell: processo %d finalizado com status %d. \n", pidForWait, WEXITSTATUS(wstatus));
            flagw = 1;
        }else if(WIFSIGNALED(wstatus)){
            printf("myshell: processo %d terminado por tempo de execução excedido. Status %d \n", pidForWait, WTERMSIG(wstatus));
        }else if(WIFSTOPPED(wstatus)){
            printf("myshell: o processo %d parou com o status %d. \n", pidForWait, WSTOPSIG(wstatus));
        }
}

void limpaEntradas(char * buffer, char * palavras[], char * arg1, char * argumentos[]){
    buffer[0] = '\0';
    
    for(int i = 99; i >= 0; i--){
        palavras[i] = NULL; 
    }
    for(int i = 97; i >= 0; i--){
        argumentos[i] = NULL; 
    }
    arg1 = NULL;
}

void clear(char * argumentos[]){ //limpa a tela
    
    if (argumentos[0] == NULL){
        printf ("\033c");
    }else{
        printf("myshell: quantidade de argumentos inválida, tente: clear\n");
    }

}