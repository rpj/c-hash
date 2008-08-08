#include "hash.h"

int main(int argc, char **argv) {
   int i;
   htbl *tbl;
   FILE *f;
   
   tbl = hash_tbl_init();
   hash_tbl_insert(tbl, "foo", "f00");
   hash_tbl_insert(tbl, "bar", "b4r");
   hash_tbl_insert(tbl, "foobar", "f00b4r");
   
   f = fopen("hash.table", "w");
   hash_tbl_write(tbl, f);
   fclose(f);

   printf("Table with one insert after save\n");
   hash_tbl_insert(tbl, "ABCD", "EFGH");
   hash_tbl_print(tbl, NULL);

   f = fopen("hash.table", "r");
   if (hash_tbl_read(tbl, f) == H_RD_WRONG_SIZE)
      printf("Table size read was wrong!\n");
   fclose(f);

   printf("Table read from saved file\n");
   hash_tbl_print(tbl, NULL);

   hash_tbl_destroy(tbl);
   return 0;
}
