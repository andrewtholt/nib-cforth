#include <sys/types.h>
#include <time.h>

struct nlist
{
  char *name;
  char *def;
  int bucket_number;
  time_t updateTime;
  struct nlist *next;
  struct nlist *prev;
};



struct hash_entry
{
  struct nlist *hash_head;
  int ref_count;
  int rec_count;
};

/* Database properties held in flags */

#define FIXED 1
#define DUPLICATE 2
#define FIXED_DB_SIZE 4
#define NEVER_SHRINK 8
#define STAMP 16

struct database
{
  struct hash_entry **hash_table;
  int hashsize;
  unsigned int flags;
  struct nlist *free_rec_list;
  int free_rec_count;
  int max_num_records;	/* only has meaning if FIXED_DB_SIZE set */
  int name_size;
  int def_size;
  int alarm_level;	/* % at which database is nearly full */
};

#define MAX_REC_SIZE 1024
char scratch_buffer[MAX_REC_SIZE];

#define FREE -1
#define IN_USE 0
/*
All databases have four basic functions :-

Create
Retrieve
Update
Delete

So here are mine */

/* Create & update */
struct nlist *install();
void db_update();
/* Retrieve */
struct nlist *find_first();
struct nlist *find_next();
struct nlist *find_first_def();
struct nlist *find_next_def();
struct nlist   *find_first_def();
struct nlist   *find_first_def();

/* Delete */
void delete();

/* Utilities */
struct database *db_create();
void db_setattr();
void db_dump();
void debug_dump();
void bucket_stat();
int db_load();

void db_set_key_len();
void db_set_def_len();

int db_get_key_len();
int db_get_def_len();
