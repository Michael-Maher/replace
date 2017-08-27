
/************************************************************
 * 		Code Name  : replace
 * 		Author     : Michael Maher Morkos
 * 		Date       : 27/8/2017
 * 		Contact me : 0127-456-4112
 * 					 michael.m.Morkos@gmail.com
 ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef unsigned char uint8;
#define MAX_LEN_SINGLE_LINE 200

/********************************************************************************************
* 	   strstr_insenstive : searches for word in string (like strstr BUT case insensitive) 	*					     *
*********************************************************************************************/

char* strstr_insenstive(const char* search_st, const char* req_word) {
  do {
    const char* h = search_st;
    const char* n = req_word;
    while (tolower((uint8) *h) == tolower((uint8 ) *n) && *n) {   // compare lowercace for word with lowercase of string
      h++;
      n++;
    }
    if (*n == 0) {                                                // found matched word
      return (char *) search_st;
    }
  } while (*search_st++);
  return 0;
}

/*************************************************************************************
* 			  replace function: takes modifiers,from,to and files as i/p		     *
*************************************************************************************/

uint8 replace (char*mod[],uint8 mod_size,char*fr,char*to,char*fil[],uint8 fil_size,uint8 l_f_exist){

/************************ Declarations & Definitions ********************************/

char *line_ptr, *find_ptr,*search=fr,*rep=to;                       // make pointer for line and find_ptr
uint8 i,j,find_len = strlen(search),count,count_dw,done,back_up,pass,comp;
//printf("mod size =%d",mod_size);
char buffer[32],buffer2[32],*last,*new; 	 					    // The filename buffer.
char line[MAX_LEN_SINGLE_LINE+2];  				                    // make a char variable represent max len. of line
FILE *fp1, *fp2,*fp_read,*fp_write;                                 // make file pointers

/************************ Outer loop for files ********************************/

for(i=0;i<fil_size;i++){
//printf("l_f_exist: %d",l_f_exist);
	count_dw=2; done=0; pass=0; comp=1;  count=comp; back_up=0;
snprintf(buffer, sizeof(char) * 32, "replaced__%s",fil[i]);         // Put "file" then i+1 then ".txt" in to filename.
snprintf(buffer2, sizeof(char) * 32, "last__%s", fil[i]);           // Put "file" then i+1 then ".txt" in to filename.
last=buffer2;
new=buffer;
fp1 = fopen(fil[i],"r");
	 if(fp1==NULL){
		 printf("one of the requested file isn't exist");
		 return 0;}
	 fp2 = fopen(new,"w");    assert(fp2);                       //check if the file (that'll be modified) whether exists or not ?!
	 fp_read=fp1; fp_write=fp2;

/************************  Inner loop for Modifiers ********************************/

	for(j=0;j<mod_size;j++){

/*************************************************************************************
* 									-f Modifier						     			 *
*************************************************************************************/
	      if((strcmp(mod[j],"-f")==0) && (l_f_exist!=2)){

/*******************  this condition to handle (-f -l) OR (-l -f) case **************/
	    	  l2: if(pass){
	    		  remove(fil[i]);
	    		  rename(last,fil[i]);
	    		  fp_read = fopen(fil[i],"r");
	    		  fp_write = fopen(new,"w");
	    		  pass=0;}

/****************  this loop to search for a word in each line of file **************/
	       while(fgets(line,MAX_LEN_SINGLE_LINE+2,fp_read))			// gets new line from read file
	       {
	           line_ptr = line;
	           while ((find_ptr = strstr(line_ptr,fr))&&!done)		//done is used here to stop searching when first occurrence is found
	           {
	               while(line_ptr < find_ptr)						// this loop searches and puts the content of read file into write file
	                   fputc((int)*line_ptr++,fp_write);
	               fputs(to,fp_write);								// when search word (from) is found. put replace word(to) in write file
	               line_ptr += find_len;							// pass length of search word
	               done=1;
	               break;
	           }
	           fputs(line_ptr,fp_write);							// write the remainder of the read file into the write file

	       }
	       fclose(fp_read);											// close read and write file
	       fclose(fp_write);
	       rename(new,last);										// rename the write file(new) to (last) to facilitate back up operation (if req. OR nor)
	    //   pass=1;
	       done=0; }


/*************************************************************************************
* 									-l Modifier						     			 *
*************************************************************************************/

	      else if ((strcmp(mod[j],"-l")==0) || (l_f_exist==2)){

/*******************  this condition to handle (-f -l) OR (-l -f) case **************/
	    	  if(l_f_exist==2){
	    		  j++;												// as the next modifier will be (-l OR -f) and i know that from l_f_exist
	    		  pass=1;											// to let (-f) operation know that (-l) operation was previously done.
	    		  comp=0;											// considered as stop limit for reverse process (replace (to) -> (from))
	    	  }

/************ this condition to replace (to) -> (from) except the last word	********/
	    l1: 	  if(done){//printf("entered safely\n");
	    		  fp_read=fopen(new,"r");
	    		  fp_write=fopen(last,"w");
	    		  search=to; rep=fr;
	    	      count_dw=count-1;									// count_dw is defined to replace (to) -> (from) except the last word
	    		  find_len = strlen(to);
	    	  }
	    	  while(fgets(line,MAX_LEN_SINGLE_LINE+2,fp_read) )
	    	 		       {
	    	 		           line_ptr = line;
	    	 		           while ((find_ptr = strstr(line_ptr,search)) && count_dw>comp ) //char *strstr(const char *haystack, const char *needle) function finds the first occurrence of the substring needle in the string haystack. The terminating '\0' characters are not compared.
	    	 		           { if((pass && done) && (count_dw==count-1)) count_dw--;        //this condition to replace (to) -> (from) except the last word
	    	 		           else{ while(line_ptr < find_ptr)
	    	 		                      fputc((int)*line_ptr++,fp_write);
	    	 		                 fputs(rep,fp_write);
	    	 		                 if(done==0)  count++;
	    	 		                //	printf("\nrepeated word=%d",count-1);
	    	 		                 else count_dw--;
	    	 		               line_ptr += find_len; }}

	    	 		           fputs(line_ptr,fp_write);

	    	 		           }

	    	 			   fclose(fp_read);
	    	 		       fclose(fp_write);
	    	 		     // printf("\ntotal no. of file%d=%d",i+1,count);
	    	 		      if(!done){ 								    // reverse operation doesn't done yet
	    	 		    	 done=1;
	    	 		    	 goto l1;}
	    	 		      else{//remove (new); 							// no need foe new any more
	    	 		    	find_len = strlen(fr);						// return find_len and other variables to its default
	    	 		    	done=0; comp=1; search=fr; rep=to;
	    	 		    	if(pass && l_f_exist==2)					// this condition to handle (-f -l) OR (-l -f) case
	    	 		    		goto l2;}
	    	 		      }
/*************************************************************************************
 * 									-i Modifier						     			 *
 *************************************************************************************/
	      else if(strcmp(mod[j],"-i")==0){
	    	  while(fgets(line,MAX_LEN_SINGLE_LINE+2,fp1))				// as discussed above except here i used strstr_insensitive function instead of strstr built in function
	    	  	       {
	    	  	           line_ptr = line;
	    	  	           while ((find_ptr = strstr_insenstive(line_ptr,fr)))
	    	  	           {
	    	  	               while(line_ptr < find_ptr)
	    	  	                   fputc((int)*line_ptr++,fp2);
	    	  	               fputs(to,fp2);
	    	  	               line_ptr += find_len;

	    	  	           }
	    	  	           fputs(line_ptr,fp2);

	    	  	       }
	    	  	       fclose(fp2);
	    	  	       fclose(fp1);
	    	  	       rename(new,last);

	    	  	       }


/*************************************************************************************
 * 									-b Modifier						     			 *
 *************************************************************************************/
	    	  	     else back_up=1;									// to know if back up operation is required OR not

	      }

		if(back_up){
	  	  rename(last,new);}
	     else {remove(fil[i]);
	      rename(new,last);
	  	   rename(last,fil[i]);}

}

return 1;
}


/*************************************************************************************
 * 									Main function					     			 *
 *************************************************************************************/
int main(int argc,char**argv)
{char*files[4],*from,*to,*Modifiers[4],*new_ent;
uint8 b=0,c=0,i,files_start=0,l_f_exist=0;

   if (argc <6){
	   printf("No. of arguments less than the accepted level");
	   return 0;}

   else{
        for (i=1;i<argc;i++){
        	new_ent=argv[i];
            if(new_ent[0]=='-' && (strlen(new_ent)==2) ){	    //this condition to fill Modifiers array with required Modifiers names
              if(new_ent[1]!='-' ){
                if(new_ent[1]=='b'||new_ent[1]=='f'||new_ent[1]=='l'||new_ent[1]=='i'){ // to ensures NOT to enter a NOT valid modifier
                	       if(new_ent[1]=='f' || new_ent[1]=='l') l_f_exist++;         // to know detect (-f -l) OR (-l -f) cases
                	       Modifiers[b]=new_ent;
                           b++;}
                else {printf("please enter a valid Modifier");
                                   return 0;}}
              else files_start=1;									       // this condition to distinguish between Modifiers and files
                }
     /*************************************************/
            else if((files_start)){										 //this condition to fill files array with required files names
                files[c]=new_ent;
                c++;}
     /*************************************************/
            else { from=new_ent; i++; to=argv[i];}
        }
   }

  // printf("b=  %d\n",b);

 for(i=0;i<b;i++)
     printf("Modifier no. %d is : %s\n",i+1,Modifiers[i]);

   printf("form : %s\n",from);
   printf("to : %s\n",to);

   for(i=0;i<c;i++)
     printf("File no. %d is : %s\n",i+1,files[i]);

  // printf("l_f_exist: %d\n",l_f_exist);

  if(replace(Modifiers,b,from,to,files,c,l_f_exist))  					// function to replace req. files with req. Modifiers
	  printf("\n>> total Replacement of files is SUCCESSED.");
  else printf("\n\n>> total Replacement of files is FAILED !!");



   return 0;


}


