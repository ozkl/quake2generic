#ifndef QUAKE2_H
#define QUAKE2_H

#include "game/q_shared.h"

void Quake2_Init(int argc, char **argv);
void Quake2_Frame(int msec);
int Quake2_Milliseconds();
void Quake2_SendKey(int key, qboolean down);

#endif //QUAKE2_H