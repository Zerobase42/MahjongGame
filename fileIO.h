#include<stdlib.h>
#include<stdio.h>
int readFILE(char*filename,char&buf){
    FILE*file=fopen(filename,"rb");
    if(file==NULL){
        return-1;
    }
    fseek(file,0,SEEK_END);
    long length=ftell(file);
    fseek(file,0,SEEK_SET);
    size_t readCnt=fread(buf,1,length,file);
    buf[readCnt]='\0';
    fclose(file);
    return readCnt;
}