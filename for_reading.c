#define MAX_TEXT 10
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

FILE *write_file;
FILE *read_file;
char buff[60];
char player[MAX_TEXT];
int score;
char name[MAX_TEXT];

void check_list(){

printf("start reading file.\n");
printf("file : \n");
read_file  = fopen("score_board.txt","r");
/*
do
{
fscanf(read_file,"%[^\n]*c",buff);
printf("%s \n",buff);
}
while(buff!=EOF);
*/

for(int i=0;i<3;i++){
fscanf(read_file,"%[^\n]*c",buff);
printf("%s \n",buff);
}

/*if(buff=="\n")
    printf(" here is a new line \n");
else*/

printf("end of file/no file exist.");
fclose(read_file);
}


void creat_list(){

write_file = fopen("score_board.txt","w");
for(int i=0;i<2;i++){
printf("plz input your name p%d: ",i+1);
scanf("%s",name);
printf("enter your scores: ");
scanf("%d",&score);
fprintf(write_file,"Play %02d : Name: %-20s || Score: %05d \n",i+1,name,score);
}

fprintf(write_file,"\n");
fclose(write_file);
}


int main(){
//creat_list();

check_list();


return 0;


}
