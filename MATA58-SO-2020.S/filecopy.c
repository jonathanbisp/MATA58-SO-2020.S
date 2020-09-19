#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

char *strerror(int errnum);

int main (int * argc, char * argv[]){ 

    if( argc != 3 ){
      printf("Quantidade de argumentos inválida.");
      return 1;
    }

    int filecopied = open(argv[1], O_RDONLY, 0755);
    //Verifica se o arquivo a copiar existe
    if(filecopied < 0) {
      printf("filecopy: Não foi possível abrir o arquivo %s: %s  \n", argv[1], strerror(errno));

      return 1;
    }
    
    //abrir arquivo se o arquivo não existir cria o arquivo
    int filetarget = open(argv[2], O_RDWR | O_CREAT, 0755); 
    //Verifica se a ação de abrir ou criar ocorreu com sucesso
    if(filetarget < 0){
      printf("filecopy: Não foi possível abrir ou criar o arquivo: %s \n", strerror(errno) );
      return 1;
    }
    char buffer[4096];
    int totalbytes = 0;

    /* read write */
      while(1){

        //read retorna sempre um valor; o valor do buffer,a quantidade lida, e 0 endoffile
        int rbytes = read(filecopied, buffer, sizeof(buffer));
        if(rbytes == 0){
          break;
        }else if(rbytes < 0){
          printf("Não foi possivel ler o arquivo: %s \n",strerror(errno) );
        }

        totalbytes += rbytes;

        int wbytes = write(filetarget, buffer, rbytes);
        if(wbytes == 0){
          break;
        }else if(wbytes < 0){
          printf("Não foi possivel escrever o arquivo: %s \n", strerror(errno));
        }
      }
  int copiedClosed = close(filecopied);
    if(copiedClosed < 0){
      printf("Não foi possível fechar o arquivo copiado: %s \n", strerror(errno));
    }
  int targetClosed = close(filetarget);
    if(targetClosed < 0){
        printf("Não foi possível fechar a cópia do arquivo original: %s \n", strerror(errno));
    }

  printf("filecopy: foram copiados %d bytes do arquivo %s para o arquivo %s \n",totalbytes, argv[1], argv[2]);
}