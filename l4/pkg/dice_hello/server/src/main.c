#include <stdio.h>

#include <l4/sys/syscalls.h>
#include <l4/names/libnames.h>

#include "hello-server.h"

char LOG_tag[9] = "dice_srv";

int main(void)
{
  names_register("dice_hello_server");
  hello_test_server_loop(NULL);
  return 0;
}

void
hello_test_f1_component(CORBA_Object _dice_corba_obj,
    unsigned long t1,
    unsigned long *t2,
    CORBA_Server_Environment *_dice_corba_env)
{
  printf("hello: f1: %ld\n", t1);
  *t2 = t1 * 10;
}

unsigned short
hello_test_f2_component(CORBA_Object _dice_corba_obj,
    long t1,
    CORBA_Server_Environment *_dice_corba_env)
{
  printf("hello: f2: %ld\n", t1);
  return (l4_int16_t)t1;
}

unsigned long
hello_test_f3_component(CORBA_Object _dice_corba_obj,
    const char* s,
    CORBA_Server_Environment *_dice_corba_env)
{
  printf("f3: %s\n", s);
  return 923756345;
}
