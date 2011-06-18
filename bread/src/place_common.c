#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>

/*
 *With the given pr_pin and pr_marco, we try to 
 *check whether these two are directly
 *connected through a metal wire.
 *The number of connected wires are returned.
 */
int
check_pin_direct_connect(IN t_pr_pin   *pin,
                         IN t_pr_marco *des
                        )
{
  int dcon=0;
  int ipin=0;
  int pinno=pin->nets->numpin;
  {return 0;}
  for (ipin=0;ipin<pinno;++ipin)
  {
    if (des==pin->nets->pins[ipin]->parent)
    {dcon++;}
  }
  return dcon;
}


/*
 *Check whether the pin belongs a certain type of block
 */
boolean
check_parent_type(IN t_pr_pin *src,
                  IN enum e_pr_type tcomp
                 )
{
  // DEBUG
 // t_pr_pin *cur = 0x8054ba0;

  enum e_pr_type gottype=src->parent->type;
  if (gottype==tcomp)
  {return TRUE;}
  return FALSE;
}

/*
 *Find out the direct attractness between one 
 *given pr_marco (exclude the IC block, VDD, GND) 
 *and virtual net.
 */
int
cal_mv_attract(IN t_pr_marco *src,
               IN t_vnet *des
              )
{
  int mvattract=0;
  int ipin=0;
  t_pr_pin* mpin=NULL;
  for(ipin=0;ipin<src->pinnum;++ipin)
  { 
    mpin=src->pins[ipin];
    if (mpin->nets==des)
    {mvattract++;}  
  }
  return mvattract;
}

/*
 *Find out the closeness between two virtual net.
 *As a matter of fact, the sub intend to calculate 
 *the indirect attractness.
 *Notice that the inter media marco should not 
 *be an ic block.
 */
int
cal_vv_close(IN t_vnet *src,
             IN t_vnet *des
            )
{
  int vvclose=0;
  int ipin=0;
  t_pr_pin* vpin=NULL;
  for (ipin=0;ipin<src->numpin;++ipin)
  {
    vpin=(*(src->pins+ipin));
    if ((!check_parent_type(vpin,ICBLOCK))&&(!check_parent_type(vpin,GND))&&(!check_parent_type(vpin,VDD)))
    {vvclose+=cal_mv_attract(vpin->parent,des);}
  }
  return vvclose;
}

/*
 *Find out the closeness (indirect) between
 *one given marco (must be IC block) and virtual net. 
 */
int
cal_mv_close(IN t_pr_marco* src,
             IN t_vnet* des
             )
{
  int mvclose=0;
  int ipin=0;
  t_pr_pin* mpin=NULL;
  for(ipin=0;ipin<src->pinnum;++ipin)
  {
    mpin=src->pins[ipin];
    mvclose+=cal_vv_close(mpin->nets,des);
  }
  return mvclose;
}

int
cal_vm_close(IN t_vnet* src,
             IN t_pr_marco* des
             )
{
  return cal_mv_close(des,src);
}

int cal_mm_close_in_vv(IN t_pr_marco* src,
                       IN t_pr_marco *des
                      )
{
  int close=0;
  t_vnet* srcnet=NULL;
  t_vnet* desnet=NULL;
  int ipin=0;
  int jpin=0;
  for (ipin=0;ipin<src->pinnum;++ipin)
  {
    for (jpin=0;jpin<des->pinnum;++jpin)
    {
      srcnet=(*(src->pins+ipin))->nets;
      desnet=(*(des->pins+jpin))->nets;
      if (srcnet!=desnet)
      {close+=cal_vv_close(srcnet,desnet);}
    }
  }
  return close;
}

/*
 *Calculate the directed connection criticality
 *between two given marcos.
 */
int 
cal_mm_close(IN t_pr_marco *src,
             IN t_pr_marco *des
            )
{
  int close=0;
  int pinno=src->pinnum;
  int ipin=0;
  t_pr_pin* pintmp=NULL;
  for(ipin=0;ipin<pinno;++ipin)
  {
     pintmp=*(src->pins+ipin);
     close+=check_pin_direct_connect(pintmp,des);
  }
 close+=cal_mm_close_in_vv(src,des);
 return close;
}

/*
 *This is a shell for find th closeness with
 *source marco and destination virtual net.
 *It is constrainted that source should be PLACED
 *and destination should be UNPLACED.
 */
int
find_mv_close(IN t_pr_marco* src,
              IN t_vnet* des
             )
{
  if ((PLACED==src->status)&&(UNPLACED==des->status))
  {return cal_mv_close(src,des);}
  else
  {return 0;}
}

/*
 *This is a shell for find th closeness with source
 *source virtual net and destination marco.
 *It is constrainted that source should be PLACED
 *and destination should be UNPLACED.
 */
int
find_vm_close(IN t_vnet* src,
              IN t_pr_marco* des
             )
{
  if ((PLACED==src->status)&&(UNPLACED==des->status))
  {return cal_mv_close(des,src);}
  else
  {return 0;}
}

/*
 *A shell contain getting the closeness between 
 *the source ic block and destination ic block.
 *It is constrainted that source should be PLACED
 *and destination should be UNPLACED. 
 */
int 
find_mm_close(IN t_pr_marco *src,
              IN t_pr_marco *des
              )
{

  if ((PLACED==src->status)&&(UNPLACED==des->status))
  {return cal_mm_close(src,des);}
  else
  {return 0;}
}

/*
 *Estimate the basic width for a virtual net
 *which is a special net.
 */
int
find_vnet_basic_width(IN t_vnet* vnet,
                      IN int wcapacity)
{
  int wbasic=0;
  int npin=vnet->numpin;
  if (npin>2*(wcapacity-1))
  {
    wbasic=1;
    npin-=(2*wcapacity-3);
    while(1)
    {
      if (npin<(2*wcapacity-3))
      {break;}
      else
      {
        wbasic++;
        npin-=(2*wcapacity-4);
      }
    }
  }
  else
  {wbasic=1;}
  return wbasic;
}

/*
 *Estimate the basic width for a virtual net
 *which is attached to a IC block.
 */
int
find_mnet_basic_width(IN t_vnet* vnet,
                      IN int wcapacity)
{
  int wbasic=0;
  int npin=vnet->numpin;
  int initw=0;
  int ipin=0;
  /*
   *When there are more than one pin of block,
   *we consider that the initial width should 
   *be enlarged.
   */
  for (ipin=0;ipin<npin;++ipin)
  {
    if (ICBLOCK==(*(vnet->pins+ipin))->parent->type)
    {initw+=(wcapacity-3);}
  }
  /*
   *If there is only one pin of a block, the initial
   *width should be (wcapacity-1) instead of (wcapacity-3).
   *Or we will compensate the initial width by adding 2
   *because the first and last width should be (wcapacity-2).
   */
  if ((wcapacity-3)==initw)
  {initw=wcapacity-1;}
  else
  {initw+=2;}
  if (npin>initw)
  {
    wbasic=1;
    npin-=(wcapacity-2);
    while(1)
    {
      if (npin<(wcapacity-2))
      {break;}
      else
      {
        wbasic++;
        npin-=(wcapacity-2);
      }
    }
  }
  else
  {wbasic=1;}
//  vnet->locnum=wbasic;
//  vnet->locations=(t_location*)malloc(vnet->locnum*sizeof(t_location));
  return wbasic;
}



