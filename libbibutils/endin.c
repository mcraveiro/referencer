/*
 * endin.c
 *
 * Copyright (c) Chris Putnam 2003-7
 *
 * Program and source code released under the GPL
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "is_ws.h"
#include "newstr.h"
#include "newstr_conv.h"
#include "lists.h"
#include "fields.h"
#include "name.h"
#include "title.h"
#include "serialno.h"
#include "reftypes.h"
#include "endin.h"

extern lists asis;
extern lists corps;
extern char progname[];

/* Endnote tag definition:
    character 1 = '%'
    character 2 = alphabetic character or digit
    character 3 = space (ansi 32)
*/
static int
endin_istag( char *buf )
{
	if (! (buf[0]=='%' ) ) return  0;
	if (! (((buf[1]>='A' && buf[1]<='Z'))||(buf[1]>='0'&&buf[1]<='9')||
	        (buf[1]>='a' && buf[1]<='z')|| (buf[1]=='?'||buf[1]=='@'||buf[1]=='!' || buf[1]=='#' || buf[1]=='$' || buf[1]=='&' || buf[1]=='(' || buf[1]==')' || buf[1]=='*' || buf[1]=='+')) ) 
		return 0;
	if (buf[2]!=' ') return 0;
	return 1;
}


static int
readmore( FILE *fp, char *buf, int bufsize, int *bufpos, newstr *line )
{
	if ( line->len ) return 1;
	else return newstr_fget( fp, buf, bufsize, bufpos, line );
}

int
endin_readf( FILE *fp, char *buf, int bufsize, int *bufpos, newstr *line, newstr *reference, int *fcharset )
{
	int haveref = 0, inref = 0;
	unsigned char *up;
	char *p;
	*fcharset = CHARSET_UNKNOWN;
	while ( !haveref && readmore( fp, buf, bufsize, bufpos, line ) ) {
		if ( !line->data ) continue;
		p = &(line->data[0]);

		/* Skip <feff> Unicode header information */
		/* <feff> = ef bb bf */
		up = (unsigned char* ) p;
		if ( *up==239 && *(up+1)==187 && *(up+2)==191 ) {
			*fcharset = CHARSET_UNICODE;
			p += 3;
		}

		if ( !*p ) {
			if ( inref ) haveref = 1; /* blank line separates */
			else continue; /* blank line to ignore */
		}
		/* Each reference starts with a tag && ends with a blank line */
		if ( endin_istag( p ) ) {
			if ( reference->len ) newstr_addchar( reference, '\n' );
			newstr_strcat( reference, p );
			inref = 1;
		} else if ( inref && p ) {
			newstr_addchar( reference, '\n' );
		   	newstr_strcat( reference, p );
		}
		newstr_empty( line );
	}
	if ( reference->len ) haveref = 1;
	return haveref;
}

static char*
process_endline( newstr *tag, newstr *data, char *p )
{
	int  i;

	i = 0;
	while ( i<2 && *p ) {
		newstr_addchar( tag, *p++);
		i++;
	}
	while ( *p==' ' || *p=='\t' ) p++;

	while ( *p && *p!='\r' && *p!='\n' )
		newstr_addchar( data, *p++ );
	newstr_trimendingws( data );

	while ( *p=='\r' || *p=='\n' ) p++;

	return p;
}

static char *
process_endline2( newstr *tag, newstr *data, char *p )
{
	while ( *p==' ' || *p=='\t' ) p++;
	while ( *p && *p!='\r' && *p!='\n' )
		newstr_addchar( data, *p++ );
	newstr_trimendingws( data );
	while ( *p=='\r' || *p=='\n' ) p++;
	return p;
}

int
endin_processf( fields *endin, char *p, char *filename, long nref )
{
	newstr tag, data;
	int n;
	newstr_init( &tag );
	newstr_init( &data );
	while ( *p ) {
		if ( endin_istag( p ) ) {
			p = process_endline( &tag, &data, p );
			/* no empty fields allowed */
			if ( data.len ) {
				fields_add( endin, tag.data, data.data, 0 );
			}
		} else {
			p = process_endline2( &tag, &data, p );
			/* endnote puts %K only on 1st line of keywords */
			n = endin->nfields;
			if ( n>0 && data.len ) {
			if ( !strncmp( endin->tag[n-1].data, "%K", 2 ) ) {
				fields_add( endin, "%K", data.data, 0 );
			} else {
				newstr_addchar( &(endin->data[n-1]), ' ' );
				newstr_strcat( &(endin->data[n-1]), data.data );
			}
			}
		}
		newstr_empty( &tag );
		newstr_empty( &data );
	}
	newstr_free( &tag );
	newstr_free( &data );
	return 1;
}

static void
addtype( fields *info, char *data, int level )
{
	lookups types[] = {
		{ "GENERIC", "ARTICLE" },
		{ "BOOK", "BOOK" }, 
		{ "MANUSCRIPT", "MANUSCRIPT" },
		{ "CONFERENCE PROCEEDINGS", "INPROCEEDINGS"},
		{ "REPORT", "REPORT" },
		{ "COMPUTER PROGRAM", "BOOK" },
		{ "AUDIOVISUAL MATERIAL", "AUDIOVISUAL" },
		{ "ARTWORK", "BOOK" },
		{ "PATENT", "BOOK" },
		{ "BILL", "BILL" },
		{ "CASE", "CASE" },
		{ "JOURNAL ARTICLE", "ARTICLE" }, 
		{ "BOOK SECTION", "INBOOK" },
		{ "EDITED BOOK", "BOOK" },
       		{ "NEWSPAPER ARTICLE",  "NEWSARTICLE" },
		{ "THESIS", "PHDTHESIS" },
		{ "PERSONAL COMMUNICATION", "COMMUNICATION" },
		{ "ELECTRONIC SOURCE", "TEXT" },
		{ "FILM OR BROADCAST", "AUDIOVISUAL" },
		{ "MAP", "MAP" },
		{ "HEARING", "HEARING" },
		{ "STATUTE", "STATUTE" }
	};
	int  ntypes = sizeof( types ) / sizeof( lookups );
	int  i, found=0;
	for ( i=0; i<ntypes; ++i ) {
		if ( !strcasecmp( types[i].oldstr, data ) ) {
			found = 1;
			fields_add( info, "TYPE", types[i].newstr, level );
		}
	}
	if ( !found ) {
		fprintf( stderr, "Did not identify reference type '%s'\n",
			data );
		fprintf( stderr, "Defaulting to journal article type\n");
		fields_add( info, "TYPE", types[0].newstr, level );
	}
}

static void
addpage( fields *info, char *p, int level )
{
	newstr page;
	newstr_init( &page );
	while ( *p && is_ws(*p) ) p++;
	while ( *p && !is_ws(*p) && *p!='-' && *p!='\r' && *p!='\n' ) 
		newstr_addchar( &page, *p++ );
	if ( page.len>0 ) fields_add( info, "PAGESTART", page.data, level );
	newstr_empty( &page );
	while ( *p && (is_ws(*p) || *p=='-' ) ) p++;
	while ( *p && !is_ws(*p) && *p!='-' && *p!='\r' && *p!='\n' ) 
		newstr_addchar( &page, *p++ );
	if ( page.len>0 ) fields_add( info, "PAGEEND", page.data, level );
	newstr_free( &page );
}

static void
adddate( fields *info, char *tag, char *newtag, char *p, int level )
{
	char *months[12]={ "January", "February", "March", "April",
		"May", "June", "July", "August", "September",
		"October", "November", "December" };
	char month[10];
	int found,i,part;
	newstr date;
	newstr_init( &date );
	part = (!strncasecmp(newtag,"PART",4));
	if ( !strcasecmp( tag, "%D" ) ) {
		while ( *p ) newstr_addchar( &date, *p++ );
		if ( date.len>0 ) {
			if ( part ) 
				fields_add(info, "PARTYEAR", date.data, level);
			else
				fields_add( info, "YEAR", date.data, level );
		}
	} else if ( !strcasecmp( tag, "%8" ) ) {
		while ( *p && *p!=' ' && *p!=',' ) newstr_addchar( &date, *p++ );
		if ( date.len>0 ) {
			found = -1;
			for ( i=0; i<12 && found==-1; ++i )
				if ( !strncasecmp( date.data, months[i], 3 ) )
					found = i;
			if ( found!=-1 ) {
				if (found>8) sprintf( month, "%d", found+1 );
				else sprintf( month, "0%d", found+1 );
				if ( part ) 
					fields_add( info, "PARTMONTH", month, level );
				else    fields_add( info, "MONTH", month, level );
			} else {
				if ( part )
					fields_add( info, "PARTMONTH", date.data, level );
				else
					fields_add( info, "MONTH", date.data, level );
			}
		}
		newstr_empty( &date );
		while ( is_ws( *p ) ) p++;
		while ( *p && *p!='\n' && *p!=',' ) newstr_addchar( &date, *p++ );
		if ( date.len>0 && date.len<3 ) {
			if ( part )
				fields_add( info, "PARTDAY", date.data, level );
			else
				fields_add( info, "DAY", date.data, level );
		}
	}
	newstr_free( &date );
}

int
endin_typef( fields *endin, char *filename, int nrefs, variants *all,
		int nall )
{
	char *refnum = "";
	int n, reftype, nrefnum;
	n = fields_find( endin, "%0", 0 );
	nrefnum = fields_find( endin, "%F", 0 );
	if ( nrefnum!=-1 ) refnum = endin->data[nrefnum].data;
	if ( n!=-1 )
		reftype = get_reftype( endin->data[n].data, nrefs, all, nall,
			refnum );
	else
		reftype = get_reftype( "", nrefs, all, nall, refnum ); /* default */
	return reftype;
}

void
endin_convertf( fields *endin, fields *info, int reftype, int verbose, variants *all, int nall )
{
	newstr *d;
	int  i, level, n, process;
	char *newtag, *t;
	for ( i=0; i<endin->nfields; ++i ) {
		t = endin->tag[i].data;
		d = &( endin->data[i] );
		/*
		 * All refer format tags start with '%'.  If we have one
		 * that doesn't, assume that it comes from endx2xml
		 * and just copy and paste to output
		 */
		if ( t[0]!='%' ) {
			fields_add( info, t, d->data, endin->level[i] );
			continue;
		}
		n = process_findoldtag( t, reftype, all, nall );
		if ( n==-1 ) {
			if ( verbose )
				fprintf( stderr, "%s: Cannot find tag '%s'='%s'\n",
					progname, t, d->data );
			continue;
		}
		process = ((all[reftype]).tags[n]).processingtype;
		if ( process == ALWAYS ) continue; /* add these later */
		level = ((all[reftype]).tags[n]).level;
		newtag = ((all[reftype]).tags[n]).newstr;
		if ( process==SIMPLE )
			fields_add( info, newtag, d->data, level );
		else if ( process==TYPE )
			addtype( info, d->data, level );
		else if ( process==TITLE )
			title_process( info, newtag, d->data, level );
		else if ( process==PERSON )
			name_add( info, newtag, d->data, level );
		else if ( process==DATE )
			adddate( info, t, newtag,d->data,level);
		else if ( process==PAGES )
			addpage( info, d->data, level );
		else if ( process==SERIALNO )
			addsn( info, d->data, level );
		else {
/*				fprintf(stderr,"%s: internal error -- illegal process %d\n", r->progname, process );
*/
		}
	}
}
