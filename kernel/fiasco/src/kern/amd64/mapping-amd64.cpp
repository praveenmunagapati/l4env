INTERFACE [amd64]:

#include "types.h"

class Treemap;

class Mapping_entry
{
public:
  enum { Alignment = 1 };
  union 
  {
    struct 
    {
      unsigned long space:11;	///< Address-space number
      unsigned long _pad:1;
      unsigned long address:36;	///< Virtual address in address space
      unsigned long tag:11;	        ///< Unmap tag
    } data;
    Treemap *_submap;
  };
  Unsigned8 _depth;
} __attribute__((packed));


