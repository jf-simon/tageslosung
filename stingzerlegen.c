/* search_string.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
   char string[] = "  <Losungsvers>Psalm 95,6</Losungsvers>";
   int i=1;
   char *ptr;
   char *strpointer;

   //ptr = strtok(string, " ");
   //int *zeiger;
   //zeiger = &zahl;
   strpointer = &string;
   
   while(strpointer[0] == ' ') 
   {
       strpointer ++;
   }
  /* 
   while(ptr != NULL) {
      //if(i >= 2)
      //{
      printf("%s",ptr);
      //}
      i++;
      ptr = strtok(NULL, "\0");
      
   }*/
   printf("\n");
   printf("%s", strpointer);
   
   return EXIT_SUCCESS;
}
