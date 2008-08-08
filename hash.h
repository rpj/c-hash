#ifndef HASH_H
# define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define HASH_TABLE_SIZE    1699 

/* error vals */
#define H_OP_SUCCESS       0
#define H_ELE_EXIST        -2
#define H_ELE_NFND         -4
#define H_NO_MEM           -8
#define H_FILE_OP_ERR      -32
#define H_RD_WRONG_SIZE    -64
#define H_BROKE_ASS_FUNC   -1024

#define LEN_INCR           256

#ifdef DEBUG
# define dprint(fmt, args...) fprintf(stderr, ">> " fmt, ## args); 
#else
# define dprint(fmt, args...)
#endif

#ifdef DO_TIMING
# define _start_timer() \
   tv _tv_s; \
   gettimeofday(&_tv_s, NULL);
# define _end_timer(loc)  \
   tv _tv_e; \
   gettimeofday(&_tv_e, NULL); \
   fprintf(stderr, loc ">\t%d secs\t%lu usecs\n", \
    (_tv_e.tv_sec - _tv_s.tv_sec), (_tv_e.tv_usec - _tv_s.tv_usec));
#else
# define _start_timer() 
# define _end_timer(loc)
#endif

struct hash_table {
   int cnt;
   struct _bucket *_table[HASH_TABLE_SIZE];
};

struct _bucket {
   char *key;
   char *data;
   
   struct _bucket *next;
};

typedef struct hash_table htbl;
typedef unsigned long long _u64;
typedef struct timeval tv;
typedef struct _bucket _bkt;

extern htbl *  hash_tbl_init();
extern void    hash_tbl_destroy(htbl *);
extern int     hash_tbl_ins_single(htbl *, char *);
extern int     hash_tbl_insert(htbl *, char *, char *);
extern char *  hash_tbl_find(htbl *, char *);
extern int     hash_tbl_remove(htbl *, char *);
extern int     hash_tbl_size(htbl *);
extern int     hash_tbl_clear(htbl *);
extern int     hash_tbl_write(htbl *, FILE *);
extern int     hash_tbl_read(htbl *, FILE *);
extern void    hash_tbl_print(htbl *, FILE *);
extern int     _make_hash(char*);
extern char *  chop(char *);
extern char *  get_line(FILE *);

#endif

/* $Id: hash.h,v 1.5 2003/07/26 00:41:54 rjoseph Exp $ */

