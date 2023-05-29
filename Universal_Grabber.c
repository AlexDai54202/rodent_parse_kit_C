#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
   // arguments: .\Universal_Grabber.c [Name_of_UEXP] [Column1] [Column2] ...
   FILE *fileptr;
   char *buffer;
   long filelen;
   char label[1024];
   char holder[1024];
   int len_of_columns = argc - 2; 
   // Set format to the column. "m_id", ... ...
   char **format = calloc(len_of_columns, 1024);
   for (int i = 0; i < len_of_columns; i++) {
      format[i] = argv[2+i];
   }

   int format_pointer = 0;
   int matches = 1;
   int parse_pointer_start = 0;
   int parse_pointer_end = 0;
   // intent: pring label: holder

   fileptr = fopen(argv[1], "rb");  // Open the file in binary mode
   fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
   filelen = ftell(fileptr);             // Get the current byte offset in the file
   rewind(fileptr);                      // Jump back to the beginning of the file

   buffer = (char *)malloc(filelen * sizeof(char)); // Enough memory for the file
   fread(buffer, filelen, 1, fileptr); // Read in the entire file
   fclose(fileptr); // Close the file

   // print out columns
   for (int i = 0; i < len_of_columns; i++) {
      if (i == len_of_columns-1) {
         printf("%s",format[i]);
      } else {
         printf("%s,",format[i]);
      }
   }
   
   for (int i = 0; i < filelen-15; i++){
      strncpy(label, buffer + i, strlen(format[format_pointer]));
      label[strlen(format[format_pointer])] = '\0';
      matches = strcmp(label, format[format_pointer]);

      if (matches == 0) {
         parse_pointer_end = i; // end secured.
         // printf("Start: %d, End: %d\n",parse_pointer_start,parse_pointer_end);
         for (int y = parse_pointer_start; y < parse_pointer_end && y!=0; y+=1)
         {
            printf("%02x", (unsigned int)(buffer[y] & 0xff));
            // I fucking hate this so much for some reason it reads anything like 0xf? as 0xfffff? so I have to do this
         }
         if (format_pointer!=0){
            printf(",");
         } else {
            printf("\n");
         }
         parse_pointer_start = i+strlen(format[format_pointer]); // start secured.
         // printf("**%s**", label);
         if (format_pointer < len_of_columns-1){
            format_pointer++;
         } else {
            format_pointer=0;
         }
      }
   }
   parse_pointer_end = filelen;
   for (int y = parse_pointer_start; y < parse_pointer_end-15; y+=1)
   {
      printf("%02x", (unsigned int)(buffer[y] & 0xff)); 
   }

   printf("\n");

   return 0;
}
