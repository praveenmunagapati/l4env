INTERFACE:

#include "console.h"
#include "types.h"

class Filter_console : public Console
{
public:

  Filter_console( Console *o, int to=8000 );
  ~Filter_console();

  int write( char const *str, size_t len );
  int getchar( bool blocking = true );
  int char_avail() const;

private:
  Console *const _o;
  int csi_timeout;
  enum {
    NORMAL,
    UNKNOWN_ESC,
    GOT_CSI, ///< control sequence introducer
  } state;
  unsigned pos;
  char ibuf[32];
  unsigned arg;
  int args[4];
};


IMPLEMENTATION:

#include <cstdio>
#include <cstring>
#include <cctype>


IMPLEMENT 
int Filter_console::char_avail() const
{
  switch(state)
    {
    case NORMAL:
    case UNKNOWN_ESC:
      if(pos) 
	return 1;
      else
	return _o->char_avail();
      
    default:
      return -1;

    } 
}

IMPLEMENT 
Filter_console::Filter_console( Console *o, int to )
  : _o(o), csi_timeout(to), state(NORMAL), pos(0), arg(0)
{}

IMPLEMENT
Filter_console::~Filter_console()
{}

IMPLEMENT
int Filter_console::write( char const *str, size_t len )
{
  char const *start = str;
  char const *stop  = str;

  static char seq[18];
  char const *const home = "\033[H";
  char const *const cel  = "\033[K";
  
  for(;stop < str + len; ++stop ) {
    switch(*stop) {
    case 1:
      if(stop-start)
	_o->write(start, stop-start);
      start = stop + 1;
      _o->write(home,3);
      break;
    case 5:
      if(stop-start)
	_o->write(start, stop-start);
      start = stop + 1;
      _o->write(cel,3);
      break;
    case 6:
      if(stop-start)
	_o->write(start, stop-start);
      if(stop + 2 < str+len) {
	sprintf(seq, "\033[%d;%dH", stop[1]+1,stop[2]+1);
	_o->write(seq,strlen(seq));
      }
      stop += 2;
      start = stop + 1;
      break;
    }
  }

  if(stop-start)
    _o->write(start, stop-start);

  return len;
}

PRIVATE inline
int Filter_console::getchar_timeout( unsigned timeout )
{
  int c;
  while((c= _o->getchar(false)) == -1 && timeout--)
    ;
  return c;
}


IMPLEMENT
int Filter_console::getchar( bool b )
{
  unsigned loop_count = 100;
  int ch;
    
 get_char:
  if(state==UNKNOWN_ESC && pos)
    {
      ch = ibuf[0];
      memmove(ibuf,ibuf+1,--pos);
    }
  else
    ch = _o->getchar(b);

  if(!pos) 
    state = NORMAL;

  if(ch==-1)
    {
      if(state == NORMAL) 
	return -1;
      else if( !b && loop_count-- )
	goto get_char;
      else
	return -1;
    }

  switch(state)
    {
    case UNKNOWN_ESC:
      return ch;

    case NORMAL:
      if(ch==27)
	{
	  ibuf[pos++] = 27;
	  int nc = getchar_timeout(csi_timeout);
	  if(nc==-1)
	    {
	      pos = 0;
	      return 27;
	    }
	  else 
	    {
	      if(pos < sizeof(ibuf))
		ibuf[pos++] = nc;
	      if(nc=='[' || nc=='O')
		{
		  arg = 0;
		  memset(args,0,sizeof(args));
		  state = GOT_CSI;
		  break;
		}
	      else
		{
		  state = UNKNOWN_ESC;
		  goto get_char;
		}
	    }	    
	}
      return ch;

    case GOT_CSI:
      if(isdigit(ch))
	{
	  if(pos < sizeof(ibuf))
	    ibuf[pos++] = ch;

	  if(arg < (sizeof(args)/sizeof(int)))
	    args[arg] = args[arg]*10 + (ch-'0');
	}
      else if(ch==';')
	{
	  if(pos < sizeof(ibuf))
	    ibuf[pos++] = ch;

	  arg++;
	}
      else 
	{
	  state = NORMAL;
	  if(pos < sizeof(ibuf))
	    ibuf[pos++] = ch;
	  
	  switch(ch)
	    {
	    case 'A': 	  
	      pos = 0; return 0x38; break; /* cursor up */
	    case 'B': 
	      pos = 0; return 0x32; break; /* cursor down */
	    case 'C': 
	      pos = 0; return 0x36; break; /* cursor right */
	    case 'D': 
	      pos = 0; return 0x34; break; /* cursor left */
	    case 'H':
	      pos = 0; return 0x37; break; /* cursor home */
	    case 'F':
	      pos = 0; return 0x31; break; /* cursor end */
	    case '~':
	      pos = 0;
	      switch(args[0])
		{
		case 7:
		case 1: return 0x37; break; /* home */
		case 2: return 0x30; break; /* insert */
		case 3: return 0x2e; break; /* delete */
		case 8:
		case 4: return 0x31; break; /* end */
		case 5: return 0x39; break; /* page up */
		case 6: return 0x33; break; /* page down */

		default:
		  arg = 0;
		  if(b)
		    goto get_char;
		  else if(loop_count)
		    {
		      --loop_count;
		      goto get_char;
		    }
		  else
		    return -1;
		    
		}
	    default:
	      state = UNKNOWN_ESC;
	      break;
	    }
	}
      break;
    }

  if(b)
    goto get_char;
  else if (loop_count)
    {
      loop_count --;
      goto get_char;
    }
  else
    return -1;

}


PUBLIC
char const *Filter_console::next_attribute( bool restart = false ) const
{
  return _o->next_attribute(restart);
}


