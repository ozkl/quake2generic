
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>


#include "../qcommon/qcommon.h"
#include "../client/keys.h"

#include "../quakegeneric.h"


#define K_LAST 256
static unsigned char KeyStates[K_LAST];

void ProcessKeyEvent(int key, qboolean down);

struct
{
  int key;
  int down;
} keyq[64];

int keyq_head = 0;
int keyq_tail = 0;

cvar_t *nostdout = NULL;

extern cvar_t *vid_fullscreen;


unsigned	sys_frame_time;

qboolean stdin_active = false;

// =======================================================================
// General routines
// =======================================================================

void Sys_ConsoleOutput (char *string)
{
	if (nostdout && nostdout->value)
		return;

	fputs(string, stdout);
}

void Sys_Printf (char *fmt, ...)
{
	va_list		argptr;
	char		text[1024];
	unsigned char		*p;

	va_start (argptr,fmt);
	vsprintf (text,fmt,argptr);
	va_end (argptr);

	if (strlen(text) > sizeof(text))
		Sys_Error("memory overwrite in Sys_Printf");

    if (nostdout && nostdout->value)
        return;

	for (p = (unsigned char *)text; *p; p++) {
		*p &= 0x7f;
		if ((*p > 128 || *p < 32) && *p != 10 && *p != 13 && *p != 9)
			printf("[%02x]", *p);
		else
			putc(*p, stdout);
	}
}

void Sys_Quit (void)
{
	CL_Shutdown ();
	Qcommon_Shutdown ();
    //fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);
	_Exit(0);
}

void Sys_Init(void)
{
	memset(KeyStates, 0, sizeof(KeyStates));

#if id386
//	Sys_SetFPCW();
#endif
}

void Sys_Error (char *error, ...)
{ 
    va_list     argptr;
    char        string[1024];

// change stdin to non blocking
    //fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);

	CL_Shutdown ();
	Qcommon_Shutdown ();
    
    va_start (argptr,error);
    vsprintf (string,error,argptr);
    va_end (argptr);
	fprintf(stderr, "Error: %s\n", string);

	_Exit (1);

} 

void Sys_Warn (char *warning, ...)
{ 
    va_list     argptr;
    char        string[1024];
    
    va_start (argptr,warning);
    vsprintf (string,warning,argptr);
    va_end (argptr);
	fprintf(stderr, "Warning: %s", string);
} 


char *Sys_ConsoleInput(void)
{
	return NULL;
}

/*****************************************************************************/


/*
=================
Sys_UnloadGame
=================
*/
void Sys_UnloadGame (void)
{
	
}

/*
=================
Sys_GetGameAPI

Loads the game dll
=================
*/
void *Sys_GetGameAPI (void *parms)
{
	extern void * GetGameAPI(void*);
	return GetGameAPI(parms);
}

/*****************************************************************************/

void Sys_AppActivate (void)
{
}

void Sys_SendKeyEvents (void)
{
#ifndef DEDICATED_ONLY
	static int reenterGuard = 0;

	if (reenterGuard == 0)
	{
		reenterGuard = 1;
		
		//Consume key queue
		while (keyq_head != keyq_tail) {
			ProcessKeyEvent(keyq[keyq_tail].key, keyq[keyq_tail].down);
			keyq_tail = (keyq_tail + 1) & 63;
		}
		
		reenterGuard = 0;
	}
#endif

	// grab frame time 
	sys_frame_time = Sys_Milliseconds();
}

/*****************************************************************************/


char *Sys_GetClipboardData(void)
{
	return NULL;
}


void Sys_CopyProtect(void)
{
}


int curtime;
int Sys_Milliseconds(void)
{
	curtime = QG_Milliseconds();
	return curtime;
}

void Sys_Mkdir(char *path)
{
    QG_Mkdir(path);
}

char * Sys_FindFirst(char *path, unsigned musthave, unsigned canhave)
{
	return NULL;
}

char * Sys_FindNext(unsigned musthave, unsigned canhave)
{
	return NULL;
}

void Sys_FindClose(void)
{
	
}

void ProcessKeyEvent(int key, qboolean down)
{
	if (key >= 0 && key < K_LAST)
	{
		KeyStates[key] = down ? 1 : 0;
	}

	Key_Event(key, down, Sys_Milliseconds());

	if (KeyStates[K_ALT] && key == K_ENTER && down)
	{
		Cvar_SetValue("vid_fullscreen", vid_fullscreen->value ? 0 : 1);
	}
}

void Quake2_Init(int argc, char **argv)
{
	keyq_head = 0;
    keyq_tail = 0;
    memset(keyq, 0, sizeof(keyq));

	Qcommon_Init(argc, argv);

	nostdout = Cvar_Get("nostdout", "0", 0);
}

void Quake2_Frame(int msec)
{
	Qcommon_Frame(msec);
}

int Quake2_Milliseconds()
{
	return Sys_Milliseconds();
}

void Quake2_SendKey(int key, int down)
{
	keyq[keyq_head].key = key;
	keyq[keyq_head].down = down;
	keyq_head = (keyq_head + 1) & 63;
}