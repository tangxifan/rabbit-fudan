#include <stdio.h>
#include <stdlib.h>

#include <rabbit.h>
#include <util.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>

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
    {
      noconn++;
      vnet->(pins+ipin)->wstatus=POINTED;
    }
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
  noconn+=check_vnet_match_pin(src->nets,marco);
  return noconn;
}

/*
 *Calculate the directed connection criticality
 *between two given marcos.
 */
int 
find_direct_attract(IN t_pr_marco *src,
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
    if((!check_parent_type(pintmp,ICBLOCK))&&(!check_parent_type(pintmp,GND))&&(!check_parent_type(pintmp,VDD)))
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
find_indirect_attract(IN t_pr_marco *src,
                     IN t_pr_marco *des
                    )
{
  int indir_att=0;
  int pinno=src->pinnum;
  int ipin=0;

  t_pr_pin* pintmp=NULL;

  for(ipin=0;ipin<pinno;++ipin)
  {
    pintmp=src_pins+ipin;
    indir_att+=find_vnets_indirect(pintmp->nets,des);
  }
  return indir_att;
}

/*
 *To get the closeness between two IC blocks, the direct
 *attractness and indirect attractness should be worked out.
 */
int 
find_closeness(IN t_pr_marco *src,
               IN t_pr_marco *des
              )
{
  int closeness=find_indirect_attract(src,des)
                +find_direct_attract(src,des);
  set_unpoint_pins(des);
  return closeness;
}

/*
 *Calculate the vnets qualified.
 *I. The vnet is a special vnet
 *II.The vnet has not been appeared in the list
     vspnets
 */
int
cal_width_parent_spvnet(IN t_pr_pin* inpin,
                        IN int nspvnet,
                        IN t_vnet* vspnets
                       )
{
  int blkwidth=0;
  t_pr_marco* parent=inpin->parent;
  
  int ipin=0;
  int pinno=parent->pinnum; 
  t_pr_pin* pintmp=NULL;
  
  for(ipin=0;ipin<pinno;++ipin)
  {
    pintmp=parent->pins+ipin;
    if ((pintmp!=inpin)&&(SPECIAL==pintmp->nets->type)&&(UNCOUNT==pintmp->nets->width_status))
    {
      blkwidth++;
      pintmp->nets->width_status==COUNTED;
    }
  }
  return blkwidth;
}

/*
 *Complete the work below.
 * Then block has connected to another vnet 
 * with a resistance or capacitance or diode.
 */
int
cal_width_pin_parents(IN t_pr_pin* pintmp,
                      IN int nspvnet,
                      IN t_vnet* vspnets
                      )
{
  int blkwidth=0;
  
  t_vnet* nettmp=pintmp->nets;
  int pinno=nets->numpin;
  int ipin=0;
  t_pr_pin* pintmp=NULL;

  for (ipin=0;ipin<pinno;++ipin)
  {
    pintmp=nettmp->pins+ipin;
    if((!check_parent_type(pintmp,ICBLOCK))&&(!check_parent_type(pintmp,GND))&&(!check_parent_type(pintmp,VDD)))
    {blkwidth+=cal_width_parent_spvnet(pintmp,nspvnet,vspnets);}
  }
  return blkwidth;
}

/*
 *In placement, we should decide the block
 *width of each IC block.
 *Rules are set in increasing block width,
 *I. Then block has connected to another marco 
 *   with a resistance or capacitance or diode.
 *II. Some pins of the block contain too many 
 *    connections which make it impossible to
 *    route on the bread board. We try to splite
 *    it into several nodes thus increase the block
 *    width.
 * III.Then block has connected to another vnet 
 *     with a resistance or capacitance or diode.
 */
int 
cal_block_width(IN t_pr_marco* marco,
                IN int nblk,
                IN t_pr_marco* icblks
                IN int nspvnet,
                IN t_vnet* vspnets,
                IN int wcapacity
               )
{
  int blkwd=marco->device->width;
  int pinno=marco->pinnum;
  int ipin=0;
  int iblk;
  int wextra=0;
 
  t_pr_pin* pintmp=NULL;  
  t_pr_marco* des=NULL;

  for (ipin=0;ipin<pinno;++ipin)
  {
    pintmp=marco->pins+ipin;
    wextra=int((pintmp->nets->numpin-1)/(wcapacity-1));
    blkwd+=wextra;
    blkwd+=cal_width_pin_parents(pintmp,nspvnet,vspnets);
    for(iblk=0;iblk<nblk;++iblk)
    {
      des=icblks+iblk;
      if(des!=marco)&&(UNPLACED==des->status)
      {
        find_indirect_attract(marco,des);
        blkwd+=count_pointed_pins(des);
        set_unpoint_pins(des);
      }
    }
  }
  return blkwd;
}

/*
 *Create a array for storing ic blocks.
 *First, select these qualified ic blocks,
 *Then put them into the array.
 */
void
create_icblk_array(INOUT int* nblk,
                   INOUT t_pr_marco* icblks
                   IN int nmarco,
                   IN t_pr_marco *marcos
                  )
{
  int imarco=0;
  int iblk=0;
  enum e_pr_type mcotype=0;
  (*nblk)=0;
  for (imarco=0;imarco<nmarco;++imarco)
  {
    mcotype=(marcos+imarco)->type;
    if (ICBLOCK==mcotype)
    {iblk++;}
  }

  (*nblk)=iblk;
  icblks=(t_pr_marco*)my_malloc((*nblk)*sizeof(t_pr_marco));

  iblk=0;
  for (imarco=0;imarco<nmarco;++imarco)
  {
    mcotype=(marcos+imarco)->type;
    if (ICBLOCK==mcotype)
    {
      icblks[iblk]=marcos[imarco];
      iblk++;
    }
  }
  return 1;
}

/*
 *Try to figure out the cost of input block
 */
float
find_starter_cost(IN t_pr_marco* blk)
{
  float cost=0.0;
  
  int conn=0;
  int ipin=0;
  int jpin=0;
  int pinnoi=0;
  int pinnoj=0
  t_pr_pin* pintmpi=NULL;
  t_pr_pin* pintmpj=NULL;
  
  pinnoi=blk->pinnum;
  for(ipin=0;ipin<pinnoi;++ipin)
  {
    pintmpi=blk->pins+ipin;
    npinnoj=pintmpi->nets->numpin;
    for(jpin=0;jpin<npinnoj;++jpin)
    {
      pintmpj=pintmpi->nets->pins+jpin;
      if (pintmpk->parent!=blk)
      {conn++;}
    }
  }
  cost=conn/pinno;
  return cost;
}

/*
 *To initialize the placement, we should 
 *choose a starter block.
 *The chosen block satisfy the following conditions.
 *I. The block have the most connections excluding
 *   those are traced to its own pins.
 *II.We try to calculate the place cost with the 
 *   equation below.
 *   Place_Cost=NumConnections/NumPins
 */
t_pr_marco*
find_starter_block(IN int nblk,
                   IN t_pr_marco* blks
                  )
{
  t_pr_marco* blkchn=NULL;
   
  int iblk=0;
  float cost=0.0;
  float max_cost=0.0;

  t_pr_marco* blktmp=NULL;

  for(iblk=0;iblk<nblk;++iblk)
  {
    blktmp=blks+iblk;
    if ((UNPLACED==blktmp->status)&&(UNSTART==blktmp->start))
    {
      cost=find_starter_cost(blktmp);
      if(cost>max_cost)
      {
        max_cost=cost;
        blkchn=blktmp;
      }
    }
  }
  return blkchn; 
}

/*
 *Find a matched IC block during placement
 *Notice that these placed IC blocks are not
 *taken into account.
 */
void
find_match_block(INOUT t_pr_marco* blkmatch,
                 IN int nblk,
                 IN t_pr_marco* icblks
                 )
{
  int iblk=0;
  int jblk=0;
  int max_close=0;
  int close=0;
  t_pr_marco* blkchn=NULL;
  for(iblk=0;iblk<nblk;++iblk)
  {
    if (UNPLACED==(icblks+iblk)->status)
    {
      close=0;
      for(jblk=0;jblk<nblk;++jblk)
      {
        if (PLACED==(icblks+jblk)->status)
        {close+=find_closeness(icblks+jblk,icblks+iblk);}
      }
      if (close>max_close)
      {
        blkchn=(icblks+iblk);
        max_close=close;
      }
    }
  }
  blkmatch=blkchn;
}

/*
 *Do the climbing placement.
 */
void
climbing_place(IN int icolumn,
               IN int bb_pwidth,
               IN int* blkwd,
               IN int nblk,
               INOUT t_pr_marco* icblks,
               IN int nspvnet,
               IN t_vnet* vspnets,
               IN int wcapacity
               )
{
  int iblk=0;
  int widthtmp=0;
  int minwid=0;
  t_pr_marco* blkchn=NULL;
  boolean overflag=TRUE;

  minwid=cal_block_width((iblk+icblks),
                          nblk,
                          icblks,
                          nspvnet,
                          vspnets,
                          wcapacity
                         );
  
  while(1)
  {
    for(iblk=0;iblk<nblk;++iblk)
    {
      if (UNPLACED==(iblk+icblks)->status)
      {
        widthtmp=cal_block_width((iblk+icblks),
                                 nblk,
                                 icblks,
                                 nspvnet,
                                 vspnets,
                                 wcapacity
                                 );
        if (minwid>widthtmp)
        {minwid=widthtmp;blkchn=(iblk+icblks);}
      }
    }
    if ((minwid+(*blkwd))<bb_pwidth)
    {
      blkchn->status=PLACED;
      blkchn->placed_column=icolumn;
      blkchn->place_width=minwid;
      (*blkwd)+=minwid;
      overflag=FALSE;
    }
    if (TRUE==overflag)
    {break;}
  }
  return 1;
}
