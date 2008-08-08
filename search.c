#include "hash.h"

int main(int argc, char **argv) {
   FILE *f;
   htbl *t;
   char *data;

   if (argc < 3)
      exit(-1);
   
   t = hash_tbl_init();
   f = fopen(argv[1], "r");

   hash_tbl_read(t, f);
   data = hash_tbl_find(t, argv[2]);

   printf("Got: %s\n", data);
   return 0;
}
