/////////////////////////////////////////////////////////////////////////
// $Id: gui.cc 11302 2012-07-28 08:17:08Z sshwarts $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2012  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


#include <signal.h>
#include "iodev.h"

#if BX_WITH_MACOS
#  include <Disks.h>
#endif

bx_gui_c *bx_gui = NULL;

#define BX_GUI_THIS bx_gui->
#define LOG_THIS BX_GUI_THIS

#define BX_KEY_UNKNOWN 0x7fffffff
#define N_USER_KEYS 37

typedef struct {
  const char *key;
  Bit32u symbol;
} user_key_t;

static user_key_t user_keys[N_USER_KEYS] =
{
  { "f1",    BX_KEY_F1 },
  { "f2",    BX_KEY_F2 },
  { "f3",    BX_KEY_F3 },
  { "f4",    BX_KEY_F4 },
  { "f5",    BX_KEY_F5 },
  { "f6",    BX_KEY_F6 },
  { "f7",    BX_KEY_F7 },
  { "f8",    BX_KEY_F8 },
  { "f9",    BX_KEY_F9 },
  { "f10",   BX_KEY_F10 },
  { "f11",   BX_KEY_F11 },
  { "f12",   BX_KEY_F12 },
  { "alt",   BX_KEY_ALT_L },
  { "bksl",  BX_KEY_BACKSLASH },
  { "bksp",  BX_KEY_BACKSPACE },
  { "ctrl",  BX_KEY_CTRL_L },
  { "del",   BX_KEY_DELETE },
  { "down",  BX_KEY_DOWN },
  { "end",   BX_KEY_END },
  { "enter", BX_KEY_ENTER },
  { "esc",   BX_KEY_ESC },
  { "home",  BX_KEY_HOME },
  { "ins",   BX_KEY_INSERT },
  { "left",  BX_KEY_LEFT },
  { "menu",  BX_KEY_MENU },
  { "minus", BX_KEY_MINUS },
  { "pgdwn", BX_KEY_PAGE_DOWN },
  { "pgup",  BX_KEY_PAGE_UP },
  { "plus",  BX_KEY_KP_ADD },
  { "right", BX_KEY_RIGHT },
  { "shift", BX_KEY_SHIFT_L },
  { "space", BX_KEY_SPACE },
  { "tab",   BX_KEY_TAB },
  { "up",    BX_KEY_UP },
  { "win",   BX_KEY_WIN_L },
  { "print", BX_KEY_PRINT },
  { "power", BX_KEY_POWER_POWER }
};

bx_gui_c::bx_gui_c(void): disp_mode(DISP_MODE_SIM)
{
  put("GUI"); // Init in specific_init
  statusitem_count = 0;
  led_timer_index = BX_NULL_TIMER_HANDLE;
  framebuffer = NULL;
}

bx_gui_c::~bx_gui_c()
{
  if (framebuffer != NULL) {
    delete [] framebuffer;
  }
}

void bx_gui_c::init(int argc, char **argv, unsigned max_xres, unsigned max_yres,
                    unsigned tilewidth, unsigned tileheight)
{
  BX_GUI_THIS new_gfx_api = 0;
  BX_GUI_THIS host_xres = 640;
  BX_GUI_THIS host_yres = 480;
  BX_GUI_THIS host_bpp = 8;
  BX_GUI_THIS max_xres = max_xres;
  BX_GUI_THIS max_yres = max_yres;
  BX_GUI_THIS x_tilesize = tilewidth;
  BX_GUI_THIS y_tilesize = tileheight;
  BX_GUI_THIS dialog_caps = BX_GUI_DLG_RUNTIME | BX_GUI_DLG_SAVE_RESTORE;

  BX_GUI_THIS toggle_method = SIM->get_param_enum(BXPN_MOUSE_TOGGLE)->get();
  BX_GUI_THIS toggle_keystate = 0;

  specific_init(argc, argv, BX_HEADER_BAR_Y);
}

void bx_gui_c::cleanup(void)
{
  statusitem_count = 0;
}

void bx_gui_c::update_drive_status_buttons(void)
{
/* BOX:REMOVE
  BX_GUI_THIS floppyA_status = SIM->get_param_bool(BXPN_FLOPPYA_STATUS)->get();
  BX_GUI_THIS floppyB_status = SIM->get_param_bool(BXPN_FLOPPYB_STATUS)->get();
  Bit32u handle = DEV_hd_get_first_cd_handle();
  BX_GUI_THIS cdrom1_status = DEV_hd_get_cd_media_status(handle);
  if (BX_GUI_THIS floppyA_status)
    replace_bitmap(BX_GUI_THIS floppyA_hbar_id, BX_GUI_THIS floppyA_bmap_id);
  else {
#if BX_WITH_MACOS
    // If we are using the Mac floppy driver, eject the disk
    // from the floppy drive.  This doesn't work in MacOS X.
    if (!strcmp(SIM->get_param_string(BXPN_FLOPPYA_PATH)->getptr(), SuperDrive))
      DiskEject(1);
#endif
    replace_bitmap(BX_GUI_THIS floppyA_hbar_id, BX_GUI_THIS floppyA_eject_bmap_id);
  }
  if (BX_GUI_THIS floppyB_status)
    replace_bitmap(BX_GUI_THIS floppyB_hbar_id, BX_GUI_THIS floppyB_bmap_id);
  else {
#if BX_WITH_MACOS
    // If we are using the Mac floppy driver, eject the disk
    // from the floppy drive.  This doesn't work in MacOS X.
    if (!strcmp(SIM->get_param_string(BXPN_FLOPPYB_PATH)->getptr(), SuperDrive))
      DiskEject(1);
#endif
    replace_bitmap(BX_GUI_THIS floppyB_hbar_id, BX_GUI_THIS floppyB_eject_bmap_id);
  }
  if (BX_GUI_THIS cdrom1_status)
    replace_bitmap(BX_GUI_THIS cdrom1_hbar_id, BX_GUI_THIS cdrom1_bmap_id);
  else {
    replace_bitmap(BX_GUI_THIS cdrom1_hbar_id, BX_GUI_THIS cdrom1_eject_bmap_id);
  }
*/
}

void bx_gui_c::make_text_snapshot(char **snapshot, Bit32u *length)
{
  Bit8u* raw_snap = NULL;
  char *clean_snap;
  unsigned line_addr, txt_addr, txHeight, txWidth;

  DEV_vga_get_text_snapshot(&raw_snap, &txHeight, &txWidth);
  clean_snap = (char*) malloc(txHeight*(txWidth+2)+1);
  txt_addr = 0;
  for (unsigned i=0; i<txHeight; i++) {
    line_addr = i * txWidth * 2;
    for (unsigned j=0; j<(txWidth*2); j+=2) {
      if (!raw_snap[line_addr+j])
        raw_snap[line_addr+j] = 0x20;
      clean_snap[txt_addr++] = raw_snap[line_addr+j];
    }
    while ((txt_addr > 0) && (clean_snap[txt_addr-1] == ' ')) txt_addr--;
    clean_snap[txt_addr++] = 10;
  }
  clean_snap[txt_addr] = 0;
  *snapshot = clean_snap;
  *length = txt_addr;
}

// create a text snapshot and copy to the system clipboard.  On guis that
// we haven't figured out how to support yet, dump to a file instead.
void bx_gui_c::copy_handler(void)
{
  Bit32u len;
  char *text_snapshot;

  if (DEV_vga_get_snapshot_mode() == BX_GUI_SNAPSHOT_TXT) {
    make_text_snapshot (&text_snapshot, &len);
    if (!BX_GUI_THIS set_clipboard_text(text_snapshot, len)) {
      // platform specific code failed, use portable code instead
      FILE *fp = fopen("copy.txt", "w");
      fwrite(text_snapshot, 1, len, fp);
      fclose(fp);
    }
    free(text_snapshot);
  } else {
    BX_ERROR(("copy button failed, mode not implemented"));
  }
}

// create a text snapshot and dump it to a file
void bx_gui_c::snapshot_handler(void)
{
  int fd, i, j, mode, pitch;
  Bit8u *snapshot_ptr = NULL, *palette_ptr = NULL;
  Bit8u *row_buffer, *pixel_ptr, *row_ptr;
  Bit8u bmp_header[54], iBits, b1, b2;
  Bit32u ilen, len, rlen;
  char filename[BX_PATHNAME_LEN];
  unsigned iHeight, iWidth, iDepth;

  mode = DEV_vga_get_snapshot_mode();
  if (mode == BX_GUI_SNAPSHOT_TXT) {
    make_text_snapshot ((char**)&snapshot_ptr, &len);
    if (BX_GUI_THIS dialog_caps & BX_GUI_DLG_SNAPSHOT) {
      int ret = SIM->ask_filename (filename, sizeof(filename),
                                   "Save snapshot as...", "snapshot.txt",
                                   bx_param_string_c::SAVE_FILE_DIALOG);
      if (ret < 0) { // cancelled
        free(snapshot_ptr);
        return;
      }
    } else {
      strcpy (filename, "snapshot.txt");
    }
    FILE *fp = fopen(filename, "wb");
    if (! fp) {
      BX_ERROR(("snapshot button failed: cannot create text file"));
      free(snapshot_ptr);
      return;
    }
    fwrite(snapshot_ptr, 1, len, fp);
    fclose(fp);
    free(snapshot_ptr);
  } else if (mode == BX_GUI_SNAPSHOT_GFX) {
    ilen = DEV_vga_get_gfx_snapshot(&snapshot_ptr, &palette_ptr, &iHeight, &iWidth, &iDepth);
    if (ilen > 0) {
      BX_INFO(("GFX snapshot: %u x %u x %u bpp (%u bytes)", iWidth, iHeight, iDepth, ilen));
    } else {
      BX_ERROR(("snapshot button failed: cannot allocate memory"));
      return;
    }
    if (BX_GUI_THIS dialog_caps & BX_GUI_DLG_SNAPSHOT) {
      int ret = SIM->ask_filename (filename, sizeof(filename),
                                   "Save snapshot as...", "snapshot.bmp",
                                   bx_param_string_c::SAVE_FILE_DIALOG);
      if (ret < 0) { // cancelled
        if (palette_ptr != NULL) free(palette_ptr);
        free(snapshot_ptr);
        return;
      }
    } else {
      strcpy (filename, "snapshot.bmp");
    }
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC
#ifdef O_BINARY
              | O_BINARY
#endif
              , S_IRUSR | S_IWUSR
              );
    if (fd < 0) {
      BX_ERROR(("snapshot button failed: cannot create BMP file"));
      if (palette_ptr != NULL) free(palette_ptr);
      free(snapshot_ptr);
      return;
    }
    iBits = (iDepth == 8) ? 8 : 24;
    rlen = (iWidth * (iBits >> 3) + 3) & ~3;
    len = rlen * iHeight + 54;
    if ((iDepth == 8) && (palette_ptr != NULL)) {
      len += (256 * 4);
    }
    memset(bmp_header, 0, 54);
    bmp_header[0] = 0x42;
    bmp_header[1] = 0x4d;
    bmp_header[2] = len & 0xff;
    bmp_header[3] = (len >> 8) & 0xff;
    bmp_header[4] = (len >> 16) & 0xff;
    bmp_header[5] = (len >> 24) & 0xff;
    bmp_header[10] = 54;
    if ((iDepth == 8) && (palette_ptr != NULL)) {
      bmp_header[11] = 4;
    }
    bmp_header[14] = 40;
    bmp_header[18] = iWidth & 0xff;
    bmp_header[19] = (iWidth >> 8) & 0xff;
    bmp_header[22] = iHeight & 0xff;
    bmp_header[23] = (iHeight >> 8) & 0xff;
    bmp_header[26] = 1;
    bmp_header[28] = iBits;
    write(fd, bmp_header, 54);
    if ((iDepth == 8) && (palette_ptr != NULL)) {
      write(fd, palette_ptr, 256 * 4);
    }
    pitch = iWidth * ((iDepth + 1) >> 3);
    row_buffer = (Bit8u*)malloc(rlen);
    row_ptr = snapshot_ptr + ((iHeight - 1) * pitch);
    for (i = iHeight; i > 0; i--) {
      memset(row_buffer, 0, rlen);
      if ((iDepth == 8) || (iDepth == 24)) {
        memcpy(row_buffer, row_ptr, pitch);
      } else if ((iDepth == 15) || (iDepth == 16)) {
        pixel_ptr = row_ptr;
        for (j = 0; j < (int)(iWidth * 3); j+=3) {
          b1 = *(pixel_ptr++);
          b2 = *(pixel_ptr++);
          *(row_buffer+j)   = (b1 << 3);
          if (iDepth == 15) {
            *(row_buffer+j+1) = ((b1 & 0xe0) >> 2) | (b2 << 6);
            *(row_buffer+j+2) = (b2 & 0x7c) << 1;
          } else {
            *(row_buffer+j+1) = ((b1 & 0xe0) >> 3) | (b2 << 5);
            *(row_buffer+j+2) = (b2 & 0xf8);
          }
        }
      } else if (iDepth == 32) {
        pixel_ptr = row_ptr;
        for (j = 0; j < (int)(iWidth * 3); j+=3) {
          *(row_buffer+j)   = *(pixel_ptr++);
          *(row_buffer+j+1) = *(pixel_ptr++);
          *(row_buffer+j+2) = *(pixel_ptr++);
          pixel_ptr++;
        }
      }
      write(fd, row_buffer, rlen);
      row_ptr -= pitch;
    }
    free(row_buffer);
    close(fd);
    if (palette_ptr != NULL) free(palette_ptr);
    free(snapshot_ptr);
  } else {
    BX_ERROR(("snapshot button failed: unsupported VGA mode"));
  }
}

// Read ASCII chars from the system clipboard and paste them into bochs.
// Note that paste cannot work with the key mapping tables loaded.
void bx_gui_c::paste_handler(void) { }

void bx_gui_c::config_handler(void) { }

void bx_gui_c::toggle_mouse_enable(void) { }

bx_bool bx_gui_c::mouse_toggle_check(Bit32u key, bx_bool pressed) { }

const char* bx_gui_c::get_toggle_info(void) { }

Bit32u get_user_key(char *key) { return BX_KEY_UNKNOWN; }

void bx_gui_c::userbutton_handler(void)
{
  Bit32u shortcut[4];
  Bit32u symbol;
  char user_shortcut[512];
  char *ptr;
  int i, len = 0, ret = 1;

  if (BX_GUI_THIS dialog_caps & BX_GUI_DLG_USER) {
    ret = SIM->ask_param(BXPN_USER_SHORTCUT);
  }
  strcpy(user_shortcut, SIM->get_param_string(BXPN_USER_SHORTCUT)->getptr());
  if ((ret > 0) && user_shortcut[0] && (strcmp(user_shortcut, "none"))) {
    ptr = strtok(user_shortcut, "-");
    if ((strcmp(ptr, SIM->get_param_string(BXPN_USER_SHORTCUT)->getptr())) ||
        (strlen(SIM->get_param_string(BXPN_USER_SHORTCUT)->getptr()) < 6)) {
      while (ptr) {
        symbol = get_user_key(ptr);
        if (symbol == BX_KEY_UNKNOWN) {
          BX_ERROR(("Unknown shortcut %s ignored", ptr));
          return;
        }
        shortcut[len++] = symbol;
        ptr = strtok(NULL, "-");
      }
    } else {
      BX_ERROR(("Unknown shortcut %s ignored", user_shortcut));
      return;
    }
    i = 0;
    while (i < len) {
      DEV_kbd_gen_scancode(shortcut[i++]);
    }
    i--;
    while (i >= 0) {
      DEV_kbd_gen_scancode(shortcut[i--] | BX_KEY_RELEASED);
    }
  }
}

void bx_gui_c::power_handler(void){ };

void bx_gui_c::save_restore_handler(void)
{
  int ret;
  char sr_path[BX_PATHNAME_LEN];

  if (BX_GUI_THIS dialog_caps & BX_GUI_DLG_SAVE_RESTORE) {
    sr_path[0] = 0;
    ret = SIM->ask_filename(sr_path, sizeof(sr_path),
                            "Save Bochs state to folder...", "none",
                            bx_param_string_c::SELECT_FOLDER_DLG);
    if ((ret >= 0) && (strcmp(sr_path, "none"))) {
      if (SIM->save_state(sr_path)) {
        if (!SIM->ask_yes_no("WARNING",
              "The save function currently doesn't handle the state of hard drive images,\n"
              "so we don't recommend to continue, unless you are running a read-only\n"
              "guest system (e.g. Live-CD).\n\n"
              "Do you want to continue?", 0)) {
          power_handler();
        }
      }
    }
  }
}

void bx_gui_c::mouse_enabled_changed(bx_bool val) { }

void bx_gui_c::init_signal_handlers()
{
#if BX_GUI_SIGHANDLER
  if (bx_gui_sighandler)
  {
    Bit32u mask = bx_gui->get_sighandler_mask ();
    for (Bit32u sig=0; sig<32; sig++)
    {
      if (mask & (1<<sig))
        signal (sig, bx_signal_handler);
    }
  }
#endif
}

void bx_gui_c::set_text_charmap(Bit8u *fbuffer)
{
  memcpy(& BX_GUI_THIS vga_charmap, fbuffer, 0x2000);
  for (unsigned i=0; i<256; i++) BX_GUI_THIS char_changed[i] = 1;
  BX_GUI_THIS charmap_updated = 1;
}

void bx_gui_c::set_text_charbyte(Bit16u address, Bit8u data)
{
  BX_GUI_THIS vga_charmap[address] = data;
  BX_GUI_THIS char_changed[address >> 5] = 1;
  BX_GUI_THIS charmap_updated = 1;
}

void bx_gui_c::beep_on(float frequency)
{
  BX_DEBUG(("GUI Beep ON (frequency=%.2f)", frequency));
}

void bx_gui_c::beep_off()
{
  BX_DEBUG(("GUI Beep OFF"));
}

int bx_gui_c::register_statusitem(const char *text, bx_bool auto_off)
{
  if (statusitem_count < BX_MAX_STATUSITEMS) {
    strncpy(statusitem[statusitem_count].text, text, 8);
    statusitem[statusitem_count].text[7] = 0;
    statusitem[statusitem_count].auto_off = auto_off;
    statusitem[statusitem_count].counter = 0;
    statusitem[statusitem_count].active = 0;
    statusitem[statusitem_count].mode = 0;
    return statusitem_count++;
  } else {
   return -1;
  }
}

void bx_gui_c::statusbar_setitem(int element, bx_bool active, bx_bool w)
{
/* BOX:REMOVE
  if (element < 0) {
    for (unsigned i = 0; i < statusitem_count; i++) {
      statusbar_setitem_specific(i, 0, 0);
    }
  } else if ((unsigned)element < statusitem_count) {
    if ((active != statusitem[element].active) ||
        (w != statusitem[element].mode)) {
      statusbar_setitem_specific(element, active, w);
      statusitem[element].active = active;
      statusitem[element].mode = w;
    }
    if (active && statusitem[element].auto_off) {
      statusitem[element].counter = 5;
    }
  }
*/
}

void bx_gui_c::led_timer_handler(void *this_ptr)
{
  bx_gui_c *class_ptr = (bx_gui_c *) this_ptr;
  class_ptr->led_timer();
}

void bx_gui_c::led_timer()
{
  for (unsigned i = 0; i < statusitem_count; i++) {
    if (statusitem[i].auto_off) {
      if (statusitem[i].counter > 0) {
        if (!(--statusitem[i].counter)) {
          statusbar_setitem(i, 0);
        }
      }
    }
  }
}

void bx_gui_c::get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp)
{
  *xres = 1024;
  *yres = 768;
  *bpp = 32;
}

bx_svga_tileinfo_t *bx_gui_c::graphics_tile_info(bx_svga_tileinfo_t *info)
{
  if (!info) {
    info = (bx_svga_tileinfo_t *)malloc(sizeof(bx_svga_tileinfo_t));
    if (!info) {
      return NULL;
    }
  }

  BX_GUI_THIS host_pitch = BX_GUI_THIS host_xres * ((BX_GUI_THIS host_bpp + 1) >> 3);

  info->bpp = BX_GUI_THIS host_bpp;
  info->pitch = BX_GUI_THIS host_pitch;
  switch (info->bpp) {
    case 15:
      info->red_shift = 15;
      info->green_shift = 10;
      info->blue_shift = 5;
      info->red_mask = 0x7c00;
      info->green_mask = 0x03e0;
      info->blue_mask = 0x001f;
      break;
    case 16:
      info->red_shift = 16;
      info->green_shift = 11;
      info->blue_shift = 5;
      info->red_mask = 0xf800;
      info->green_mask = 0x07e0;
      info->blue_mask = 0x001f;
      break;
    case 24:
    case 32:
      info->red_shift = 24;
      info->green_shift = 16;
      info->blue_shift = 8;
      info->red_mask = 0xff0000;
      info->green_mask = 0x00ff00;
      info->blue_mask = 0x0000ff;
      break;
  }
  info->is_indexed = (BX_GUI_THIS host_bpp == 8);
#ifdef BX_LITTLE_ENDIAN
  info->is_little_endian = 1;
#else
  info->is_little_endian = 0;
#endif

  return info;
}

Bit8u *bx_gui_c::graphics_tile_get(unsigned x0, unsigned y0,
                            unsigned *w, unsigned *h)
{
  if (x0+BX_GUI_THIS x_tilesize > BX_GUI_THIS host_xres) {
    *w = BX_GUI_THIS host_xres - x0;
  } else {
    *w = BX_GUI_THIS x_tilesize;
  }

  if (y0+BX_GUI_THIS y_tilesize > BX_GUI_THIS host_yres) {
    *h = BX_GUI_THIS host_yres - y0;
  } else {
    *h = BX_GUI_THIS y_tilesize;
  }

  return (Bit8u *)framebuffer + y0 * BX_GUI_THIS host_pitch +
                  x0 * ((BX_GUI_THIS host_bpp + 1) >> 3);
}

void bx_gui_c::graphics_tile_update_in_place(unsigned x0, unsigned y0,
                                        unsigned w, unsigned h)
{
  Bit8u *tile;
  Bit8u *tile_ptr, *fb_ptr;
  Bit16u xc, yc, fb_pitch, tile_pitch;
  Bit8u r, diffx, diffy;

  tile = new Bit8u[BX_GUI_THIS x_tilesize * BX_GUI_THIS y_tilesize * 4];
  diffx = (x0 % BX_GUI_THIS x_tilesize);
  diffy = (y0 % BX_GUI_THIS y_tilesize);
  if (diffx > 0) {
    x0 -= diffx;
    w += diffx;
  }
  if (diffy > 0) {
    y0 -= diffy;
    h += diffy;
  }
  fb_pitch = BX_GUI_THIS host_pitch;
  tile_pitch = BX_GUI_THIS x_tilesize * ((BX_GUI_THIS host_bpp + 1) >> 3);
  for (yc=y0; yc<(y0+h); yc+=BX_GUI_THIS y_tilesize) {
    for (xc=x0; xc<(x0+w); xc+=BX_GUI_THIS x_tilesize) {
      fb_ptr = BX_GUI_THIS framebuffer + (yc * fb_pitch + xc * ((BX_GUI_THIS host_bpp + 1) >> 3));
      tile_ptr = &tile[0];
      for (r=0; r<h; r++) {
        memcpy(tile_ptr, fb_ptr, tile_pitch);
        fb_ptr += fb_pitch;
        tile_ptr += tile_pitch;
      }
      BX_GUI_THIS graphics_tile_update(tile, xc, yc);
    }
  }
  delete [] tile;
}

void bx_gui_c::show_ips(Bit32u ips_count)
{
#if BX_SHOW_IPS
  BX_INFO(("ips = %3.3fM", ips_count / 1000000.0));
#endif
}

Bit8u bx_gui_c::get_mouse_headerbar_id()
{
  return BX_GUI_THIS mouse_hbar_id;
}

#if BX_DEBUGGER && BX_DEBUGGER_GUI
void bx_gui_c::init_debug_dialog()
{
  extern void InitDebugDialog();
  InitDebugDialog();
}
#endif
