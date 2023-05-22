#include <stdio.h>

int createFrame(char *finalData, char *name, int rollNo) 
{   
    snprintf(finalData,25,"1%s*%d",name,rollNo);
    return 0;
}

int main(void)
{
    char arr[25] = {'1','\0'};
    char name[5];
    printf("%d\n",scanf("%s",name));
    int roll = 9;
    int k = createFrame(arr,name,roll);
    printf("%s",arr);
}