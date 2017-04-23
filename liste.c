#include "e.h"
#include "Eina.h"
#include "eina_list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



static int
main(void)
{
  
   
   FILE *fp;
   // Datei oeffnen
   fp = fopen("/home/simon/efl_src/tageslosung/losungen2016.xml", "r");

  if(fp == NULL) 
   {
	 printf("Error:\n");
        return 0;
   }

   
#define BUF 512
  char puffer[BUF];
  int counter = 1;
  /////////////////////////////////////
   Eina_List *Losungs_List = NULL;
   Eina_List *Losungs_Item_List = NULL;
   Eina_List *l = NULL;
//   EINA_LIST_FREE(app_list, data) eina_stringshare_del(data);
//    eina_list_free(app_list);
//    
   while( fgets(puffer, BUF, fp) != NULL ) 
    {
        //if(counter > 2)
            Losungs_List = eina_list_prepend(Losungs_List, "test");
         //if(strstr(puffer, buf_time) != 0)
         
         //counter++;
    }

                
	fclose(fp);

        
     printf("LISTE AUSGABE:\n");
    for(l = Losungs_List; l; l = eina_list_next(l))
       printf("%s\n", (char*)eina_list_data_get(l));
 
        
   return EXIT_SUCCESS;
}
