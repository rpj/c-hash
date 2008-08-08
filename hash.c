/* hash.c - my shanty hash table library
 *
 * Yes, I said shanty, and by shanty I imply that this thing sucks, because
 * frankly, it does.  But I wanted to write a hash table, it sounded fun.
 * Bite me.
 */
#include "hash.h"

/* returns a pointer to the new table, or NULL if unsuccesful */
htbl *hash_tbl_init()
{
   int i;
   htbl *t = (htbl *)malloc(sizeof(htbl));

   dprint("In hash_tbl_init...\n");
   for (i = 0; t != NULL && i < HASH_TABLE_SIZE; i++) 
      t->_table[i] = NULL;

   if (t != NULL)
      t->cnt = 0;
   return t;
}

/* we'll take care of memory for you, just MAKE SURE TO
 * CALL THIS before your program ends! */
void hash_tbl_destroy(htbl *t)
{
   int i;
   _bkt *p, *q;
   
   dprint("In hash_tbl_destroy...\n");

   if (t == NULL) return;
   
   for (i = 0; i < HASH_TABLE_SIZE; i++) {
      for (p = t->_table[i]; p != NULL; p = q) {
         q = p->next;
         free(p->key); free(p->data); free(p);
      }

      t->_table[i] = NULL;
   }

   t->cnt = 0;
}

int hash_tbl_ins_single(htbl *t, char *key) { hash_tbl_insert(t, key, key); }

/* returns the hash number of the data just inserted */
int hash_tbl_insert(htbl *t, char *key, char *data) {
   _start_timer();
   int hash = _make_hash(key);
   _bkt *new = (_bkt *)malloc(sizeof(_bkt));;
   
   if (hash_tbl_find(t, key) != NULL)
      return H_ELE_EXIST;

   /* put the data in the list */
   new->key = (char *)malloc(strlen(key) + 1);
   strcpy(new->key, key);

   new->data = (char *)malloc(strlen(data) + 1);
   strcpy(new->data, data);
   
   new->next = t->_table[hash];
   t->_table[hash] = new;
   t->cnt++;
   
   _end_timer("insert");
   return hash;
}

/* returns the data or NULL if not found */
char *hash_tbl_find(htbl *t, char *key) { 
   _start_timer();
   int hash = _make_hash(key);
   _bkt *tmp;

   for (tmp = t->_table[hash]; tmp != NULL; tmp = tmp->next)
      if (tmp->key != NULL && !(strcmp(key, tmp->key)))
         return tmp->data;

   _end_timer("find");
   return NULL;
}

/* returns H_OP_SUCCESS if succesful, H_ELE_NFND otherwise */
int hash_tbl_remove(htbl *t, char *key) {
   _start_timer();
   int hash = _make_hash(key), i;
   _bkt *tmp, *last;

   for (tmp = last = t->_table[hash], i = 1; tmp != NULL;
     last = tmp, tmp = tmp->next, i++) {
      if (tmp->key != NULL && !(strcmp(key, tmp->key))) {
         free (tmp->key); free(tmp->data);

         /* if we're deleting the last element in the list, we need
          * to just set the array element to NULL */
         if (i == 1 && tmp->next == NULL)
            t->_table[hash] = NULL;
         else last->next = tmp->next;

         free(tmp);
         return H_OP_SUCCESS;
      }
   }

   _end_timer("remove");
   return H_ELE_NFND;
}

/* returns H_OP_SUCCESS or H_FILE_OP_ERR, or just fails */
int hash_tbl_write(htbl *t, FILE *f) {
   _start_timer();
   int tint, i;
   long orig_off, more_off;
   char *buf;
   short key_bytes, data_bytes, j;
   _bkt *tmp;
   
   for (i = 0; i < HASH_TABLE_SIZE; i++) {
      orig_off = ftell(f);
      if (fseek(f, 2L, SEEK_CUR))
         return H_FILE_OP_ERR;

      for (tmp = t->_table[i], j = 0; tmp != NULL;
       tmp = tmp->next, j++) {
         key_bytes = (short)(strlen(tmp->key) + 1);
         data_bytes = (short)(strlen(tmp->data) + 1);

         fwrite(&key_bytes, sizeof(key_bytes), 1, f);
         fwrite(&data_bytes, sizeof(data_bytes), 1, f);

         fwrite(tmp->key, (strlen(tmp->key) + 1), 1, f);
         fwrite(tmp->data, (strlen(tmp->data) + 1), 1, f);
      }

      more_off = ftell(f);
      if (fseek(f, orig_off, SEEK_SET))
         return H_FILE_OP_ERR;

      fwrite(&j, sizeof(j), 1, f);
      fseek(f, more_off, SEEK_SET);
   }

   _end_timer("write");
   return H_OP_SUCCESS;
}

/* returns H_OP_SUCCESS or H_NO_MEM which should never happen, or
 * it could just fail and trigger the apocalypse.
 * Oh, if the table read isn't the size of HASH_TABLE_SIZE, then
 * this function returns H_RD_WRONG_SIZE: this is something you 
 * really might want to check for! */
int hash_tbl_read(htbl *t, FILE *f) {
   _start_timer();
   int ele_cnt, master_node_count = 0;
   short node_cnt, i, key_bytes, data_bytes;
   char *key_buf, *data_buf;

   hash_tbl_clear(t);
   
   for (ele_cnt = 0; !feof(f); ele_cnt++) {
      fread(&node_cnt, sizeof(node_cnt), 1, f);
      dprint("Hash element %d has %d nodes\n", ele_cnt, node_cnt);

      for (i = 0; i < node_cnt; i++, master_node_count++) {
         fread(&key_bytes, sizeof(key_bytes), 1, f);
         fread(&data_bytes, sizeof(data_bytes), 1, f);

         if ((key_buf = (char *)malloc(key_bytes)) == NULL)
            return H_NO_MEM;
         fread(key_buf, key_bytes, 1, f);

         if ((data_buf = (char *)malloc(data_bytes)) == NULL)
            return H_NO_MEM;
         fread(data_buf, data_bytes, 1, f);

         dprint("Inserting %s => %s\n", key_buf, data_buf);
         hash_tbl_insert(t, key_buf, data_buf);
      }
   }

   t->cnt = master_node_count;
   _end_timer("read");
   return (ele_cnt - 1) != HASH_TABLE_SIZE ? H_RD_WRONG_SIZE : H_OP_SUCCESS;
}

int hash_tbl_size(htbl *t) { return t->cnt; }

int hash_tbl_clear(htbl *t) { 
   hash_tbl_destroy(t);
   t = hash_tbl_init();
}

/* for internal use only, not intended for API use */
int _make_hash(char *key) {
   _u64 val = 0;

   /* a shantier-than-this-library hashing function that just
    * shifts up by four bits for each character.  Yikes */
   dprint("_make_hash given '%s'\n", key);
   for (; *key; key++)
      val = (val << 4) + (*key);
         
   dprint("Got a hash val of %d\n", (int)(val % HASH_TABLE_SIZE));
   return (int)(val % HASH_TABLE_SIZE);
}

/* prints the table in a somewhat good-looking format to stream f */
void hash_tbl_print(htbl *t, FILE *f) {
   int i;
   _bkt *tmp;

   dprint("hash_table_print using %x\n", t);
   if (f == NULL)
      f = stdout;

   fprintf(f, "This hash table has %d elements total\n", t->cnt);
   fprintf(f, "%7s ", "hash #");
   for (i = 0; i < 8; i++)
      fprintf(f, "[ slot #%d ] ", (i+1));
   fprintf(f, "\n");

   for (i = 0; i < HASH_TABLE_SIZE; i++) {
      fprintf(f, "%7d  ", i);

      for (tmp = t->_table[i]; tmp != NULL; tmp = tmp->next)
         fprintf(f, "%9.9s ", tmp->key);
         
#     ifdef DEBUG
      fprintf(f, "%8.8s\n", "\\0");
#     else
      fprintf(f, "\n");
#     endif
   }
}

char *get_string(FILE *f) {
   char *tmp;
   int ndx = 0;

   if (!(tmp = (char *)malloc(LEN_INCR)))
      return NULL;

   while ((tmp[ndx++] = fgetc(f)) != '\n') {
      if (tmp[ndx] == EOF)
         return NULL;

      if ((ndx % LEN_INCR))
         if (!(tmp = realloc(tmp, (ndx + LEN_INCR))))
            return NULL;
   }

   return tmp;
}

char *chop(char *str) {
   char *new;
   int len = strlen(str);

   new = (char *)malloc(len);
   strncpy(new, str, (len - 1));
   new[len-1] = '\0';
   
   return new;
}

/* $Id: hash.c,v 1.6 2003/07/26 00:41:54 rjoseph Exp $ */

