#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int main(int argc, char** argv) {
   // arguments: 
   FILE *fileptr;
   char *buffer;
   long filelen;

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
   
   int is_label = 1;
   printf("\"%s\" ",argv[1]);
   for(int i = 39; i < filelen; i++) {
      // printf("%02x ", buffer[i] & 0xff);
      if((buffer[i] & 0xff) == (0xdc)){
         // printf("\nDC header spotted\n");
         i++;
         int new_val = ((buffer[i] & 0xff) << 8) + (buffer[i+1] & 0xff);
         // printf("Number of Entries in the DataList: {%02x%02x}: {%d}\n", buffer[i] & 0xff, buffer[i+1] & 0xff, new_val);
         i++;
      }
      else if(((buffer[i] & 0xff) == 0xde)){
         // printf("\nDE header spotted\n");
         i++;
         // is_label = ((buffer[i] & 0xff) << 8) + (buffer[i+1] & 0xff);
         // printf("Number of Columns in Each Entry: {%02x%02x}: {%d}\n", buffer[i] & 0xff, buffer[i+1] & 0xff, new_val);
         i++;
      }
      else if(((buffer[i] & 0xff) >= 0x80) && ((buffer[i] & 0xff) < 0x90)){
         // printf("\nDE header spotted, type small.\n");
         int size = ((buffer[i] & 0xff)) - 0x80;
         // printf("Number of Columns in Each Entry: {%02x}: {%d}\n", buffer[i] & 0xff, new_val);
      }
      else if(((buffer[i] & 0xff) >= 0x90) && ((buffer[i] & 0xff) < 0xa0)){
         int size = ((buffer[i] & 0xff)) - 0x90;
         // printf("Number of Columns in Each Entry: {%02x}: {%d}\n", buffer[i] & 0xff, new_val);
      }
      else if((((buffer[i] & 0xff) >= (0xa0)) && ((buffer[i] & 0xff) < (0xb0)))){
         int len_of_label = buffer[i] & 0xff - 0xa0;
         // printf("\nA[%d] header spotted\n",len_of_label);
         // printf("Length of Label: {%02x}: {%d}\n", buffer[i] & 0xff, len_of_label);
         int curr_pointer = i;
         int end_pointer = i + len_of_label;
         if ((len_of_label >= 3) && ((buffer[i+1] & 0xff) == 0x6d) && ((buffer[i+2] & 0xff) == 0x5f)) {
            
            if (((buffer[i+3] & 0xff) == 0x69) && ((buffer[i+4] & 0xff) == 0x64) && len_of_label == 4) {
               if (is_label == 0){
                  return 0;
               }
               is_label = 0;
            }
            while (i < end_pointer) {
               printf("%c", buffer[++i] & 0xff);
            }
            printf(" ");
         }
      }
      else if ((buffer[i] & 0xff) == 0xda) {
         i++;
         int len_of_label = ((buffer[i] & 0xff) << 8) + (buffer[i+1] & 0xff);
         // printf("\nDA header spotted\n",len_of_label);
         i++;
         int end_pointer = i + len_of_label;
         if ((len_of_label >= 3) && ((buffer[i+1] & 0xff) == 0x6d) && ((buffer[i+2] & 0xff) == 0x5f)) {
            while (i < end_pointer) {
               printf("%c", buffer[++i] & 0xff);
            }
            printf(" ");
         }
      }
      else if ((buffer[i] & 0xff) == 0xd2) {
         i+=4;
      }
      else if ((buffer[i] & 0xff) == 0xd1) {
         i+=2;
      }
   }
   return 0;
}
// Object: ([Some Label] : [Some Object]) | (Some Object)
// 