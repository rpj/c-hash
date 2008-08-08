/* db-pipe.c - Create hash tables of pipe-delimited data
 * 
 * This little utility will takes two arguments and input from STDIN.
 * The first argument is how many hashtables to create, which corresponds to
 * how many records you're attempting to store.
 * The second is a base name file, which will be appended with .x, where x
 * is the number of the hash table created.  If you don't provide this, it
 * will dump the tables to STDOUT, but not save them.
 *
 * Send it some pipe-delimited data on STDIN and watch the magic!  The data
 * you send it should be in some type of key-data format.  For example, a
 * three-element file which has two keys is shown in the example below, and
 * you can create two tables, each table having a different key.  It will
 * always use the LAST element in the data as the data argument for tables.
 *
 * example usage: ./db2 2 hash_table < data_file
 * will create hash_table.1 and hash_table.2, and assumes the data is
 * something like:
 * 
 * key1|key2|data
 */
#include "hash.h"

int main(int argc, char **argv) {
   _start_timer();
   char *line, *tok, *tmp;
   FILE *f;
   int i, num_tbls;
   
   if (argc < 2) {
      fprintf(stderr, "Need a number of tables to create!\n");
      exit(-1);
   }

   num_tbls = atoi(argv[1]);

   htbl *tables[num_tbls];
   char *tokens[num_tbls + 1];
   
   for (i = 0; i < num_tbls; i++)
      tables[i] = hash_tbl_init();

   while ((line = get_string(stdin))) {
      line = chop(line);

      tok = strtok(line, "|");
      tokens[0] = (char *)malloc(strlen(tok) + 1);
      strcpy(tokens[0], tok);
      dprint("Got first token: %s\n", tok);
      
      for (i = 1; tok = strtok(NULL, "|"); i++) {
         tokens[i] = (char *)malloc(strlen(tok) + 1);
         strcpy(tokens[i], tok);
         dprint("Got token %d: %s\n", i, tok);
      }
         
      for (i = 0; i < num_tbls; i++)
         hash_tbl_insert(tables[i], tokens[i], tokens[num_tbls]);
   }
  
   if (argc > 2) {
      printf("Saving to files %s.x\n", argv[2]);
      tmp = (char *)malloc(strlen(argv[2]) + 10);

      for (i = 0; i < num_tbls; i++) {
         sprintf(tmp, "%s.%d", argv[2], (i+1));
         printf("Writing to %s...\n", tmp);

         f = fopen(tmp, "w");
         hash_tbl_write(tables[i], f);
         fclose(f);
      }
   } else {
      for (i = 0; i < num_tbls; i++) {
         printf("------------------\n");
         hash_tbl_print(tables[i], NULL);
      }
   }

   for (i = 0; i < num_tbls; i++)
      hash_tbl_destroy(tables[i]);

   _end_timer("main");
   return 0;
}

/* $Id: db-pipe.c,v 1.3 2003/07/26 00:41:54 rjoseph Exp $ */

