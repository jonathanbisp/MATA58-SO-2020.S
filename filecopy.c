#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main (int  * argc, char * argv[]){
  if(argc != 2){
    printf("Erro! Quantidade de argumentos inválida.")
    return -1;
  }

    int fd = open(argv[0], O_RDONLY);
    if(fd == -1) {
      printf("O arquivo que deseja copiar não existe. Erro número %d", errno)
    }
    
}