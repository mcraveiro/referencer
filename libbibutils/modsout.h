/*
 * modsout.h
 *
 * Copyright (c) Chris Putnam 2003-7
 *
 * Source code released under the GPL
 *
 */
#ifndef MODSOUT_H
#define MODSOUT_H

/* format-specific options */
#define MODSOUT_DROPKEY (2)
#define MODSOUT_BOM     (4)

#include <stdio.h>
#include <stdlib.h>
#include "bibl.h"

extern void modsout_writeheader( FILE *outptr, int unicode );
extern void modsout_writefooter( FILE *outptr );
extern void modsout_write( fields *info, FILE *outptr,
	int format_opts, unsigned long numrefs );

#endif

