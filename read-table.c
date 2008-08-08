/* read-table.c - Read a binary table and print it to STDOUT
 */
#include "hash.h"

int main(int argc, char **argv) {
   FILE *f;
   htbl *h;

   h = hash_tbl_init();
   f = fopen(argv[1], "r");
   hash_tbl_read(h, f);
   hash_tbl_print(h, NULL);

   hash_tbl_destroy(h);
   return 0;
}

/* $Id: read-table.c,v 1.3 2003/07/26 00:41:54 rjoseph Exp $ */

