# quake2generic
The purpose of Quake 2 generic is to make porting Quake 2 easier.
Now it is possible with just a few functions to port Quake 2 in a single file.

To try it, you will need a baseq2 directory (game data). If you don't own the game, shareware version is freely available.

Original game loads both game and renderer library dynamically to support different mods and rendering engines.
This port embeds these libraries to make porting easier. Because many toy operating systems don't support dynamically loadable libraries (for example: .dll, .so, .dylib).

# porting
Implement these functions to suit your platform as well as renderer frame functions.

* QG_Milliseconds
* QG_GetMouseDiff
* QG_CaptureMouse
* QG_ReleaseMouse
* QG_Mkdir

|Functions            |Description|
|---------------------|-----------|
|QG_Milliseconds      |Measure ticks in milliseconds.
|QG_GetMouseDiff      |If you support mouse, return x and y mouse difference from previous frame.
|QG_CaptureMouse      |If you support mouse, this is used for capturing mouse so that cursor cannot go outside the window.
|QG_ReleaseMouse      |If you support mouse, this is used for releasing mouse when going to menu.
|QG_Mkdir             |Save game depends on this. If you don't implement this, the game still runs.

## renderer - software or gl
Choose a renderer. Software renderer does not have any dependencies and runs everywhere. It is in ref_soft folder.
If your porting target has OpenGL, you can use gl renderer in ref_gl folder.

### software renderer
Implement functions beginning with name SWimp_ (look at port_soft_sdl.c for SDL port).

### gl renderer
Implement functions beginning with name GLimp_ (look at port_gl_sdl.c for SDL port).

### main loop
At start, call Quake2_Init().

In a loop, call Quake2_Frame().

Send key events with Quake2_SendKey().

In simplest form:
```
int main(int argc, char **argv)
{
    int time, oldtime, newtime;
    Quake2_Init(argc, argv);
    oldtime = Quake2_Milliseconds ();
    while (1)
    {
      HandleInput();
      
      do {
        newtime = Quake2_Milliseconds ();
        time = newtime - oldtime;
      } while (time < 1);
      Quake2_Frame(time);
      oldtime = newtime;
    }
    return 0;
}
```

# sound
Null sound is used in Makefile. You can use sound/snd_sdl.c instead of sound/snddma_null.c for SDL port.

# net
Networking works in both unix environment and Windows. There are implementations in net folder.
For systems lacking network support, you can compile net/net_loopback.c.

# platforms
Ported platforms include Windows GDI, SDL (both software and gl),. Just look at (port_soft_gdi.c, port_soft_sdl.c, port_gl_sdl.c).
Provided Makefile works for unix (MacOS and Linux tested). Visual Studio solution is also provided.
