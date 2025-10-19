
#include <stdio.h>

#include "ref_soft/r_local.h"
#include "client/keys.h"

#include "quake2.h"


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <sys/time.h>

#include "nano-X.h"


typedef struct
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} Color;

static Color g_colors[256];

static const int REQUESTED_WIDTH = 640;
static const int REQUESTED_HEIGHT = 480;

static int g_width = REQUESTED_WIDTH;
static int g_height = REQUESTED_HEIGHT;

static char * g_screen_memory = NULL;

#define KEYQUEUE_SIZE 16

static unsigned short g_key_queue[KEYQUEUE_SIZE];
static unsigned int g_key_queue_write_index = 0;
static unsigned int g_key_queue_read_index = 0;
static unsigned char g_key_states[256];

static int g_mouse_diff_x = 0;
static int g_mouse_diff_y = 0;
static int g_mouse_captured = 0;

static GR_WINDOW_ID  g_wid;
static GR_GC_ID      g_gc;
static unsigned int* g_window_buffer = 0;
static int winSizeX = REQUESTED_WIDTH;
static int winSizeY = REQUESTED_HEIGHT;

static void add_key_to_queue(int pressed, unsigned char key)
{
    unsigned short key_data = (pressed << 8) | key;

    g_key_queue[g_key_queue_write_index] = key_data;
    g_key_queue_write_index++;
    g_key_queue_write_index %= KEYQUEUE_SIZE;
}

typedef struct
{
    int fd;
    uint8_t buf[3];
    unsigned idx;
    uint8_t prev_buttons; // bit0=L, bit1=R, bit2=M
} ps2_parser_t;

typedef struct
{
    int dx;          // +right, -left
    int dy;          // +down,  -up  (PS/2 Y is down)
    bool moved;      // true if dx or dy != 0

    // Button transitions for this packet (edge-triggered)
    bool left_pressed,  left_released;
    bool right_pressed, right_released;
    bool middle_pressed, middle_released;
} ps2_event_t;

static void emit_packet(ps2_parser_t *p, uint8_t b0, uint8_t b1, uint8_t b2, ps2_event_t *ev)
{
    // Skip if overflow
    if ((b0 & 0x40) || (b0 & 0x80)) return;

    uint8_t buttons = b0 & 0x07;
    int dx = (int8_t)b1;
    int dy = (int8_t)b2;

    
    ev->dx = dx;
    ev->dy = -dy;
    ev->moved = (dx != 0) || (dy != 0);

    uint8_t changed = buttons ^ p->prev_buttons;
    if (changed & 0x01)
    { // left
        ev->left_pressed  = (buttons & 0x01) != 0;
        ev->left_released = (buttons & 0x01) == 0;
    }
    if (changed & 0x02)
    { // right
        ev->right_pressed  = (buttons & 0x02) != 0;
        ev->right_released = (buttons & 0x02) == 0;
    }
    if (changed & 0x04)
    { // middle
        ev->middle_pressed  = (buttons & 0x04) != 0;
        ev->middle_released = (buttons & 0x04) == 0;
    }

    p->prev_buttons = buttons;
}

void ps2_tick(ps2_parser_t *p, ps2_event_t *ev)
{
    if (!p || p->fd < 0) return;

    for (;;)
    {
        uint8_t byte;
        ssize_t n = read(p->fd, &byte, 1);
        if (n == 0)
        {
            // No data (rare on char dev); stop this tick.
            break;
        }
        else if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // No more bytes right now; stop this tick.
                break;
            }
            if (errno == EINTR) {
                // Try again within the same tick loop.
                continue;
            }
            // Other errors: stop trying this tick.
            break;
        }

        if (p->idx == 0)
        {
            // Resync: first byte must have bit 3 set in standard PS/2 packets.
            if ((byte & 0x08) == 0) continue;
        }

        p->buf[p->idx++] = byte;

        if (p->idx == 3)
        {
            uint8_t b0 = p->buf[0], b1 = p->buf[1], b2 = p->buf[2];
            p->idx = 0;
            emit_packet(p, b0, b1, b2, ev);
        }

        // Keep looping until we run out of bytes (EAGAIN) to avoid blocking.
    }
}

static ps2_parser_t g_psaux_parser;

/*****************************************************************************/

static void setupVideoBuffer()
{
  if (g_screen_memory)
  {
    free(g_screen_memory);
    g_screen_memory = NULL;
  }

  g_screen_memory = malloc(g_width * g_height);

  vid.rowbytes = g_width;
  vid.buffer = g_screen_memory;
}

static unsigned char convert_to_doom_keyx(int wkey)
{
    unsigned char doomkey = 0;

    switch (wkey)
	{
	case MWKEY_ENTER:   doomkey = K_ENTER; break;
	case MWKEY_ESCAPE:   doomkey = K_ESCAPE; break;
	case MWKEY_TAB:      doomkey = K_TAB; break;
	case MWKEY_DOWN: case MWKEY_KP2:     doomkey = K_DOWNARROW; break;
	case MWKEY_UP: case MWKEY_KP8:      doomkey = K_UPARROW; break;
	case MWKEY_LEFT: case MWKEY_KP4:    doomkey = K_LEFTARROW; break;
	case MWKEY_RIGHT: case MWKEY_KP6:   doomkey = K_RIGHTARROW; break;
    case MWKEY_LCTRL: case MWKEY_RCTRL:    doomkey = K_CTRL; break;
    case MWKEY_LSHIFT: case MWKEY_RSHIFT:    doomkey = K_SHIFT; break;
    //case ' ':    doomkey = K_USE; break;
	case MWKEY_F1:  doomkey = K_F1; break;
	case MWKEY_F2:  doomkey = K_F2; break;
	case MWKEY_F3:  doomkey = K_F3; break;
	case MWKEY_F4:  doomkey = K_F4; break;
	case MWKEY_F5:  doomkey = K_F5; break;
	case MWKEY_F6:  doomkey = K_F6; break;
	case MWKEY_F7:  doomkey = K_F7; break;
	case MWKEY_F8:  doomkey = K_F8; break;
	case MWKEY_F9:  doomkey = K_F9; break;
	case MWKEY_F10: doomkey = K_F10; break;
	case MWKEY_F11: doomkey = K_F11; break;
	case MWKEY_F12: doomkey = K_F12; break;
	
	default:
		break;
	}

    if ((wkey >= 'a' && wkey <= 'z') ||
        (wkey >= 'A' && wkey <= 'Z') ||
        (wkey >= '0' && wkey <= '9'))
    {
        doomkey = (uint8_t)wkey;
    }

    return doomkey;
}


rserr_t SWimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
	int width = 0;
  int height = 0;

	ri.Con_Printf( PRINT_ALL, "Initializing OpenGL display\n");

	ri.Con_Printf (PRINT_ALL, "...setting mode %d:", mode );

	if ( !ri.Vid_GetModeInfo( &width, &height, mode ) )
	{
		ri.Con_Printf( PRINT_ALL, " invalid mode\n" );
		return rserr_invalid_mode;
	}

	ri.Con_Printf( PRINT_ALL, " %d %d\n", width, height );


  g_width = width;
  g_height = height;

  *pwidth = width;
	*pheight = height;

  setupVideoBuffer();	

	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow (width, height);

	return rserr_ok;
}

void SWimp_Shutdown( void )
{
  if (g_screen_memory)
  {
    free(g_screen_memory);
    g_screen_memory = NULL;
  }

  exit(0);
}


int SWimp_Init( void *hInstance, void *wndProc )
{
    if (GrOpen() < 0)
    {
        GrError("GrOpen failed");
        return false;
    }

    g_gc = GrNewGC();
    GrSetGCUseBackground(g_gc, GR_FALSE);
    GrSetGCForeground(g_gc, MWRGB( 255, 0, 0 ));

    g_wid = GrNewBufferedWindow(GR_WM_PROPS_APPFRAME |
                        GR_WM_PROPS_CAPTION  |
                        GR_WM_PROPS_CLOSEBOX |
                        GR_WM_PROPS_BUFFER_MMAP |
                        GR_WM_PROPS_BUFFER_BGRA,
                        "Quake 2",
                        GR_ROOT_WINDOW_ID, 
                        50, 50, winSizeX, winSizeY, MWRGB( 255, 255, 255 ));

    GrSelectEvents(g_wid, GR_EVENT_MASK_EXPOSURE | 
                        GR_EVENT_MASK_KEY_UP |
                        GR_EVENT_MASK_KEY_DOWN |
                        GR_EVENT_MASK_TIMER |
                        GR_EVENT_MASK_CLOSE_REQ);

    GrMapWindow (g_wid);

    g_window_buffer = GrOpenClientFramebuffer(g_wid);
    
    setupVideoBuffer();

    return true;
}

static qboolean SWimp_InitGraphics( qboolean fullscreen )
{
  setupVideoBuffer();

  return rserr_ok;
}

void SWimp_SetPalette( const unsigned char *palette )
{
	for (int i=0; i<256; ++i)
    {
        g_colors[i].b = *palette++;
        g_colors[i].g = *palette++;
        g_colors[i].r = *palette++;
        g_colors[i].a = 255;
        palette++;
    }
}

void SWimp_BeginFrame( float camera_seperation )
{
  
}


/*
** GLimp_EndFrame
** 
** Responsible for doing a swapbuffers and possibly for other stuff
** as yet to be determined.  Probably better not to make this a GLimp
** function and instead do a call to GLimp_SwapBuffers.
*/
void SWimp_EndFrame (void)
{
    GR_EVENT event;
    GR_EVENT_KEYSTROKE *kp = NULL;

    unsigned char key = 0;

    while (GrPeekEvent(&event))
    {
        GrGetNextEvent(&event);

        switch (event.type)
        {
        case GR_EVENT_TYPE_CLOSE_REQ:
            GrClose();
            exit (0);
            break;
        case GR_EVENT_TYPE_EXPOSURE:
            break;
        case GR_EVENT_TYPE_KEY_UP:
            kp = (GR_EVENT_KEYSTROKE*)&event;
            key = convert_to_doom_keyx(kp->ch);
            if (key > 0 && key < sizeof(g_key_states))
            {
                unsigned char state = g_key_states[key];
                if (state == 1)
                {
                    g_key_states[key] = 0;
                    add_key_to_queue(0, key);
                }
            }
            break;
        case GR_EVENT_TYPE_KEY_DOWN:
            //this event comes continuously while pressed, so checking for changes only!
            kp = (GR_EVENT_KEYSTROKE*)&event;
            key = convert_to_doom_keyx(kp->ch);
            if (key > 0 && key < sizeof(g_key_states))
            {
                unsigned char state = g_key_states[key];
                if (state == 0)
                {
                    g_key_states[key] = 1;
                    add_key_to_queue(1, key);
                }
            }
            break;
        case GR_EVENT_TYPE_TIMER:
            
            break;
        }
    }

    if (g_window_buffer && g_screen_memory)
    {
        for (int y = 0; y < g_height; ++y)
        {
          for (int x = 0; x < g_width; ++x)
          {
            uint8_t colorIndex = g_screen_memory[y * g_width + x];

            g_window_buffer[y * winSizeX + x] = *(uint32_t*)&g_colors[colorIndex];
          }
        }
    }
    GrFlushWindow(g_wid);
}


void SWimp_AppActivate( qboolean active )
{

}

void get_absolute_window_center_position(GR_WINDOW_ID window_id, GR_COORD *abs_x, GR_COORD *abs_y)
{
    GR_WINDOW_INFO info;
    GR_COORD x = 0, y = 0;
    GR_WINDOW_ID current_id = window_id;
    
    // Traverse up the parent chain to get absolute coordinates
    while (current_id != 0)
    {
        GrGetWindowInfo(current_id, &info);
        x += info.x;
        y += info.y;
        current_id = info.parent;
    }

    x += info.width / 2;
    y += info.height / 2;
    
    *abs_x = x;
    *abs_y = y;
}

void HandleInput(void)
{
    if (g_key_queue_read_index == g_key_queue_write_index)
    {
        //key queue is empty
    }
    else
    {
        unsigned short keyData = g_key_queue[g_key_queue_read_index];
        g_key_queue_read_index++;
        g_key_queue_read_index %= KEYQUEUE_SIZE;

        int pressed = keyData >> 8;
        int quakeKey = keyData & 0xFF;

        Quake2_SendKey(quakeKey, pressed);
    }

    if (g_mouse_captured)
    {
        ps2_event_t ev = {0};
        ps2_tick(&g_psaux_parser, &ev);

        GR_COORD center_x = 0, center_y = 0;
        get_absolute_window_center_position(g_wid, &center_x, &center_y);
        GrMoveCursor(center_x, center_y);

        if (ev.moved)
        {
          g_mouse_diff_x = ev.dx;
          g_mouse_diff_y = ev.dy;
        }

        if (ev.left_pressed)
        {
          Quake2_SendKey(K_MOUSE1 + 0, true);
        }
        if (ev.left_released)
        {
          Quake2_SendKey(K_MOUSE1 + 0, false);
        }
        if (ev.right_pressed)
        {
          Quake2_SendKey(K_MOUSE1 + 1, true);
        }
        if (ev.right_released)
        {
          Quake2_SendKey(K_MOUSE1 + 1, false);
        }
        if (ev.middle_pressed)
        {
          Quake2_SendKey(K_MOUSE1 + 2, true);
        }
        if (ev.middle_released)
        {
          Quake2_SendKey(K_MOUSE1 + 2, false);
        }
    }
}


int QG_Milliseconds(void)
{	
  struct timeval  tp;
  struct timezone tzp;

  gettimeofday(&tp, &tzp);

  return (tp.tv_sec * 1000) + (tp.tv_usec / 1000); /* return milliseconds */
}

void QG_GetMouseDiff(int* dx, int* dy)
{
  *dx = g_mouse_diff_x;
  *dy = g_mouse_diff_y;

  g_mouse_diff_x = 0;
  g_mouse_diff_y = 0;
}

void QG_CaptureMouse(void)
{
  g_mouse_captured = 1;
}

void QG_ReleaseMouse(void)
{
  g_mouse_captured = 0;
}

int main(int argc, char **argv)
{
	int time, oldtime, newtime;

  memset(g_key_states, 0, sizeof(g_key_states));

  memset(&g_psaux_parser, 0, sizeof(g_psaux_parser));
  g_psaux_parser.fd = open("/dev/psaux", O_NONBLOCK);
  

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