#include <stdio.h>
#include <ctype.h>

#include "../qcommon/qcommon.h"

//===============================================================================

int		hunkcount;


byte	*membase;
int		hunkmaxsize;
int		cursize;

void *Hunk_Begin (int maxsize)
{
	// reserve a huge chunk of memory, but don't commit any yet
	cursize = 0;
	hunkmaxsize = maxsize;

	membase = malloc (maxsize);
	memset (membase, 0, maxsize);

	if (!membase)
		Sys_Error ("reserve failed");
	return (void *)membase;
}

void *Hunk_Alloc (int size)
{
	// round to cacheline
	size = (size+31)&~31;

	cursize += size;
	if (cursize > hunkmaxsize)
		Sys_Error ("Hunk_Alloc overflow");

	return (void *)(membase+cursize-size);
}

int Hunk_End (void)
{
	hunkcount++;
	//Com_Printf ("hunkcount: %i\n", hunkcount);
	return cursize;
}

void Hunk_Free (void *base)
{
	if ( base )
		free (base);

	hunkcount--;
}



//============================================

