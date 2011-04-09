#include <stdio.h>
#include <stdlib.h>

#include <rabbit.h>
#include <bb_type.h>
#include <place_route.h>
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
    if (ICBLOCK==vnet->(pins+ipin)->parent->type)
    {return FALSE;}
  }
  return TRUE;
}

/*
 *Check the given vnet and find out the number
 *of connected pins.
 */
int 
check_vnet_match_pin(IN t_vnet *vnet,
                     IN t_pr_marco *marco
                    )
{
  int ipin=0;
  int pinno=vnet->numpin;
  int noconn=0;
  for (ipin=0;ipin<pinno;++ipin)
  {
    if (marco==vnet->(pins+ipin)->parent)
    {noconn++;}
  }
  return noconn;
}

/*
 *With the given pr_pin and pr_marco, we try to 
 *check whether these two are directly
 *connected through a metal wire.
 *The number of connected wires are returned.
 */
int
check_pin_direct_connect(IN t_pr_pin   *src,
                         IN t_pr_marco *marco
                        )
{
  int noconn=0;
  int netno=src->numnet;
  int inet=0;
  t_vnet* tmpnet=NULL;
  for (inet=0;inet<netno;++inet)
  {
    tmpnet=src->nets+inet;
    noconn+=check_vnet_match_pin(tmpnet,marco);
  }
  return noconn;
}

/*
 *Calculate the directed connection criticality
 *between two given marcos.
 */
int 
cal_direct_attract(IN t_pr_marco *src,
                   IN t_pr_marco *des
                  )
{
  int dir_att=0;
  int pinno=src->pinnum;
  int ipin=0;
  t_pr_pin* pintmp=NULL;
  for(ipin=0;ipin<pinno;++ipin)
  {
     pintmp=src->pins+ipin;
     dir_att+=check_pin_direct_connect(pintmp,des);
  }
 return dir_att;
}

/*
 *Check whether the pin belongs a certain type of block
 */
boolean
check_parent_type(IN t_pr_pin *src,
                  IN e_pr_type tcomp
                 )
{
  e_pr_type gottype=src->parent->type;
  if (gottype==tcomp)
  {return TRUE;}
  return FALSE;
}

/*
 *To calculate the direct attractness
 *for indirect connections.
 *Notice that the input pin should be eliminated 
 *during search desitination.
 */
int
cal_indirect_attract(IN t_pr_pin* pin,
                     IN t_pr_marco *des
                    )
{
  int indir_att=0;
  int ipin=0;
  t_pr_marco* marco=pin->parent;
  t_pr_pin *pintmp=NULL;
  
  for(ipin=0;ipin<pinno;++ipin)
  {  
    pintmp=marco->pins+ipin;
    if (pintmp!=pin)
    {indir_att+=check_pin_direct_connect(pintmp,des);}
  }
  return indir_att;
}

/*
 *Find out the indirect connections with the 
 *given vnet and target pr_marco 
 */
int 
find_vnets_indirect(IN t_vnet *net,
                    IN t_pr_marco *des
                   )
{
  int indir_att=0;
  int pinno=net->numpin;
  int ipin=0;
  t_pr_pin *pintmp=NULL;
  
  for(ipin=0;ipin<pinno;++ipin)
  {
    pintmp=net->pins+ipin;
    if((!check_parent_type(pintmp,VDD))&&(!check_parent_type(pintmp,VDD))&&(!check_parent_type(pintmp,VDD)))
    {indir_att+=cal_indirect_attract(pintmp,des);}
  }
  return indir_att;
}

/*
 *Calculate the indirect connection criticality
 *between two given marcos.
 *The indirect connection means there exits only
 *one medium between two given marcos.
 *More than medium exist won't be counted in.
 */
int
cal_indirect_attract(IN t_pr_marco *src,
                     IN t_pr_marco *des
                    )
{
  int indir_att=0;
  int pinno=src->pinnum;
  int netno=0;
  int ipin=0;
  int inet=0;
  t_pr_pin* pintmp=NULL;
  t_vnet* nettmp=NULL;
  for(ipin=0;ipin<pinno;++ipin)
  {
    pintmp=src_pins+ipin;
    netno=pintmp->numnet;
    for{inet=0;inet<netno;++inet}
    {
      nettmp=pintmp->nets+inet;
      indir_att+=find_vnets_indirect(nettmp,des);
    }
  }
  return indir_att;
}

/*
 *In placement, we should decide the block
 *width of each IC block.
 *Rules are set in increasing block width,
 *I. Then block has connected to another block 
 *   with a resistance or capacitance or diode.
 *II. Some pins of the block contain too many 
 *    connections which make it impossible to
 *    route on the bread board. We try to splite
 *    it into several nodes thus increase the block
 *    width.  
 */
int 
cal_block_width(t_pr_marco* marco)
{
  int blkwd;
  
}


