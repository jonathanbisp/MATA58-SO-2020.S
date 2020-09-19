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

    int filecopied = open(argv[1], O_RDONLY);
    //Verifica se o arquivo a copiar existe
    if(filecopied < 0) {
      printf("ERRO: Não foi possível abrir o código %s: %s  \n", argv[1], strerror(errno));

      return 1;
    }
    
    //abrir arquivo se o arquivo não existir cria o arquivo
    int filetarget = open(argv[2], O_RDWR | O_CREAT); 
    //Verifica se a ação de abrir ou criar ocorreu com sucesso
    if(filetarget < 0){
      printf("ERRO: A ação não foi concluída: %s \n", strerror(errno) );

      return 1;
    }
    char buffer[4096];
    int totalbytes = 0;

    /* read write */
      while(1){
        int offset = 1;
        

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
  
  close(filecopied);
  close(filetarget);
}