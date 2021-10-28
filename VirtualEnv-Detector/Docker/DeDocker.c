#include<stdlib.h>
#include<stdio.h>

int main(){
  int confidence = system("bash docker.sh"); /* chmod +x test.sh ,路径前面要加上bash */
  //printf("%d\n",confidence);
  return 0;
}
