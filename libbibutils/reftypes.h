/*
 * reftypes.h
 *
 * Copyright (c) Chris Putnam 2003-5
 *
 * Source code released under the GPL
 *
 */
#ifndef REFTYPES_H
#define REFTYPES_H

#define SIMPLE      (0)
#define TYPE        (1)
#define PERSON      (2)
#define DATE        (3)
#define PAGES       (4)
#define SERIALNO    (5)
#define TITLE       (6)
#define ALWAYS      (7)
#define BIBTEX_URL  (8)
#define ISI_KEYWORD (9)

typedef struct {
	char *oldstr;
	char *newstr;
	int  processingtype;
	int  level;
} lookups;

typedef struct {
	char    type[25];
	lookups *tags;
	int     ntags;
} variants;

extern int get_reftype( char *p, long refnum, variants *all, int nall );
extern int process_findoldtag( char *oldtag, int reftype, variants all[], int nall );


#endif
