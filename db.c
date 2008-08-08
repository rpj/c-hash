/* An even shantier program to insert lines of stdin
 * into a hash table ... wow, so cool ...
 */
#include "hash.h"

int main(int argc, char **argv) {
   _start_timer();
   htbl *my_table;
   char *line, *tok;
   FILE *f;

   my_table = hash_tbl_init();
   while ((line = get_string(stdin))) {
      line = chop(line);
      hash_tbl_insert(my_table, line, line);
   }
  
   // print the table to stdout 
   float size = hash_tbl_size(my_table);
   float load = size / HASH_TABLE_SIZE;
   printf("Table has %d records\n", my_table->cnt);
   printf("Hash table load is aprox. %0.3f\n\n", load);

   if (argc > 1) {
      printf("Saving table to %s\n", argv[1]);
      f = fopen(argv[1], "w");
      if (hash_tbl_write(my_table, f) == H_FILE_OP_ERR)
         printf("Couldn't write file!\n");
      fclose(f);
   } else hash_tbl_print(my_table, NULL);
   
   _end_timer("main");
   return 0;
}

/* $Id: db.c,v 1.8 2003/08/06 03:44:16 rjoseph Exp $ */

