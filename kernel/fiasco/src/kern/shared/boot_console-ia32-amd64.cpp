INTERFACE[ia32,amd64]:

class Console;

class Boot_console
{
public:
  static void init();
  static inline Console * cons();

private:
  static Console *_c;
};

IMPLEMENTATION[ia32,amd64]:

#include <cstring>
#include <cstdio>

#include "cmdline.h"
#include "kernel_console.h"
#include "keyb.h"
#include "mux_console.h"
#include "initcalls.h"
#include "static_init.h"
#include "vga_console.h"
#include "mem_layout.h"


Console *Boot_console::_c;


static Console *vga_console()
{
  static Vga_console v(Mem_layout::Adap_vram_cga_beg,80,25,true,true);
  return &v;
}

static Console *herc_console()
{
  static Vga_console v(Mem_layout::Adap_vram_mda_beg,80,25,true,false);
  return &v;
}


STATIC_INITIALIZE_P(Boot_console, BOOT_CONSOLE_INIT_PRIO);

IMPLEMENT FIASCO_INIT
void Boot_console::init()
{
  static Keyb k;
  Kconsole::console()->register_console(&k);

  if( strstr (Cmdline::cmdline(), " -noscreen" ) ) 
      return;

  char const *s;

  Vga_console *c = (Vga_console*)vga_console();
  if(c->is_working())
    Kconsole::console()->register_console(c);
  
  if((s = strstr (Cmdline::cmdline(), " -hercules")) )
    {
      Vga_console *c = (Vga_console*)herc_console();
      if(c->is_working())
	Kconsole::console()->register_console(c);
      else
	puts("Requested hercules console not available!");
    }
};

IMPLEMENT inline
Console * Boot_console::cons()
{
  return _c;
}
