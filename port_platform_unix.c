#include <sys/stat.h>

#include "other/glob.h"
#include "game/q_shared.h"

#include "quake2.h"


void QG_Mkdir(const char * path)
{
    mkdir(path, 0777);
}

