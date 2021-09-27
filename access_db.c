#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "db.h"

int             pop(void);
void            bucket_stats(struct database *);
void            exit();
void            push(int);
void            pre_allocate_records(struct database *);
void           *memcpy();
void            free();
char           *strsave();
struct nlist   *lookup();
struct nlist   *db_install();
int             db_count_records();
void            db_delete();
int             add_to_free();

extern void     db_update();

void 
dbBucketStats(void)
{
	struct database *db;

	db = (struct database *) pop();
	bucket_stats(db);
}

void
dbCreate(void)
{
	int             hashsize;
	struct database *db;
	db = (struct database *) malloc(sizeof(struct database));
	if (!db)
	{
		fprintf(stderr, "dbCreate:Fatal error, faile to allocate memory for db structure\n");
		exit(-1);
	}
	hashsize = pop();
	db = (struct database *) db_create(hashsize);
	push((int) db);
}

void 
dbOccupancy(void)
{
	struct database *db;
	int             occ = 0;

	db = (struct database *) pop();

	occ = ((db->max_num_records - db->free_rec_count) * 100) / db->max_num_records;
	push(occ);
}

void 
dbSetAlarmLevel(void)
{
	struct database *db;
	int             level;

	level = pop();
	if (level > 100)
		level = 100;
	else if (level < 0)
		level = 50;

	db = (struct database *) pop();

	db->alarm_level = level;
}

void 
dbGetAlarmLevel(void)
{
	struct database *db;

	db = (struct database *) pop();
	push((int) db->alarm_level);
}


void
dbSetFlags(void)
{
	int             flags;
	struct database *db;
	flags = pop();
	db = (struct database *) pop();
	db->flags = flags;
}

void
dbSetMaxRecords(void)
{
	int             size;
	struct database *db;

	size = pop();
	db = (struct database *) pop();
	if (db->max_num_records)
		db->max_num_records = size;
	else
	{
		if ((db->name_size == -1) || (db->def_size == -1))
		{
			fprintf(stderr, "\n\tYou must set the size of the name & definition before\n");
			fprintf(stderr, "\tsetting the number of records\n");
			exit(-1);
		} else
		{
			db->max_num_records = size;
			pre_allocate_records(db);
		}
	}
}

void
dbSetNameSize(void)
{
	struct database *db;
	int             size;

	size = pop();
	db = (struct database *) pop();
	db->name_size = size;
}

void
dbSetDefSize(void)
{
	struct database *db;
	int             size;

	size = pop();
	db = (struct database *) pop();
	db->def_size = size;
}

void
dbGetName(void)
{
	struct nlist   *np;
	struct database *db;
	char           *dst;
	int             name_len;

	dst = (char *) pop();
	np = (struct nlist *) pop();
	db = (struct database *) pop();

	if (db->flags & FIXED)
		name_len = db->name_size;
	else
		name_len = strlen(np->name);

	memcpy(dst, np->name, name_len);
	push((int) dst);
}

void
dbGetDef(void)
{
	struct nlist   *np;
	struct database *db;
	char           *dst;
	int             def_len;

	dst = (char *) pop();
	np = (struct nlist *) pop();
	db = (struct database *) pop();

	if (db->flags & FIXED)
		def_len = db->def_size;
	else
		def_len = strlen(np->def);

	memcpy(dst, np->def, def_len);
	push(def_len);
}

void
dbSetName(void)
{
	char           *ptr;
	struct database *db;
	struct nlist   *np;

	ptr = (char *) pop();
	np = (struct nlist *) pop();
	db = (struct database *) pop();
	if (db->flags & FIXED)
		memcpy(np->name, ptr, db->name_size);
	else
	{
		free(np->name);
		np->name = (char *) strsave(ptr);
	}
}

void 
dbSetDef(void)
{
	char           *ptr;
	struct database *db;
	struct nlist   *np;

	ptr = (char *) pop();
	np = (struct nlist *) pop();
	db = (struct database *) pop();
	if (db->flags & FIXED)
		memcpy(np->def, ptr, db->def_size);
	else
	{
		free(np->def);
		np->def = (char *) strsave(ptr);
	}
}

void 
dbUpdate(void)
{
	struct database *db;
	struct nlist   *np;

	char           *def;
	int             def_len;

	np = (struct nlist *) pop();
	def_len = pop();
	def = (char *) pop();
	def[def_len] = (char) NULL;
	db = (struct database *) pop();

	db_update(np, def, db);
}

void 
dbLookup(void)
{
	struct database *db;
	struct nlist   *np;

	char           *key;
	int             key_len;

	key_len = pop();
	key = (char *) pop();

	*(key + key_len) = (char) NULL;

	db = (struct database *) pop();

	np = (struct nlist *) lookup(key, db);

	if (np)
	{
		push((int) np);
		push(0);
	} else
		push(-1);
}

void 
dbDump(void)
{
	char           *filename;
	int             len;
	FILE           *fp;
	struct database *db;

	len = pop();
	filename = (char *) pop();
	filename[len] = (char) NULL;
	db = (struct database *) pop();

	fp = fopen(filename, "w");
	if (fp)
	{
		db_dump(fp, db);
		fclose(fp);
	} else
	{
		fprintf(stderr, "\nFailed to open file\n");
	}
}

void 
dbLoad(void)
{
	char           *filename;
	int             len;
	int             ret;
	struct database *db;


	len = pop();
	filename = (char *) pop();
	filename[len] = (char) NULL;
	db = (struct database *) pop();

	ret = db_load(filename, db);
	push(~ret);
}

void 
dbInstall(void)
{
	int             def_len;
	int             name_len;
	char           *name;
	char           *def;
	struct database *db;
	struct nlist   *np;

	def_len = pop();
	def = (char *) pop();
	def[def_len] = (char) NULL;

	name_len = pop();
	name = (char *) pop();
	name[name_len] = (char) NULL;

	db = (struct database *) pop();
	np = (struct nlist *) db_install(name, def, db);

	if (!np)
		push(-1);
	else
	{
		push((int) np);
		push(0);
	}
}

void 
dbDisplayRecord(void)
{
	struct nlist   *r;
	r = (struct nlist *) pop();
	if (r != NULL)
	{
		printf("\n\r\tName:\t%s\n\r", r->name);
		printf("\tdef:\t%s\r\n", r->def);
	} else
	{
		printf("\r\nEmpty record\r\n");
	}
}


void
dbStatus(void)
{
	struct database *db;
	int             recordCount;

	db = (struct database *) pop();

	recordCount = db_count_records(db);

	printf("\n\r\t\tDatabase status\n\r");
	printf("\n\r\n\rHash value is %d\n\r", db->hashsize);
	if (db->flags & DUPLICATE)
		printf("Duplicate keys\r\n");
	else
		printf("Unique keys\r\n");

	if (db->flags & STAMP)
		printf("Time stamp updates\r\n");
	if (db->flags & FIXED_DB_SIZE)
		printf("Database size fixed\r\n");
	else
		printf("Size variable\r\n");

	if (db->flags & FIXED)
	{
		printf("Fixed Length Records\r\n");
		printf("\tName length = %d\r\n", db->name_size);
		printf("\tDef  length = %d\r\n", db->def_size);

		if (db->flags & FIXED_DB_SIZE)
			printf("\tMax Number of Records = %d\r\n", db->max_num_records);

		printf("\tNumber of records on free list = %d\r\n", db->free_rec_count);
		printf("\tUsed\t\t%3d %%\r\n", (recordCount * 100) / db->max_num_records);
		printf("\tRemaining\t%3d %%\r\n", (db->free_rec_count * 100) / db->max_num_records);
		printf("\tHigh Water\t%3d %%\r\n", db->alarm_level);
	} else
		printf("\nVariable length records\r\n");

	printf("\t%d records in database\r\n", recordCount);

	printf("\r\n\r\n");
}

void 
dbDebugDump(void)
{
	struct database *db;
	db = (struct database *) pop();
	debug_dump(db);
}
void 
dbFindFirstDef(void)
{
	char           *key;
	char            buff[sizeof(key)];

	int             len;
	int             status = 0;
	struct nlist   *rec;
	struct database *db;

	len = pop();
	key = (char *) pop();
	strncpy(buff, key, len);
	buff[len] = (char) NULL;

	db = (struct database *) pop();
	rec = find_first_def(buff, db);
	if (rec)
	{
		push((int) rec);
		status = 0;
	} else
		status = -1;

	push(status);
}

void 
dbFindNextDef(void)
{
	char           *key;
	int             len;
	int             status = 0;
	struct nlist   *rec;
	struct database *db;

	len = pop();
	key = (char *) pop();
	*(key + len) = (char) NULL;

	db = (struct database *) pop();
	rec = find_next_def(key, db);
	if (rec)
	{
		push((int) rec);
		status = 0;
	} else
		status = -1;

	push(status);
}


void 
dbFindFirst(void)
{
	char           *key;
	char            buff[sizeof(key)];

	int             len;
	int             status = 0;
	struct nlist   *rec;
	struct database *db;

	len = pop();
	key = (char *) pop();
	strncpy(buff, key, len);
	buff[len] = (char) NULL;

	db = (struct database *) pop();
	rec = find_first(buff, db);
	if (rec)
	{
		push((int) rec);
		status = 0;
	} else
		status = -1;

	push(status);
}

void 
dbFindNext(void)
{
	char           *key;
	int             len;
	int             status = 0;
	struct nlist   *rec;
	struct database *db;

	len = pop();
	key = (char *) pop();
	*(key + len) = (char) NULL;

	db = (struct database *) pop();
	rec = find_next(key, db);
	if (rec)
	{
		push((int) rec);
		status = 0;
	} else
		status = -1;

	push(status);
}

void 
dbDelete(void)
{
	struct database *db;
	struct nlist   *np;

	np = (struct nlist *) pop();
	db = (struct database *) pop();

	db_delete(np, db);
}

void 
dbGrow(void)
{
	struct database *db;
	int             count;

	count = pop();
	db = (struct database *) pop();
	push((int) add_to_free(db, count));
}
