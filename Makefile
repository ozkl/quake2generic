
SDL_CFLAGS = `sdl2-config --cflags`
SDL_LIBS = `sdl2-config --cflags --libs` -lSDL2_mixer

MOUNT_DIR=.

BUILDDIR=build
CLIENT_DIR=$(MOUNT_DIR)/client
SERVER_DIR=$(MOUNT_DIR)/server
REF_SOFT_DIR=$(MOUNT_DIR)/ref_soft
REF_GL_DIR=$(MOUNT_DIR)/ref_gl
COMMON_DIR=$(MOUNT_DIR)/qcommon
OTHER_DIR=$(MOUNT_DIR)/other
NET_DIR=$(MOUNT_DIR)/net
SOUND_DIR=$(MOUNT_DIR)/sound
NULL_DIR=$(MOUNT_DIR)/null
GAME_DIR=$(MOUNT_DIR)/game
CTF_DIR=$(MOUNT_DIR)/ctf
XATRIX_DIR=$(MOUNT_DIR)/xatrix

CC=clang #emcc
BASE_CFLAGS=-Dstricmp=strcasecmp


RELEASE_CFLAGS=$(BASE_CFLAGS) -O6 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2

DEBUG_CFLAGS=$(BASE_CFLAGS) -g
LDFLAGS=-ldl -lm
XCFLAGS=-I/opt/X11/include

GLLDFLAGS=-L/usr/X11/lib -L/usr/local/lib \
	$(SDL_LIBS) -lGL
GLCFLAGS=$(SDL_CFLAGS)

SHLIBEXT=so

SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_SHLIB_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<
DO_GL_SHLIB_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) $(GLCFLAGS) $(XCFLAGS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################


.PHONY: clean createdirs


build_debug: CFLAGS=$(DEBUG_CFLAGS)
build_debug: createdirs TARGETS

build_release: CFLAGS=$(RELEASE_CFLAGS)
build_release: createdirs TARGETS

createdirs:
	@-mkdir -p $(BUILDDIR) \
		$(BUILDDIR)/client \
		$(BUILDDIR)/ref_soft \
		$(BUILDDIR)/ref_softsdl \
		$(BUILDDIR)/ref_gl \
		$(BUILDDIR)/net \
		$(BUILDDIR)/sound \
		$(BUILDDIR)/game


TARGETS: $(BUILDDIR)/quake2-soft $(BUILDDIR)/quake2-gl

#############################################################################
# CLIENT/SERVER
#############################################################################

QUAKE2_OBJS = \
	$(BUILDDIR)/client/cl_cin.o \
	$(BUILDDIR)/client/cl_ents.o \
	$(BUILDDIR)/client/cl_fx.o \
	$(BUILDDIR)/client/cl_newfx.o \
	$(BUILDDIR)/client/cl_input.o \
	$(BUILDDIR)/client/cl_inv.o \
	$(BUILDDIR)/client/cl_main.o \
	$(BUILDDIR)/client/cl_parse.o \
	$(BUILDDIR)/client/cl_pred.o \
	$(BUILDDIR)/client/cl_tent.o \
	$(BUILDDIR)/client/cl_scrn.o \
	$(BUILDDIR)/client/cl_view.o \
	$(BUILDDIR)/client/console.o \
	$(BUILDDIR)/client/keys.o \
	$(BUILDDIR)/client/menu.o \
	$(BUILDDIR)/client/snd_dma.o \
	$(BUILDDIR)/client/snd_mem.o \
	$(BUILDDIR)/client/snd_mix.o \
	$(BUILDDIR)/client/qmenu.o \
	\
	$(BUILDDIR)/client/cmd.o \
	$(BUILDDIR)/client/cmodel.o \
	$(BUILDDIR)/client/common.o \
	$(BUILDDIR)/client/crc.o \
	$(BUILDDIR)/client/cvar.o \
	$(BUILDDIR)/client/files.o \
	$(BUILDDIR)/client/md4.o \
	$(BUILDDIR)/client/net_chan.o \
	\
	$(BUILDDIR)/client/sv_ccmds.o \
	$(BUILDDIR)/client/sv_ents.o \
	$(BUILDDIR)/client/sv_game.o \
	$(BUILDDIR)/client/sv_init.o \
	$(BUILDDIR)/client/sv_main.o \
	$(BUILDDIR)/client/sv_send.o \
	$(BUILDDIR)/client/sv_user.o \
	$(BUILDDIR)/client/sv_world.o \
	\
	$(BUILDDIR)/client/cd_null.o \
	$(BUILDDIR)/client/q_hunk.o \
	$(BUILDDIR)/client/vid_menu.o \
	$(BUILDDIR)/client/vid_lib.o \
	$(BUILDDIR)/client/q_system.o \
	$(BUILDDIR)/client/glob.o \
	\
	$(BUILDDIR)/client/pmove.o \
	\
	$(BUILDDIR)/net/net_unix.o \
	\
	$(BUILDDIR)/sound/snddma_null.o \
	\
	$(BUILDDIR)/port_platform_unix.o


GAME_OBJS = \
	$(BUILDDIR)/game/g_ai.o \
	$(BUILDDIR)/game/p_client.o \
	$(BUILDDIR)/game/g_cmds.o \
	$(BUILDDIR)/game/g_svcmds.o \
	$(BUILDDIR)/game/g_combat.o \
	$(BUILDDIR)/game/g_func.o \
	$(BUILDDIR)/game/g_items.o \
	$(BUILDDIR)/game/g_main.o \
	$(BUILDDIR)/game/g_misc.o \
	$(BUILDDIR)/game/g_monster.o \
	$(BUILDDIR)/game/g_phys.o \
	$(BUILDDIR)/game/g_save.o \
	$(BUILDDIR)/game/g_spawn.o \
	$(BUILDDIR)/game/g_target.o \
	$(BUILDDIR)/game/g_trigger.o \
	$(BUILDDIR)/game/g_turret.o \
	$(BUILDDIR)/game/g_utils.o \
	$(BUILDDIR)/game/g_weapon.o \
	$(BUILDDIR)/game/m_actor.o \
	$(BUILDDIR)/game/m_berserk.o \
	$(BUILDDIR)/game/m_boss2.o \
	$(BUILDDIR)/game/m_boss3.o \
	$(BUILDDIR)/game/m_boss31.o \
	$(BUILDDIR)/game/m_boss32.o \
	$(BUILDDIR)/game/m_brain.o \
	$(BUILDDIR)/game/m_chick.o \
	$(BUILDDIR)/game/m_flipper.o \
	$(BUILDDIR)/game/m_float.o \
	$(BUILDDIR)/game/m_flyer.o \
	$(BUILDDIR)/game/m_gladiator.o \
	$(BUILDDIR)/game/m_gunner.o \
	$(BUILDDIR)/game/m_hover.o \
	$(BUILDDIR)/game/m_infantry.o \
	$(BUILDDIR)/game/m_insane.o \
	$(BUILDDIR)/game/m_medic.o \
	$(BUILDDIR)/game/m_move.o \
	$(BUILDDIR)/game/m_mutant.o \
	$(BUILDDIR)/game/m_parasite.o \
	$(BUILDDIR)/game/m_soldier.o \
	$(BUILDDIR)/game/m_supertank.o \
	$(BUILDDIR)/game/m_tank.o \
	$(BUILDDIR)/game/p_hud.o \
	$(BUILDDIR)/game/p_trail.o \
	$(BUILDDIR)/game/p_view.o \
	$(BUILDDIR)/game/p_weapon.o \
	$(BUILDDIR)/game/q_shared.o \
	$(BUILDDIR)/game/g_chase.o \
	$(BUILDDIR)/game/m_flash.o

#############################################################################
# REF_SOFT
#############################################################################

REF_SOFT_OBJS = \
	$(BUILDDIR)/ref_soft/r_aclip.o \
	$(BUILDDIR)/ref_soft/r_alias.o \
	$(BUILDDIR)/ref_soft/r_bsp.o \
	$(BUILDDIR)/ref_soft/r_draw.o \
	$(BUILDDIR)/ref_soft/r_edge.o \
	$(BUILDDIR)/ref_soft/r_image.o \
	$(BUILDDIR)/ref_soft/r_light.o \
	$(BUILDDIR)/ref_soft/r_main.o \
	$(BUILDDIR)/ref_soft/r_misc.o \
	$(BUILDDIR)/ref_soft/r_model.o \
	$(BUILDDIR)/ref_soft/r_part.o \
	$(BUILDDIR)/ref_soft/r_poly.o \
	$(BUILDDIR)/ref_soft/r_polyse.o \
	$(BUILDDIR)/ref_soft/r_rast.o \
	$(BUILDDIR)/ref_soft/r_scan.o \
	$(BUILDDIR)/ref_soft/r_sprite.o \
	$(BUILDDIR)/ref_soft/r_surf.o


REF_SOFT_SDL_OBJS = \
	$(BUILDDIR)/ref_soft/port_soft_sdl.o


#############################################################################
# REF_GL
#############################################################################

REF_GL_OBJS = \
	$(BUILDDIR)/ref_gl/gl_draw.o \
	$(BUILDDIR)/ref_gl/gl_image.o \
	$(BUILDDIR)/ref_gl/gl_light.o \
	$(BUILDDIR)/ref_gl/gl_mesh.o \
	$(BUILDDIR)/ref_gl/gl_model.o \
	$(BUILDDIR)/ref_gl/gl_rmain.o \
	$(BUILDDIR)/ref_gl/gl_rmisc.o \
	$(BUILDDIR)/ref_gl/gl_rsurf.o \
	$(BUILDDIR)/ref_gl/gl_warp.o \
	\
	$(BUILDDIR)/ref_gl/qgl_system.o \
	$(BUILDDIR)/ref_gl/port_gl_sdl.o


$(BUILDDIR)/quake2-soft : $(QUAKE2_OBJS) $(GAME_OBJS) $(REF_SOFT_OBJS) $(REF_SOFT_SDL_OBJS)
	$(CC) $(CFLAGS) -o $@ $(QUAKE2_OBJS) $(GAME_OBJS) $(REF_SOFT_OBJS) $(REF_SOFT_SDL_OBJS) $(LDFLAGS) $(GLLDFLAGS)

$(BUILDDIR)/quake2-gl : $(QUAKE2_OBJS) $(GAME_OBJS) $(REF_GL_OBJS)
	$(CC) $(CFLAGS) -o $@ $(QUAKE2_OBJS) $(GAME_OBJS) $(REF_GL_OBJS) $(LDFLAGS) $(GLLDFLAGS)


$(BUILDDIR)/client/cl_cin.o :     $(CLIENT_DIR)/cl_cin.c
	$(DO_CC)

$(BUILDDIR)/client/cl_ents.o :    $(CLIENT_DIR)/cl_ents.c
	$(DO_CC)

$(BUILDDIR)/client/cl_fx.o :      $(CLIENT_DIR)/cl_fx.c
	$(DO_CC)

$(BUILDDIR)/client/cl_newfx.o :      $(CLIENT_DIR)/cl_newfx.c
	$(DO_CC)

$(BUILDDIR)/client/cl_input.o :   $(CLIENT_DIR)/cl_input.c
	$(DO_CC)

$(BUILDDIR)/client/cl_inv.o :     $(CLIENT_DIR)/cl_inv.c
	$(DO_CC)

$(BUILDDIR)/client/cl_main.o :    $(CLIENT_DIR)/cl_main.c
	$(DO_CC)

$(BUILDDIR)/client/cl_parse.o :   $(CLIENT_DIR)/cl_parse.c
	$(DO_CC)

$(BUILDDIR)/client/cl_pred.o :    $(CLIENT_DIR)/cl_pred.c
	$(DO_CC)

$(BUILDDIR)/client/cl_tent.o :    $(CLIENT_DIR)/cl_tent.c
	$(DO_CC)

$(BUILDDIR)/client/cl_scrn.o :    $(CLIENT_DIR)/cl_scrn.c
	$(DO_CC)

$(BUILDDIR)/client/cl_view.o :    $(CLIENT_DIR)/cl_view.c
	$(DO_CC)

$(BUILDDIR)/client/console.o :    $(CLIENT_DIR)/console.c
	$(DO_CC)

$(BUILDDIR)/client/keys.o :       $(CLIENT_DIR)/keys.c
	$(DO_CC)

$(BUILDDIR)/client/menu.o :       $(CLIENT_DIR)/menu.c
	$(DO_CC)

$(BUILDDIR)/client/snd_dma.o :    $(CLIENT_DIR)/snd_dma.c
	$(DO_CC)

$(BUILDDIR)/client/snd_mem.o :    $(CLIENT_DIR)/snd_mem.c
	$(DO_CC)

$(BUILDDIR)/client/snd_mix.o :    $(CLIENT_DIR)/snd_mix.c
	$(DO_CC)

$(BUILDDIR)/client/qmenu.o :      $(CLIENT_DIR)/qmenu.c
	$(DO_CC)

$(BUILDDIR)/client/cmd.o :        $(COMMON_DIR)/cmd.c
	$(DO_CC)

$(BUILDDIR)/client/cmodel.o :     $(COMMON_DIR)/cmodel.c
	$(DO_CC)

$(BUILDDIR)/client/common.o :     $(COMMON_DIR)/common.c
	$(DO_CC)

$(BUILDDIR)/client/crc.o :        $(COMMON_DIR)/crc.c
	$(DO_CC)

$(BUILDDIR)/client/cvar.o :       $(COMMON_DIR)/cvar.c
	$(DO_CC)

$(BUILDDIR)/client/files.o :      $(COMMON_DIR)/files.c
	$(DO_CC)

$(BUILDDIR)/client/md4.o :        $(COMMON_DIR)/md4.c
	$(DO_CC)

$(BUILDDIR)/client/net_chan.o :   $(COMMON_DIR)/net_chan.c
	$(DO_CC)

$(BUILDDIR)/client/pmove.o :      $(COMMON_DIR)/pmove.c
	$(DO_CC)

$(BUILDDIR)/client/sv_ccmds.o :   $(SERVER_DIR)/sv_ccmds.c
	$(DO_CC)

$(BUILDDIR)/client/sv_ents.o :    $(SERVER_DIR)/sv_ents.c
	$(DO_CC)

$(BUILDDIR)/client/sv_game.o :    $(SERVER_DIR)/sv_game.c
	$(DO_CC)

$(BUILDDIR)/client/sv_init.o :    $(SERVER_DIR)/sv_init.c
	$(DO_CC)

$(BUILDDIR)/client/sv_main.o :    $(SERVER_DIR)/sv_main.c
	$(DO_CC)

$(BUILDDIR)/client/sv_send.o :    $(SERVER_DIR)/sv_send.c
	$(DO_CC)

$(BUILDDIR)/client/sv_user.o :    $(SERVER_DIR)/sv_user.c
	$(DO_CC)

$(BUILDDIR)/client/sv_world.o :   $(SERVER_DIR)/sv_world.c
	$(DO_CC)

$(BUILDDIR)/client/cd_null.o :   $(NULL_DIR)/cd_null.c
	$(DO_CC)

$(BUILDDIR)/client/q_hunk.o :  $(OTHER_DIR)/q_hunk.c
	$(DO_CC)

$(BUILDDIR)/client/vid_menu.o :   $(OTHER_DIR)/vid_menu.c
	$(DO_CC)

$(BUILDDIR)/client/vid_lib.o :     $(OTHER_DIR)/vid_lib.c
	$(DO_CC)

$(BUILDDIR)/client/snddma_null.o :  $(NULL_DIR)/snddma_null.c
	$(DO_CC)

$(BUILDDIR)/client/q_system.o :  $(OTHER_DIR)/q_system.c
	$(DO_CC)

$(BUILDDIR)/client/glob.o :       $(OTHER_DIR)/glob.c
	$(DO_CC)

$(BUILDDIR)/net/net_unix.o :    $(NET_DIR)/net_unix.c
	$(DO_CC)

$(BUILDDIR)/port_platform_unix.o :    $(MOUNT_DIR)/port_platform_unix.c
	$(DO_CC)

$(BUILDDIR)/sound/snddma_null.o :    $(SOUND_DIR)/snddma_null.c
	$(DO_GL_SHLIB_CC)


### GAME

$(BUILDDIR)/game/g_ai.o :        $(GAME_DIR)/g_ai.c
	$(DO_CC)

$(BUILDDIR)/game/p_client.o :    $(GAME_DIR)/p_client.c
	$(DO_CC)

$(BUILDDIR)/game/g_cmds.o :      $(GAME_DIR)/g_cmds.c
	$(DO_CC)

$(BUILDDIR)/game/g_svcmds.o :    $(GAME_DIR)/g_svcmds.c
	$(DO_CC)

$(BUILDDIR)/game/g_combat.o :    $(GAME_DIR)/g_combat.c
	$(DO_CC)

$(BUILDDIR)/game/g_func.o :      $(GAME_DIR)/g_func.c
	$(DO_CC)

$(BUILDDIR)/game/g_items.o :     $(GAME_DIR)/g_items.c
	$(DO_CC)

$(BUILDDIR)/game/g_main.o :      $(GAME_DIR)/g_main.c
	$(DO_CC)

$(BUILDDIR)/game/g_misc.o :      $(GAME_DIR)/g_misc.c
	$(DO_CC)

$(BUILDDIR)/game/g_monster.o :   $(GAME_DIR)/g_monster.c
	$(DO_CC)

$(BUILDDIR)/game/g_phys.o :      $(GAME_DIR)/g_phys.c
	$(DO_CC)

$(BUILDDIR)/game/g_save.o :      $(GAME_DIR)/g_save.c
	$(DO_CC)

$(BUILDDIR)/game/g_spawn.o :     $(GAME_DIR)/g_spawn.c
	$(DO_CC)

$(BUILDDIR)/game/g_target.o :    $(GAME_DIR)/g_target.c
	$(DO_CC)

$(BUILDDIR)/game/g_trigger.o :   $(GAME_DIR)/g_trigger.c
	$(DO_CC)

$(BUILDDIR)/game/g_turret.o :    $(GAME_DIR)/g_turret.c
	$(DO_CC)

$(BUILDDIR)/game/g_utils.o :     $(GAME_DIR)/g_utils.c
	$(DO_CC)

$(BUILDDIR)/game/g_weapon.o :    $(GAME_DIR)/g_weapon.c
	$(DO_CC)

$(BUILDDIR)/game/m_actor.o :     $(GAME_DIR)/m_actor.c
	$(DO_CC)

$(BUILDDIR)/game/m_berserk.o :   $(GAME_DIR)/m_berserk.c
	$(DO_CC)

$(BUILDDIR)/game/m_boss2.o :     $(GAME_DIR)/m_boss2.c
	$(DO_CC)

$(BUILDDIR)/game/m_boss3.o :     $(GAME_DIR)/m_boss3.c
	$(DO_CC)

$(BUILDDIR)/game/m_boss31.o :     $(GAME_DIR)/m_boss31.c
	$(DO_CC)

$(BUILDDIR)/game/m_boss32.o :     $(GAME_DIR)/m_boss32.c
	$(DO_CC)

$(BUILDDIR)/game/m_brain.o :     $(GAME_DIR)/m_brain.c
	$(DO_CC)

$(BUILDDIR)/game/m_chick.o :     $(GAME_DIR)/m_chick.c
	$(DO_CC)

$(BUILDDIR)/game/m_flipper.o :   $(GAME_DIR)/m_flipper.c
	$(DO_CC)

$(BUILDDIR)/game/m_float.o :     $(GAME_DIR)/m_float.c
	$(DO_CC)

$(BUILDDIR)/game/m_flyer.o :     $(GAME_DIR)/m_flyer.c
	$(DO_CC)

$(BUILDDIR)/game/m_gladiator.o : $(GAME_DIR)/m_gladiator.c
	$(DO_CC)

$(BUILDDIR)/game/m_gunner.o :    $(GAME_DIR)/m_gunner.c
	$(DO_CC)

$(BUILDDIR)/game/m_hover.o :     $(GAME_DIR)/m_hover.c
	$(DO_CC)

$(BUILDDIR)/game/m_infantry.o :  $(GAME_DIR)/m_infantry.c
	$(DO_CC)

$(BUILDDIR)/game/m_insane.o :    $(GAME_DIR)/m_insane.c
	$(DO_CC)

$(BUILDDIR)/game/m_medic.o :     $(GAME_DIR)/m_medic.c
	$(DO_CC)

$(BUILDDIR)/game/m_move.o :      $(GAME_DIR)/m_move.c
	$(DO_CC)

$(BUILDDIR)/game/m_mutant.o :    $(GAME_DIR)/m_mutant.c
	$(DO_CC)

$(BUILDDIR)/game/m_parasite.o :  $(GAME_DIR)/m_parasite.c
	$(DO_CC)

$(BUILDDIR)/game/m_soldier.o :   $(GAME_DIR)/m_soldier.c
	$(DO_CC)

$(BUILDDIR)/game/m_supertank.o : $(GAME_DIR)/m_supertank.c
	$(DO_CC)

$(BUILDDIR)/game/m_tank.o :      $(GAME_DIR)/m_tank.c
	$(DO_CC)

$(BUILDDIR)/game/p_hud.o :       $(GAME_DIR)/p_hud.c
	$(DO_CC)

$(BUILDDIR)/game/p_trail.o :     $(GAME_DIR)/p_trail.c
	$(DO_CC)

$(BUILDDIR)/game/p_view.o :      $(GAME_DIR)/p_view.c
	$(DO_CC)

$(BUILDDIR)/game/p_weapon.o :    $(GAME_DIR)/p_weapon.c
	$(DO_CC)

$(BUILDDIR)/game/q_shared.o :    $(GAME_DIR)/q_shared.c
	$(DO_CC)

$(BUILDDIR)/game/g_chase.o :    $(GAME_DIR)/g_chase.c
	$(DO_CC)

$(BUILDDIR)/game/m_flash.o :     $(GAME_DIR)/m_flash.c
	$(DO_CC)

#############################################################################
# CTF
#############################################################################

CTF_OBJS = \
	$(BUILDDIR)/ctf/g_ai.o \
	$(BUILDDIR)/ctf/g_chase.o \
	$(BUILDDIR)/ctf/g_cmds.o \
	$(BUILDDIR)/ctf/g_combat.o \
	$(BUILDDIR)/ctf/g_ctf.o \
	$(BUILDDIR)/ctf/g_func.o \
	$(BUILDDIR)/ctf/g_items.o \
	$(BUILDDIR)/ctf/g_main.o \
	$(BUILDDIR)/ctf/g_misc.o \
	$(BUILDDIR)/ctf/g_monster.o \
	$(BUILDDIR)/ctf/g_phys.o \
	$(BUILDDIR)/ctf/g_save.o \
	$(BUILDDIR)/ctf/g_spawn.o \
	$(BUILDDIR)/ctf/g_svcmds.o \
	$(BUILDDIR)/ctf/g_target.o \
	$(BUILDDIR)/ctf/g_trigger.o \
	$(BUILDDIR)/ctf/g_utils.o \
	$(BUILDDIR)/ctf/g_weapon.o \
	$(BUILDDIR)/ctf/m_move.o \
	$(BUILDDIR)/ctf/p_client.o \
	$(BUILDDIR)/ctf/p_hud.o \
	$(BUILDDIR)/ctf/p_menu.o \
	$(BUILDDIR)/ctf/p_trail.o \
	$(BUILDDIR)/ctf/p_view.o \
	$(BUILDDIR)/ctf/p_weapon.o \
	$(BUILDDIR)/ctf/q_shared.o

$(BUILDDIR)/ctf/game.$(SHLIBEXT) : $(CTF_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(CTF_OBJS)

$(BUILDDIR)/ctf/g_ai.o :       $(CTF_DIR)/g_ai.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_chase.o :    $(CTF_DIR)/g_chase.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_cmds.o :     $(CTF_DIR)/g_cmds.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_combat.o :   $(CTF_DIR)/g_combat.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_ctf.o :      $(CTF_DIR)/g_ctf.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_func.o :     $(CTF_DIR)/g_func.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_items.o :    $(CTF_DIR)/g_items.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_main.o :     $(CTF_DIR)/g_main.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_misc.o :     $(CTF_DIR)/g_misc.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_monster.o :  $(CTF_DIR)/g_monster.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_phys.o :     $(CTF_DIR)/g_phys.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_save.o :     $(CTF_DIR)/g_save.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_spawn.o :    $(CTF_DIR)/g_spawn.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_svcmds.o :   $(CTF_DIR)/g_svcmds.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_target.o :   $(CTF_DIR)/g_target.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_trigger.o :  $(CTF_DIR)/g_trigger.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_utils.o :    $(CTF_DIR)/g_utils.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_weapon.o :   $(CTF_DIR)/g_weapon.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/m_move.o :     $(CTF_DIR)/m_move.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_client.o :   $(CTF_DIR)/p_client.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_hud.o :      $(CTF_DIR)/p_hud.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_menu.o :     $(CTF_DIR)/p_menu.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_trail.o :    $(CTF_DIR)/p_trail.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_view.o :     $(CTF_DIR)/p_view.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_weapon.o :   $(CTF_DIR)/p_weapon.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/q_shared.o :   $(CTF_DIR)/q_shared.c
	$(DO_SHLIB_CC)



#############################################################################
# REF_SOFT
#############################################################################


$(BUILDDIR)/ref_soft/r_aclip.o :      $(REF_SOFT_DIR)/r_aclip.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_alias.o :      $(REF_SOFT_DIR)/r_alias.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_bsp.o :        $(REF_SOFT_DIR)/r_bsp.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_draw.o :       $(REF_SOFT_DIR)/r_draw.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_edge.o :       $(REF_SOFT_DIR)/r_edge.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_image.o :      $(REF_SOFT_DIR)/r_image.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_light.o :      $(REF_SOFT_DIR)/r_light.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_main.o :       $(REF_SOFT_DIR)/r_main.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_misc.o :       $(REF_SOFT_DIR)/r_misc.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_model.o :      $(REF_SOFT_DIR)/r_model.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_part.o :       $(REF_SOFT_DIR)/r_part.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_poly.o :       $(REF_SOFT_DIR)/r_poly.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_polyse.o :     $(REF_SOFT_DIR)/r_polyse.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_rast.o :       $(REF_SOFT_DIR)/r_rast.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_scan.o :       $(REF_SOFT_DIR)/r_scan.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_sprite.o :     $(REF_SOFT_DIR)/r_sprite.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_surf.o :       $(REF_SOFT_DIR)/r_surf.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_soft/port_soft_sdl.o :       $(MOUNT_DIR)/port_soft_sdl.c
	$(DO_GL_SHLIB_CC)

#############################################################################
# REF_GL
#############################################################################

$(BUILDDIR)/ref_gl/gl_draw.o :        $(REF_GL_DIR)/gl_draw.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_image.o :       $(REF_GL_DIR)/gl_image.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_light.o :       $(REF_GL_DIR)/gl_light.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_mesh.o :        $(REF_GL_DIR)/gl_mesh.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_model.o :       $(REF_GL_DIR)/gl_model.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_rmain.o :       $(REF_GL_DIR)/gl_rmain.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_rmisc.o :       $(REF_GL_DIR)/gl_rmisc.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_rsurf.o :       $(REF_GL_DIR)/gl_rsurf.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_warp.o :        $(REF_GL_DIR)/gl_warp.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/qgl_system.o :      $(REF_GL_DIR)/qgl_system.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/port_gl_sdl.o :         $(MOUNT_DIR)/port_gl_sdl.c
	$(DO_GL_SHLIB_CC)


#############################################################################
# MISC
#############################################################################

clean:
	-rm -rf \
	$(BUILDDIR)/client \
	$(BUILDDIR)/ref_soft \
	$(BUILDDIR)/ref_softsdl \
	$(BUILDDIR)/ref_gl \
	$(BUILDDIR)/game \
	$(BUILDDIR)/net \
	$(BUILDDIR)/sound \
	$(BUILDDIR)/ctf
	-rm $(BUILDDIR)/quake2*
	-rm $(BUILDDIR)/game*
	-rm $(BUILDDIR)/ref*
	-rm $(BUILDDIR)/*.o