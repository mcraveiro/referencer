/*
 * copacin.c
 *
 * Copyright (c) Chris Putnam 2004-7
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
#include "name.h"
#include "title.h"
#include "fields.h"
#include "reftypes.h"
#include "serialno.h"
#include "copacin.h"

extern lists asis;
extern lists corps;

/* Endnote-Refer/Copac tag definition:
    character 1 = alphabetic character
    character 2 = alphabetic character
    character 3 = dash
    character 4 = space
*/
static int
copacin_istag( char *buf )
{
	if (! ((buf[0]>='A' && buf[0]<='Z')) || (buf[0]>='a' && buf[0]<='z') )
		return 0;
	if (! ((buf[1]>='A' && buf[1]<='Z')) || (buf[1]>='a' && buf[1]<='z') )
		return 0;
	if (buf[2]!='-' ) return 0;
	if (buf[3]!=' ' ) return 0;
	return 1; 
}
static int
readmore( FILE *fp, char *buf, int bufsize, int *bufpos, newstr *line )
{
	if ( line->len ) return 1;
	else return newstr_fget( fp, buf, bufsize, bufpos, line );
}

int
copacin_readf( FILE *fp, char *buf, int bufsize, int *bufpos, newstr *line, newstr *reference, int *fcharset )
{
	int haveref = 0, inref=0;
	char *p;
	while ( !haveref && readmore( fp, buf, bufsize, bufpos, line ) ) {
		/* blank line separates */
		if ( line->data==NULL ) continue;
		if ( inref && line->len==0 ) haveref=1; 
		p = &(line->data[0]);
		if ( copacin_istag( p ) ) {
			if ( inref) newstr_addchar( reference, '\n' );
			newstr_strcat( reference, p );
			newstr_empty( line );
			inref = 1;
		} else if ( inref && p ) {
			/* copac puts tag only on 1st line */
			newstr_addchar( reference, ' ' );
			if ( *p ) p++;
			if ( *p ) p++;
			if ( *p ) p++;
		   	newstr_strcat( reference, p );
			newstr_empty( line );
		}
	}
	*fcharset = CHARSET_UNKNOWN;
	return haveref;
}

static char*
copacin_addtag2( char *p, newstr *tag, newstr *data )
{
	int  i;
	i =0;
	while ( i<3 && *p ) {
		newstr_addchar( tag, *p++ );
		i++;
	}
	while ( *p==' ' || *p=='\t' ) p++;
	while ( *p && *p!='\r' && *p!='\n' ) {
		newstr_addchar( data, *p );
		p++;
	}
	newstr_trimendingws( data );
	while ( *p=='\n' || *p=='\r' ) p++;
	return p;
}

static char *
copacin_nextline( char *p )
{
	while ( *p && *p!='\n' && *p!='\r') p++;
	while ( *p=='\n' || *p=='\r' ) p++;
	return p;
}

int
copacin_processf( fields *copacin, char *p, char *filename, long nref )
{
	newstr tag, data;
	newstr_init( &tag );
	newstr_init( &data );
	while ( *p ) {
		while ( is_ws( *p ) ) p++;
		if ( copacin_istag( p ) ) {
			p = copacin_addtag2( p, &tag, &data );
			/* don't add empty strings */
			if ( tag.len && data.len )
				fields_add( copacin, tag.data, data.data, 0 );
			newstr_empty( &tag );
			newstr_empty( &data );
		}
		else p = copacin_nextline( p );
	}
	newstr_free( &tag );
	newstr_free( &data );
	return 1;
}

/* copac names appear to always start with last name first, but don't
 * always seem to have a comma after the name
 *
 * editors seem to be stuck in as authors with the tag "[Editor]" in it
 */
static void
copacin_addname( fields *info, char *tag, newstr *name, int level )
{
	char *usetag = tag, editor[]="EDITOR", *p;
	int comma = 0;
	if ( strstr( name->data,"[Editor]" ) ) {
		newstr_findreplace( name, "[Editor]", "" );
		usetag = editor;
	}
	p = name->data;
	while ( is_ws( *p ) ) p++;
	while ( *p && !is_ws( *p ) ) {
		if ( *p==',' ) comma++;
		p++;
	}
	if ( !comma && is_ws( *p ) ) *p = ',';
	name_add( info, usetag, name->data, level );
}

static void
copacin_addpage( fields *info, char *p, int level )
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
copacin_adddate( fields *info, char *tag, char *newtag, char *p, int level )
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

void
copacin_convertf( fields *copacin, fields *info, int reftype, int verbose, variants *all, int nall )
{
	newstr *t, *d;
	int  process, level, i, n;
	char *newtag;
	for ( i=0; i<copacin->nfields; ++i ) {
		t = &( copacin->tag[i] );
		d = &( copacin->data[i] );
		n = process_findoldtag( t->data, reftype, all, nall );
		if ( n==-1 ) {
			if ( verbose ) 
				fprintf( stderr, "Cannot find tag '%s'\n",
					t->data );
			continue;
		}
		process = ((all[reftype]).tags[n]).processingtype;
		if ( process == ALWAYS ) continue; /*add these later*/
		level = ((all[reftype]).tags[n]).level;
		newtag = ((all[reftype]).tags[n]).newstr;
		if ( process==SIMPLE )
			fields_add( info, newtag, d->data, level );
		else if ( process==TITLE )
			title_process( info, newtag, d->data, level );
		else if ( process==PERSON )
			copacin_addname( info, newtag, d, level );
		else if ( process==DATE )
			copacin_adddate(info,all[reftype].
					tags[i].oldstr,newtag,d->data,level);
		else if ( process==PAGES )
			copacin_addpage( info, d->data, level );
		else if ( process==SERIALNO )
			addsn( info, d->data, level );
/*		else {
			fprintf(stderr,"%s: internal error -- "
				"illegal process %d\n", r->progname, process );
		}*/
	}
}


