#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <place_route.h>
#include <bb_type.h>
#include <device.h>
#include <setup_rabbit.h>
#include "bb_setup.h"

/*********Subroutines************/
int 
set_pr_marco_unplaced(IN t_pr_marco* marco);

int 
set_pr_marco_unstart(IN t_pr_marco* marco);

int 
initial_vnet(INOUT t_vnet* vnet);

int 
initial_marco(INOUT t_pr_marco* marco);

/********************************/

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
 *This function has been annoted.
 *Uncount is an abandoned flag.
 */
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

/*
 *Set single pr_marco unplaced.
 */
int 
set_pr_marco_unplaced(IN t_pr_marco* marco)
{
  marco->status=UNPLACED;
  return 1;
}

/*
 *Set all the pr_marcos unplaced.
 */
int 
set_unplaced_marco(IN int nmarco,
                   IN t_pr_marco* marcos
                  )
{
  int imarco=0;
  for (imarco=0;imarco<nmarco;++imarco)
  {
    set_pr_marco_unplaced(marcos+imarco);
  }
  return 1;
}

/*
 *Set single pr_marco unstart.
 */
int 
set_pr_marco_unstart(IN t_pr_marco* marco)
{
  marco->sstart=UNSTART;
  return 1;
}

/*
 *set all the pr_marcos unstarted.
 */
int 
set_unstart_marco(IN int nmarco,
                  IN t_pr_marco* marcos
                  )
{
  int imarco=0;
  for (imarco=0;imarco<nmarco;++imarco)
  {
    set_pr_marco_unstart(marcos+imarco);
  }
  return 1;
}


/*
 *Set all the pins of the pr_marco unpoint
 */
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
    if (POINTED==marco->pins[ipin]->nets->spwidth)
    {nptred++;}
  }
  return nptred;
}

int 
initial_vnet(INOUT t_vnet* vnet)
{
  int ipin=0;
  boolean special_flag=TRUE;

  vnet->name=UNKNOWN;
  /*Determine the virtual net type*/
  while(ipin<vnet->numpin)
  {
    if (ICBLOCK==vnet->pins[ipin]->parent->type)
    {
      vnet->type=NORMAL;
      special_flag=FALSE;
      break;
    }
	++ipin;
  }
  if (TRUE==special_flag)
  {vnet->type=SPECIAL;}
  /*Initialize those status*/
  vnet->status=UNPLACED;
  vnet->sstart=UNSTART;
  vnet->spwidth=UNPOINT;
  vnet->pcost=0.0;
  vnet->pcolumn=UNKNOWN;
  vnet->pwidth=0;
  vnet->locnum=UNKNOWN;
  vnet->locations=NULL;
  vnet->bbnum=UNKNOWN;
  vnet->bb_nodes=NULL;
  vnet->rstatus=UNROUTE;
  
  return 1;
}

int 
initial_marco(INOUT t_pr_marco* marco)
{
  marco->name=UNKNOWN;
  marco->pcost=0.0;
  marco->pwidth=0;
  marco->status=UNPLACED;
  marco->sstart=UNSTART;
  marco->priority=1;
  marco->detail_offset=UNKNOWN;
  //marco->location={ 0 };
  marco->pcolumn=UNKNOWN;
  marco->rstatus=UNROUTE;
  /*Mike Zou*/
  //marco->label=(char*)malloc(MAX_LABEL_LENGTH*sizeof(char));
  return 1;
}

int 
setup_rabbit(IN int nvnet,
             INOUT t_vnet* vnets,
             IN int nmarco,
             INOUT t_pr_marco* marcos,
             INOUT t_bb_array* bb_array
            )
{
  int inet=0;
  int imarco=0;

  for (inet=0;inet<nvnet;++inet)
  {initial_vnet(&vnets[inet]);}

  for (imarco=0;imarco<nmarco;++imarco) 
  {initial_marco(marcos+imarco);}

  setup_breadboard(bb_array);
  return 1;
}
