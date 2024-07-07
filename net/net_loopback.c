#include <stdint.h>

#include "../qcommon/qcommon.h"

/*
=============================================================================

LOOPBACK BUFFERS FOR LOCAL PLAYER

=============================================================================
*/



#define	MAX_LOOPBACK	4

typedef struct
{
	byte	data[MAX_MSGLEN];
	int		datalen;
} loopmsg_t;

typedef struct
{
	loopmsg_t	msgs[MAX_LOOPBACK];
	int			get, send;
} loopback_t;

netadr_t	net_local_adr;
loopback_t	loopbacks[2];

qboolean NET_GetLoopPacket(netsrc_t sock, netadr_t* net_from, sizebuf_t* net_message)
{
	int		i;
	loopback_t* loop;

	loop = &loopbacks[sock];

	if (loop->send - loop->get > MAX_LOOPBACK)
		loop->get = loop->send - MAX_LOOPBACK;

	if (loop->get >= loop->send)
		return false;

	i = loop->get & (MAX_LOOPBACK - 1);
	loop->get++;

	memcpy(net_message->data, loop->msgs[i].data, loop->msgs[i].datalen);
	net_message->cursize = loop->msgs[i].datalen;
	*net_from = net_local_adr;
	return true;

}


void NET_SendLoopPacket(netsrc_t sock, int length, void* data, netadr_t to)
{
	int		i;
	loopback_t* loop;

	loop = &loopbacks[sock ^ 1];

	i = loop->send & (MAX_LOOPBACK - 1);
	loop->send++;

	memcpy(loop->msgs[i].data, data, length);
	loop->msgs[i].datalen = length;
}

// -------------

void NET_Init(void)
{
	memset(&net_local_adr, 0, sizeof(netadr_t));
	memset(loopbacks, 0, sizeof(loopback_t));
}

void NET_Shutdown(void)
{

}

void NET_Config(qboolean multiplayer)
{

}

qboolean NET_GetPacket(netsrc_t sock, netadr_t* net_from, sizebuf_t* net_message)
{
	if (NET_GetLoopPacket(sock, net_from, net_message))
		return true;

	return false;
}

void NET_SendPacket(netsrc_t sock, int length, void* data, netadr_t to)
{
	if (to.type == NA_LOOPBACK)
	{
		NET_SendLoopPacket(sock, length, data, to);
		return;
	}

	Com_Printf("Unknown network type: %d\n", to.type);
}

qboolean NET_CompareAdr(netadr_t a, netadr_t b)
{
	if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] && a.port == b.port)
		return true;
	return false;
}

qboolean NET_CompareBaseAdr(netadr_t a, netadr_t b)
{
	if (a.type != b.type)
		return false;

	if (a.type == NA_LOOPBACK)
		return true;

	if (a.type == NA_IP)
	{
		if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3])
			return true;
	}

	return false;
}

qboolean NET_IsLocalAddress(netadr_t adr)
{
	return adr.type == NA_LOOPBACK;
}

char* NET_AdrToString(netadr_t a)
{
	static	char	s[64];

	if (a.type == NA_LOOPBACK)
		Com_sprintf(s, sizeof(s), "loopback");

	return s;
}

qboolean NET_StringToAdr(char* s, netadr_t* a)
{
	if (!strcmp(s, "localhost"))
	{
		memset(a, 0, sizeof(*a));
		a->type = NA_LOOPBACK;
		return true;
	}

	return false;
}

void NET_Sleep(int msec)
{
	
}
