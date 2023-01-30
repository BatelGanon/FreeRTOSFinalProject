#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#define ADDRESS_NAME "%c%cNakhalat Yitskhak St %d, Tel Aviv-Yafo"
#define SEND_FILES_AND_REMOVE_THEM "./tftp_send.sh"


void generate_files() {
uint8_t n = 20, department = 0, urgency = 0, i = 0, index1=0;
char filename[64]= {0};
size_t buffer_size = 0;
FILE *fp = NULL;
// char buffer[100] = {0};
uint8_t buffer[100] = {0};
    for (index1 = 0; index1 < n; index1++) {
        memset(filename,0,64);
        memset(buffer,0,100);
        sprintf(filename, "file%d.txt", index1);

        if((fp = fopen(filename, "r")) == NULL) {
          fp = fopen(filename, "w");
          if (fp == NULL) {
              perror("Error opening file");
              exit(1);
          }

        buffer[0] = (rand() % 4) + 1 + '0';
        buffer[1] = (rand() % 3) + '0';
        // printf("department:%c      urgency:%c\n",department,urgency);
        // printf("department:%d      urgency:%d\n",department,urgency);

        // sprintf(buffer, "%"PRIu8"%"PRIu8"Nakhalat Yitskhak St %"PRIu8", Tel Aviv-Yafo",department,urgency, rand() % 10);
        sprintf(&buffer[2], "Nakhalat Yitskhak St %"PRIu8", Tel Aviv-Yafo", rand() % 10);
        buffer_size = strlen(buffer);
        for(i = buffer_size; i < 82; ++i) {
            buffer[i] = 0;
        }
        for (i = 82; i < 100; i++)
        {
            buffer[i] = 255;
        }
        
         
        // if (buffer_size == 0){
        //     printf("Buffer %d len is 0!\n",i);
        // }
        // else{
        //     printf("FINE Buffer %d len is %zu FINE!\n",i,buffer_size);
        // }

        // fprintf(fp,"%s",buffer);
        fwrite(buffer, sizeof(char),sizeof(buffer), fp);
        fclose(fp);
        } 
        else { 
            fclose(fp); 
        }
    }

}

void remove_all_files() 
{
    int n = 20;
    char filename[64] = {0};
    char command[128] = {0};
    FILE *fp = NULL;
    for (int i = 0; i < n; i++) {
    memset(filename,0,64);
    memset(command,0,128);
    sprintf(filename, "file%d.txt", i);
    sprintf(command, "rm ./%s", filename);
    if((fp = fopen(filename, "r")) != NULL) {
        fclose(fp);
    int status = system(command);
        if (status) {
            perror("Error removing file");
            exit(1);
            }
    }
    }
}


void send_twenty_files_to_server()
{
    int n = 20, i =0;
    char filename[64]= {0};
    char command_tftp_connect[128]= {0};
    FILE *fp = NULL;
    char command_sleep[10] = "sleep 0.1";
    // int status = system("tftp 192.168.1.2");
    int status = 0;

    for (i = 0; i < n; i++) {
        memset(command_tftp_connect,0,128);
        memset(filename,0,64);
        sprintf(filename, "file%d.txt", i);
        sprintf(command_tftp_connect, "echo -e \"binary\\nput %s\\nquit\" | tftp 192.168.1.2", filename);



        if((fp = fopen(filename, "r")) != NULL) {
            fclose(fp);
            status = system(command_tftp_connect);
                if (status) {
                    perror("Error sending file");
                    exit(1);
                }
            status = system(command_sleep);
                if (status) {
                        perror("Sleep failed");
                        exit(1);
                }
            }
    }
}


void send_generated_files_to_server()
{
    while (1) {
        generate_files();
        int status = system(SEND_FILES_AND_REMOVE_THEM);
        // send_twenty_files_to_server();
        // remove_all_files() ;
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));   // Initialization, should only be called once.
    send_generated_files_to_server();
    return 0;
}
