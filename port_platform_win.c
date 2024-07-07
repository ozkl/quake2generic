
#include <Windows.h>
#include <direct.h>
#include <io.h>

#include "other/glob.h"
#include "game/q_shared.h"

#include "quake2.h"


void QG_Mkdir(const char *path)
{
	_mkdir(path);
}
