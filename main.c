#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#define _POSIX_C_SOURCE 200809L
int* buffer1;
int* buffer2;
int* buffer3;
int* buffer4;
int* buffer5;
int* buffer6;
int* buffer7;
int contControl;
int buffersize1 = 0;
int buffersize2 = 0;
int buffersize3 = 0;
int buffersize4 = 0;
int buffersize5 = 0;
int buffersize6 = 0;
int buffersize7 = 0;

char outputFileName[20];
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;

int cont = 0;

FILE *files[8];
void mergeWithOneThread(int filenum);

enum { NS_PER_SECOND = 1000000000 };

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

void *myThreadFun(void *vargp)
{
    // Store the value argument passed to this thread
    int *myid = (int *)vargp;

    static int s = 0;

    int i;
    int a = 0;

    int counter;
    int *buffer ;
    int *firstInput;
    int buffsize;
    int outputPermission = 0;
    int b = 0;
    int outWriter = 0;
    pthread_mutex_lock(&lock);
    ++s;
    int k;
    int t;
    if(*myid == 4){
        contControl = 2;
        if(s == 1){
            k = 0;
            t = 2;
            buffer = buffer1;
        }
        if(s == 2){
            k = 2;
            t = 4;
            buffer = buffer2;
        }
        if(s == 3){
            k=0;
            t=0;
            outWriter = 1;
            buffer = buffer5;
            firstInput = buffer1;
            buffsize = buffersize1;
            outputPermission =1;
            goto outputWriters;
        }

        counter = 0;
    }
    else if(*myid == 8){
        contControl = 4;
        if(s == 1){
            k = 0;
            t = 2;
            buffer = buffer1;
        }
        if(s == 2){
            k = 2;
            t = 4;
            buffer = buffer2;
        }
        if(s == 3){
            k = 4;
            t = 6;
            buffer = buffer3;
        }
        if(s == 4){
            k = 6;
            t = 8;
            buffer = buffer4;
        }
        if(s == 5){
            k=0;
            t=0;
            outWriter = 1;
            buffer = buffer5;
            firstInput = buffer1;
            buffsize = buffersize1;
            goto outputWriters;
        }
        if(s == 6){
            k=0;
            t=0;
            outWriter = 2;
            buffer = buffer6;
            firstInput = buffer3;
            buffsize = buffersize3;
            goto outputWriters1;
        }
        if(s == 7){
            k=0;
            t=0;
            outWriter = 3;
            buffer = buffer7;
            firstInput = buffer5;
            buffsize = buffersize1+buffersize2;
            outputPermission =1;
            goto outputWriters2;
        }
        counter = 0;
    }
    else{
        k = 0;
        t = 2;
        buffer = buffer1;
        outputPermission = 1;
    }

    pthread_mutex_unlock(&lock);

    for(int i = k;i<t;i++) {
        while (!feof(files[i])) {
            //pthread_mutex_lock(&lock);
            if (!feof(files[i])) {
                fscanf(files[i], "%d", &a);
                if (counter == 0) {
                    buffer[0] = a;
                } else {
                    int j;
                    for (j = counter - 1; (j >= 0 && buffer[j] > a); j--) {
                        buffer[j + 1] = buffer[j];
                    }
                    buffer[j + 1] = a;
                    //printf("a:%d j:%d %d\n",a,j,pthread_self());
                }
                //printf("%d ",pthread_self());
                counter++;
                //pthread_mutex_unlock(&lock);
            } else {
                //pthread_mutex_unlock(&lock);
            }

        }


    }
    fprintf(stderr,"Sorting is done by %d\n",pthread_self());
    cont++;

    //Create Fifth Buffer
    outputWriters:
    if(outWriter == 1) {
        pthread_mutex_unlock(&lock);
        while (cont < contControl) {//wait for first 4 file or first 8 file

        }
        for (int i = 0; i < 2; i++) {

            int r = 0;
            int w = 0;
            while (r < buffsize) {
                w = firstInput[r];
                if (counter == 0) {
                    buffer[0] = w;
                } else {
                    int j;
                    for (j = counter - 1; (j >= 0 && buffer[j] > w); j--) {
                        buffer[j + 1] = buffer[j];
                    }
                    buffer[j + 1] = w;
                }
                counter++;
                r++;
            }
            buffsize = buffersize2;
            firstInput = buffer2;
            }
            cont++;
        }

        //Create Sixth Buffer
        outputWriters1:
        if(outWriter == 2) {
            pthread_mutex_unlock(&lock);
            printf("cont   ");
            while (cont < contControl) {//wait for first 8 file
            }
            for (int i = 0; i < 2; i++) {
                int r = 0;
                int w = 0;
                while (r < buffsize) {
                    w = firstInput[r];
                    if (counter == 0) {
                        buffer[0] = w;
                    } else {
                        int j;
                        for (j = counter - 1; (j >= 0 && buffer[j] > w); j--) {
                            buffer[j + 1] = buffer[j];
                        }
                        buffer[j + 1] = w;
                    }
                    counter++;
                    r++;
                }
                buffsize = buffersize4;
                firstInput = buffer4;
            }
            cont++;
        }
        outputWriters2:
            if(outWriter == 3) {
                pthread_mutex_unlock(&lock);
                while (cont < 6) {//wait for all outputs and merge them
                }
                for (int i = 0; i < 2; i++) {
                    int r = 0;
                    int w = 0;
                    while (r < buffsize) {
                        w = firstInput[r];
                        if (counter == 0) {
                            buffer[0] = w;
                        } else {
                            int j;
                            for (j = counter - 1; (j >= 0 && buffer[j] > w); j--) {
                                buffer[j + 1] = buffer[j];
                            }
                            buffer[j + 1] = w;
                        }
                        counter++;
                        r++;
                    }
                    buffsize = buffersize3 + buffersize4;
                    firstInput = buffer6;
                }
            }


        if(outputPermission == 1){
            FILE *out;
            out = fopen(outputFileName,"w");
            int h=0;
            for(int i = 0;i<buffersize1+buffersize2+buffersize3+buffersize4 -1;i++){
                fprintf(out,"%d\n",buffer[i]);
                h++;
            }
            fprintf(out,"%d",buffer[h]);
            fprintf(stderr,"writing is done by %d\n",pthread_self());
        }


    return NULL;
    //pthread_exit(NULL);

}

int main(int argc, char *argv[])
{
    int i;
    int filenum;
    if(argc <6){
        printf("Wrong Number of Arguments\n");
    }
    for (int i = 0; i < argc; ++i) {
        if (strcmp("-n", argv[i]) == 0) {
            i++;
            filenum = atoi(argv[i]);
            i++;
            int k = 0;
            while(k<filenum){
                files[k] = fopen (argv[i+k], "r");
                if (files[k] == NULL) {
                    perror("Failed while opening file: \n ");
                    return 1;
                }
                if (strcmp("-o", argv[i]) == 0) {
                    printf("Wrong Arguments Given. Try Again\n");
                    return 0;
                }
                k++;
            }
            if(strcmp(argv[i+k],"-o")!=0){
                printf("Wrong Arguments Given. Try Again\n");
                return 0;
            }
        }
        if (strcmp("-o", argv[i]) == 0) {
            strcpy(outputFileName,argv[i+1]);
        }

    }

    int a = 0;
    if(filenum == 2){
        fscanf(files[0], "%d", &a);
        buffersize1 +=a;
        fscanf(files[1], "%d", &a);
        buffersize1 +=a;
    }
    if(filenum == 4){
        fscanf(files[0], "%d", &a);
        buffersize1 +=a;
        fscanf(files[1], "%d", &a);
        buffersize1 +=a;
        fscanf(files[2], "%d", &a);
        buffersize2 +=a;
        fscanf(files[3], "%d", &a);
        buffersize2 +=a;
    }
    if(filenum == 8){
        fscanf(files[0], "%d", &a);
        buffersize1 +=a;
        fscanf(files[1], "%d", &a);
        buffersize1 +=a;
        fscanf(files[2], "%d", &a);
        buffersize2 +=a;
        fscanf(files[3], "%d", &a);
        buffersize2 +=a;
        fscanf(files[4], "%d", &a);
        buffersize3 +=a;
        fscanf(files[5], "%d", &a);
        buffersize3 +=a;
        fscanf(files[6], "%d", &a);
        buffersize4 +=a;
        fscanf(files[7], "%d", &a);
        buffersize4 +=a;
    }
    if(filenum !=2 && filenum !=4 && filenum!=8){
        printf("Wrong number of file");
    }
    buffer1 = calloc(buffersize1,sizeof(int));
    buffer2 = calloc(buffersize2,sizeof(int));
    buffer3 = calloc(buffersize3,sizeof(int));
    buffer4 = calloc(buffersize4,sizeof(int));
    buffer5 = calloc(buffersize1+buffersize2,sizeof(int));
    buffer6 = calloc(buffersize3+buffersize4,sizeof(int));
    buffer7 = calloc(buffersize1+buffersize2+buffersize3+buffersize4,sizeof(int));
    struct timespec start, finish, delta;
    clock_gettime(CLOCK_REALTIME, &start);
    mergeWithOneThread(filenum);
    clock_gettime(CLOCK_REALTIME, &finish);
    sub_timespec(start, finish, &delta);
    printf("%d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);
    pthread_t tid;
    int k = 0;
   /* for (i = 0; i < filenum-1; i++){
        pthread_create(&tid, NULL, myThreadFun, (void *)&filenum);
    }

    for (i = 0; i < filenum-1; i++)
        pthread_join(tid,NULL);

    for(int i = 0;i<filenum;i++) {
        fclose(files[i]);
    }*/

    //mergeWithOneThread(filenum);
    //printf("Buffer:%d ",buffer1[0]);

    /*for (i = 0; i < buffersize1; i++)
        printf( "%d  \n", buffer1[i]) ;

    for (i = 0; i < buffersize2; i++)
        printf( "B2:%d  \n", buffer2[i]) ;

    for (i = 0; i < buffersize1+buffersize2; i++)
        printf( "B5:%d  \n", buffer5[i]) ;*/


    free(buffer1);
    pthread_exit(NULL);

    return 0;
}

void mergeWithOneThread(int filenum){
    int counter = 0;
    int *buf1;
    int *buf2;
    int *buf3;
    int *buf4;
    int *buf5;
    int *buf6;
    int *buf7;
    int *buf;
    int a;

    buf1 = calloc(buffersize1,sizeof(int));
    buf2 = calloc(buffersize2,sizeof(int));
    buf3 = calloc(buffersize3,sizeof(int));
    buf4 = calloc(buffersize4,sizeof(int));
    buf5 = calloc(buffersize1+buffersize2,sizeof(int));
    buf6 = calloc(buffersize3+buffersize4,sizeof(int));
    buf7 = calloc(buffersize1+buffersize2+buffersize3+buffersize4,sizeof(int));
    buf = buf1;
    for(int i = 0;i<filenum;i++) {
        if(i%2 == 0 && i !=0){
            counter = 0;

            if(i==2){
                buf = buf2;
            }
            if(i==4){
                buf = buf3;

            }
            if(i==6){
                buf = buf4;

            }
        }
        while (!feof(files[i])) {
            if (!feof(files[i])) {
                fscanf(files[i], "%d", &a);
                if (counter == 0) {
                    buf[0] = a;
                } else {
                    int j;
                    for (j = counter - 1; (j >= 0 && buf[j] > a); j--) {
                        buf[j + 1] = buf[j];
                    }
                    buf[j + 1] = a;
                    //printf("a is %d:\n",a);

                }
                counter++;
            } else {

            }

        }
    }

    int buffsize = buffersize1;
    int *firstInput = buf1;
    int outWriter = 1;
    int outputPermission =0;

    if(filenum == 2){
        FILE *out;
        out = fopen("outputSingleThread","w");
        int h=0;
        for(int i = 0;i<buffersize1-1;i++){
            fprintf(out,"%d\n",buf[i]);
            h++;
        }
        fprintf(out,"%d",buf[h]);
        fprintf(stderr,"writing is done by %d\n",pthread_self());
    }
    else{



    if(outWriter == 1) {
        counter = 0;
        buf = buf5;
        for (int i = 0; i < 2; i++) {
            int r = 0;
            int w = 0;
            while (r < buffsize) {
                w = firstInput[r];
                if (counter == 0) {
                    buf[0] = w;
                } else {
                    int j;
                    for (j = counter - 1; (j >= 0 && buf[j] > w); j--) {
                        buf[j + 1] = buf[j];
                    }
                    buf[j + 1] = w;
                }
                counter++;
                r++;
            }
            buffsize = buffersize2;
            firstInput = buf2;
        }
        cont++;
        outputPermission =1;
        if(filenum==8){
            outWriter = 2;
            buf = buf3;
            buffsize = buffersize3;
            firstInput = buf3;
            outputPermission = 0;
        }
    }
    if(outWriter == 2) {
        counter = 0;

        buf = buf6;
        for (int i = 0; i < 2; i++) {

            int r = 0;
            int w = 0;
            while (r < buffsize) {
                w = firstInput[r];
                if (counter == 0) {
                    buf[0] = w;
                } else {
                    int j;
                    for (j = counter - 1; (j >= 0 && buf[j] > w); j--) {
                        buf[j + 1] = buf[j];
                    }
                    buf[j + 1] = w;
                }
                counter++;
                r++;
            }
            buffsize = buffersize3;
            firstInput = buf4;
        }
        cont++;
        outWriter=3;
        firstInput = buf5;
        buffsize = buffersize1 + buffersize2;
    }

    if(outWriter == 3) {
        counter = 0;
        buf = buf7;
        for (int i = 0; i < 2; i++) {
            int r = 0;
            int w = 0;
            while (r < buffsize) {
                w = firstInput[r];
                if (counter == 0) {
                    buf[0] = w;
                } else {
                    int j;
                    for (j = counter - 1; (j >= 0 && buf[j] > w); j--) {
                        buf[j + 1] = buf[j];
                    }
                    buf[j + 1] = w;
                }
                counter++;
                r++;
            }
            buffsize = buffersize3 + buffersize4;
            firstInput = buf6;
        }
        outputPermission = 1;
    }
    if(outputPermission == 1){
        FILE *out;
        out = fopen("outputSingleThread","w");
        int h=0;
        for(int i = 0;i<buffersize1+buffersize2+buffersize3+buffersize4-1;i++){
            fprintf(out,"%d\n",buf[i]);
            h++;
        }
        fprintf(out,"%d",buf[h]);
        fprintf(stderr,"writing is done by %d\n",pthread_self());
    }
    }
}
