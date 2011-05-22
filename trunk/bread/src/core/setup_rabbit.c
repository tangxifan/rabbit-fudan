#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <place_route.h>
#include <bb_type.h>
#include <device.h>



/*
 *To identify whether the vnet is a special net.
 *If the vnet is special, it will be specially
 *considered during placement.
 *The standard is whether the net has none 
 *connection with any ICBLOCK. 
 */
boolean
check_vnet(IN t_vnet* vnet)
{
  int pinno=vnet->numpin;
  int ipin=0;
  for(ipin=0;ipin<pinno;++ipin)
  {
    if (ICBLOCK==vnet->pins[ipin]->parent->type)
    {return FALSE;}
  }
  return TRUE;
}

/*
void
set_uncount_vnets(IN int nvnet,
                  IN t_vnet* vnets
                 )
{
  int inet=0;
  for (inet=0;inet<nvnet;++inet)
  {
    (vnets+inet)->spstatus=UNCOUNT;
  }
  return 1;
}
*/

int
set_unplaced_marco(IN int nmarco,
                   IN t_pr_marco* marcos
                  )
{
  int imarco=0;
  for (imarco=0;imarco<nmarco;++imarco)
  {
    (marcos+imarco)->status=UNPLACED;
  }
  return 1;
}

int
set_unstart_marco(IN int nmarco,
                  IN t_pr_marco* marcos
                  )
{
  int imarco=0;
  for (imarco=0;imarco<nmarco;++imarco)
  {
    (marcos+imarco)->status=UNSTART;
  }
  return 1;
}

int
set_unpoint_marco(IN t_pr_marco* marco)
{
  int ipin=0;
  for (ipin=0;ipin<(marco->pinnum);++ipin)
  {
    marco->pins[ipin]->nets->spwidth=UNPOINT;
  }
  return 1;
}

/*
 *Count the number of pointed pins with
 *given marco.
 */
int
count_pointed_pins(IN t_pr_marco* marco)
{
  int ipin=0;
  int nptred=0;
  for (ipin=0;ipin<(marco->pinnum);++ipin)
  {
    if (POINTED==marco->pins[ipin]->spwidth)
    {nptred++;}
  }
  return nptred;
}

