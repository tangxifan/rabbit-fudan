#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>

/*****************Subroutines***********************/

t_vnet* 
find_start_vnet(IN int nvnet,
                IN t_vnet* vnets,
                IN int wcapacity
                );

float
find_starterblk_cost(IN t_pr_marco* blk);

t_pr_marco*
find_starter_block(IN int nblk,
                   IN t_pr_marco* blks
                  );

boolean
check_pwidth_status(IN t_pr_marco* marco,
                    IN t_vnet* vnet
                   );

int
find_vnet_pwidth(IN t_vnet* vnet,
                 IN int wcapacity);

int
find_blk_pwidth(IN int nblk,
                IN t_pr_marco* blks,
                IN t_pr_marco* blk,
                IN int wcapacity
               );



t_pr_marco*
find_match_blk(IN int nblk,
                    IN t_pr_marco* blks,
                    IN int nvnet,
                    IN t_vnet* vnets
                   );

t_vnet*
find_match_vnet(IN int nblk,
                IN t_pr_marco* blks,
                IN int nvnet,
                IN t_vnet* vnets
                );

int
determine_net_place_location(IN t_vnet* net,
                             INOUT t_place_info* place_info
                            );

int
determine_blk_place_location(IN t_pr_marco* blk,
                             INOUT t_place_info* place_info
                            );

/***************************************************/

/*
 *Spot the start vnet
 */
t_vnet* 
find_start_vnet(IN int nvnet,
                IN t_vnet* vnets,
                IN int wcapacity
                )
{
  t_vnet* netchn=NULL;
  int inet=0;
  float maxcost=0.0;
  float tmpcost=0.0;
  int pwidth=0;
  t_vnet* bnets=NULL;
 
  
   if (1==DEBUG)
   {printf("Finding vnet....\n");}
  
  for (inet=0;inet<nvnet;++inet)
  {
    if (1==DEBUG)
	{printf("Currently checking vnet(No.%d)...\n",inet);}
    bnets=vnets+inet;
    if ((UNSTART==bnets->sstart)&&(UNPLACED==bnets->status))
    {
      if (SPECIAL==bnets->type)
      {
        pwidth=find_vnet_basic_width(bnets,wcapacity);
        tmpcost=(float)(bnets->numpin/pwidth);
        if (tmpcost>maxcost)
        {
        //if (NULL!=netchn)
        //{netchn->pcost=0;}
          maxcost=tmpcost;
          netchn=bnets;
          bnets->pcost=maxcost;
        }
      }
    }
  }
  return netchn;
}

/*
 *Try to figure out the cost of input block
 */
float
find_starterblk_cost(IN t_pr_marco* blk)
{
  float cost=0.0;
  
  int conn=0;
  int ipin=0;
  int jpin=0;
  int pinnoi=0;
  int pinnoj=0;
  t_pr_pin* pintmpi=NULL;
  t_pr_pin* pintmpj=NULL;
  
  pinnoi=blk->pinnum;
  for(ipin=0;ipin<pinnoi;++ipin)
  {
    pintmpi=(*(blk->pins+ipin));
    pinnoj=pintmpi->nets->numpin;
    for(jpin=0;jpin<pinnoj;++jpin)
    {
      pintmpj=pintmpi->nets->pins[jpin];
      if (pintmpj->parent!=blk)
      {conn++;}
    }
  }
  cost=conn/pinnoi;
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
    if ((UNPLACED==blktmp->status)&&(UNSTART==blktmp->sstart))
    {
      cost=find_starterblk_cost(blktmp);
      if(cost>max_cost)
      {
        max_cost=cost;
        blkchn=blktmp;
        blkchn->pcost=cost;
      }
    }
  }
  return blkchn; 
}

/*
 *Check whether the nets of the given marco have 
 *been POINTED during calculating virtual net 
 *place width.
 */
boolean
check_pwidth_status(IN t_pr_marco* marco,
                    IN t_vnet* vnet
                   )
{
  int ipin=0;
  for (ipin=0;ipin<marco->pinnum;++ipin)
  {
    if (POINTED==(*(marco->pins+ipin))->nets->spwidth)
    {return TRUE;}
    else if ((*(marco->pins+ipin))->nets!=vnet)
    {
      (*(marco->pins+ipin))->nets->spwidth=POINTED;
      return FALSE;
    }
  }
  return TRUE;
}

/*
 *Estimate the place width for virtual net.
 */
int
find_vnet_pwidth(IN t_vnet* vnet,
                 IN int wcapacity)
{
  int pwidth=find_vnet_basic_width(vnet,wcapacity);
  int ipin=0;
  int npin=vnet->numpin;
  int minl=vnet->pins[0]->parent->device->min_length;
  t_pr_pin* vpin=NULL;
  
  if(1==DEBUG)
  {printf("Virtual net basic place width(%d)....\n",pwidth);}
  for (ipin=0;ipin<npin;++ipin)
  {
    if(1==DEBUG)
	{printf("Checking pin[%d](Net No.%d) for place width...\n",ipin,(int)vnet);}
    vpin=vnet->pins[ipin];
    if ((!check_parent_type(vpin,ICBLOCK))&&(!check_parent_type(vpin,GND))&&(!check_parent_type(vpin,VDD)))
    {
      pwidth++;
      /*Currently the optimization is neglected*/
    //if (check_pwidth_status(vpin->parent,vnet))
    //{pwidth--;}
      if (minl>vpin->parent->device->min_length)
      {minl=vpin->parent->device->min_length;}
    }
  }
  //for (ipin=0;ipin<npin;++ipin)
  //{set_unpoint_marco((*(vnet->pins+ipin))->parent);}
//pwidth+=minl;
  vnet->pwidth=pwidth;
  if (1==DEBUG)
  {printf("Virtual Net(%d) place width is %d.\n",(int)vnet,pwidth);}
  return pwidth;  
}


/*
 *Estimate the place width for IC block.
 */
int
find_blk_pwidth(IN int nblk,
                IN t_pr_marco* blks,
                IN t_pr_marco* blk,
                IN int wcapacity
               )
{
  int pwidth=blk->device->width;
  int ipin=0;
  int iblk=0;
  t_vnet* mnet=NULL;
  for (ipin=0;ipin<blk->pinnum;++ipin)
  {
    mnet=(*(blk->pins+ipin))->nets;
    pwidth+=find_mnet_basic_width(mnet,wcapacity)-1;
  }
  for (iblk=0;iblk<nblk;++iblk)
  {
    if (blk!=blks+iblk)
    {pwidth+=cal_mm_close_in_vv(blk,blks+iblk);}
  }
  return pwidth;
}

/*
 *Find a virtual net or IC block as starter
 */
int
find_starter(IN int nblk,
             IN t_pr_marco* blks,
             IN int nvnet,
             IN t_vnet* vnets,
             IN t_place_info *place_info,
             IN t_bb_array bb_array
             )
{
  int pwidth=0;
  int bb_pwidth=place_info->bb_pwidth;
  int wcapacity=bb_array.columns[place_info->column].width_capacity; 

  t_pr_marco* blkcd=NULL;
  t_vnet* netcd=NULL;
  
  float vnet_pcost=0.0;
  float blk_pcost=0.0;

  while(1)
  {
    if (nblk!=0)
    {
	  blkcd=find_starter_block(nblk,blks);
      blk_pcost=blkcd->pcost;	  
    }
    if (nvnet!=0)
    {
	  netcd=find_start_vnet(nvnet,vnets,wcapacity);
      vnet_pcost=netcd->pcost;
	}
	if (blk_pcost<vnet_pcost)
    {
	  if(1==DEBUG)
	  {printf("The Start Net is vnet[%d].\n",netcd-vnets);}
      pwidth=find_vnet_pwidth(netcd,wcapacity);

	  if(1==DEBUG)
	  {printf("The Start Net place width is %d.\n",pwidth);}

      if (bb_pwidth<pwidth)
      {netcd->sstart=STARTED;}
      else
      {
        netcd->sstart=STARTED;
        netcd->pwidth=pwidth;
        netcd->status=PLACED;
        netcd->pcolumn=place_info->column;
        netcd->name=0;
        place_info->cur_width+=pwidth;
        place_info->left->flag=FALSE;
        place_info->left->mnext=NULL;
        place_info->left->vnext=netcd;
        place_info->right->flag=FALSE;
        place_info->right->mnext=NULL;
        place_info->right->vnext=netcd;
        break;
      }
    }
    else
    {
      pwidth=find_blk_pwidth(nblk,blks,blkcd,wcapacity);
      if (bb_pwidth<pwidth)
      {blkcd->sstart=STARTED;}
      else
      {
        blkcd->sstart=STARTED;
        blkcd->pwidth=pwidth;
        blkcd->status=PLACED;
        blkcd->pcolumn=place_info->column;
        blkcd->name=0;
        place_info->cur_width+=pwidth;
        place_info->left->flag=TRUE;
        place_info->left->mnext=blkcd;
        place_info->left->vnext=NULL;
        place_info->right->flag=TRUE;
        place_info->right->mnext=blkcd;
        place_info->right->vnext=NULL;
        break;
      }
    }
    check_start_error(nblk,blks,nvnet,vnets);
  }  
  return 1;
}

t_pr_marco*
find_match_blk(IN int nblk,
                    IN t_pr_marco* blks,
                    IN int nvnet,
                    IN t_vnet* vnets
                   )
{
  int iblk=0;
  int jblk=0;
  int inet=0;
  float maxcost=0.0;
  t_pr_marco* blkmatch=NULL;

  /*Find the matched block from PLACED ic blocks*/
  for(iblk=0;iblk<nblk;++iblk)
  {
    if (PLACED==(blks+iblk)->status)
    {
      for(jblk=0;jblk<nblk;++jblk)
      {
        if(UNPLACED==(blks+jblk)->status)
        {(blks+jblk)->pcost+=(float)find_mm_close((blks+iblk),(blks+jblk));}
      }
    }
  }
  /*Find the matched block from PLACED virtual nets*/
  for(inet=0;inet<nvnet;++inet)
  {
    if ((PLACED==(vnets+inet)->status)&&(SPECIAL==(vnets+inet)->type))
    {
      for(jblk=0;jblk<nblk;++jblk)
      {
        if(UNPLACED==(blks+jblk)->status)
        {(blks+jblk)->pcost+=(float)find_vm_close((vnets+inet),(blks+jblk));}
      }
    }
  }
  /*Spot the matched IC block*/
  for (iblk=0;iblk<nblk;++iblk)
  {
    if ((blks[iblk].pcost>maxcost)&&(UNPLACED==blks[iblk].status))
    {
      maxcost=(blks+iblk)->pcost;
      blkmatch=(blks+iblk);
    }
  }
  return blkmatch;  
}

t_vnet*
find_match_vnet(IN int nblk,
                IN t_pr_marco* blks,
                IN int nvnet,
                IN t_vnet* vnets
                )
{
  int inet=0;
  int jnet=0;
  int iblk=0;
  float maxcost=0.0;
  t_vnet* netmatch=NULL;

  /*Find the matched virtual net from PLACED ic blocks*/
  for(iblk=0;iblk<nblk;++iblk)
  {
    if (PLACED==(blks+iblk)->status)
    {
      for(jnet=0;jnet<nvnet;++jnet)
      {
        if ((UNPLACED==(vnets+jnet)->status)&&(SPECIAL==(vnets+jnet)->type))
        {(vnets+jnet)->pcost+=(float)find_mv_close((blks+iblk),(vnets+jnet));}
      }
    }
  }
  /*Find the matched virtual net from PLACED virtual nets*/
  for(inet=0;inet<nvnet;++inet)
  {
    if ((PLACED==(vnets+inet)->status)&&(SPECIAL==(vnets+inet)->type))
    {
      for(jnet=0;jnet<nvnet;++jnet)
      {
        if ((UNPLACED==(vnets+jnet)->status)&&(SPECIAL==(vnets+jnet)->type))
        {(vnets+jnet)->pcost+=(float)cal_vv_close((vnets+inet),(vnets+jnet));}
      }
    }
  }
  /*Spot the matched virtual net*/
  for (inet=0;inet<nvnet;++inet)
  {
    if ((vnets[inet].pcost>maxcost)&&(SPECIAL==vnets[inet].type)&&(UNPLACED==vnets[inet].status))
    {
      maxcost=(vnets+inet)->pcost;
      netmatch=(vnets+inet);
    }
  }
  return netmatch;  
}

int
determine_net_place_location(IN t_vnet* net,
                             INOUT t_place_info* place_info
                            )
{
  int leftcl=0;
  int rightcl=0;
  t_vnet* netlft=NULL;
  t_vnet* netrgt=NULL;
  t_pr_marco* blklft=NULL;
  t_pr_marco* blkrgt=NULL;
  
  int leftn=0;
  int rightn=0;

  if (TRUE==place_info->left->flag)
  {
    blklft=place_info->left->mnext;
    leftn=blklft->name;
    leftcl=find_mv_close(blklft,net);
  }
  else
  {
    netlft=place_info->left->vnext;
    leftn=netlft->name;
    leftcl=cal_vv_close(netlft,net);
  }

  if (TRUE==place_info->right->flag)
  {
    blkrgt=place_info->right->mnext;
    rightn=blkrgt->name;
    rightcl=find_mv_close(blkrgt,net);
  }
  else
  {
    netrgt=place_info->right->vnext;
    rightn=netrgt->name;
    rightcl=cal_vv_close(netrgt,net);
  }
  
  if (rightcl<leftcl)
  {
    net->name=leftn-1;
    place_info->left->flag=FALSE;
    place_info->left->mnext=NULL;
    place_info->left->vnext=net;
  }
  else
  {
    net->name=rightn+1;
    place_info->right->flag=FALSE;
    place_info->right->mnext=NULL;
    place_info->right->vnext=net;
  }
  return 1;
}

int
determine_blk_place_location(IN t_pr_marco* blk,
                             INOUT t_place_info* place_info
                            )
{
  int leftcl=0;
  int rightcl=0;
  t_vnet* netlft=NULL;
  t_vnet* netrgt=NULL;
  t_pr_marco* blklft=NULL;
  t_pr_marco* blkrgt=NULL;
  
  int leftn=0;
  int rightn=0;

  if (TRUE==place_info->left->flag)
  {
    blklft=place_info->left->mnext;
    leftn=blklft->name;
    leftcl=find_mm_close(blklft,blk);
  }
  else
  {
    netlft=place_info->left->vnext;
    leftn=netlft->name;
    leftcl=find_vm_close(netlft,blk);
  }

  if (TRUE==place_info->right->flag)
  {
    blkrgt=place_info->right->mnext;
    rightn=blkrgt->name;
    rightcl=find_mm_close(blkrgt,blk);
  }
  else
  {
    netrgt=place_info->right->vnext;
    rightn=netrgt->name;
    rightcl=find_vm_close(netrgt,blk);
  }
  
  if (rightcl<leftcl)
  {
    blk->name=leftn-1;
    place_info->left->flag=TRUE;
    place_info->left->vnext=NULL;
    place_info->left->mnext=blk;
  }
  else
  {
    blk->name=rightn+1;
    place_info->right->flag=TRUE;
    place_info->right->vnext=NULL;
    place_info->right->mnext=blk;
  }
  return 1;
}

/*
 *Find the matched virtual net or IC block
 *with the greatest attractness.
 */
boolean
find_match(IN int nblk,
           IN t_pr_marco* blks,
           IN int nvnet,
           IN t_vnet* vnets,
           IN t_bb_array bb_array,
           INOUT t_place_info* place_info
          )
{
  int wcapacity=bb_array.columns[place_info->column].width_capacity; 
  int pwidth;
  int iblk=0;
  int inet=0;
  int blkpw=0;
  int netpw=0;

  t_vnet* netmatch=NULL;
  t_pr_marco* blkmatch=NULL;
  
  float blk_pcost=0.0;
  float net_pcost=0.0;

  if (nvnet!=0)
  {
    netmatch=find_match_vnet(nblk,blks,nvnet,vnets);
    if (netmatch!=NULL)
	{net_pcost=netmatch->pcost;}
  }
  
  if (nblk!=0)
  {
    blkmatch=find_match_blk(nblk,blks,nvnet,vnets);
    if (blkmatch!=NULL)
	{blk_pcost=blkmatch->pcost;}	
  }
  
  if ((nvnet!=0)&&(netmatch!=NULL))
  {netpw=find_vnet_pwidth(netmatch,wcapacity);}
  
  if ((nblk!=0)&&(blkmatch!=NULL))
  {blkpw=find_blk_pwidth(nblk,blks,blkmatch,wcapacity);}


  if (((place_info->cur_width+netpw)>place_info->bb_pwidth)&&((place_info->cur_width+blkpw)>place_info->bb_pwidth))
  {return FALSE;}

  if ((NULL==blkmatch)&&(NULL==netmatch))
  {return FALSE;}
  if ((net_pcost*netpw)>(blk_pcost*blkpw))
  {
    if (!((place_info->cur_width+netpw)>place_info->bb_pwidth))
    {
      printf("Find net[%d]!\n",netmatch-vnets); 
      determine_net_place_location(netmatch,place_info);
      netmatch->pcolumn=place_info->column;
      netmatch->status=PLACED;
    }
    else if (nblk!=0)
    {
      printf("Find block[%d]!\n",blkmatch-blks); 
      determine_blk_place_location(blkmatch,place_info);
      blkmatch->pcolumn=place_info->column;
      blkmatch->status=PLACED;
    }
  }  
  else
  {
    if (!((place_info->cur_width+netpw)>place_info->bb_pwidth))
    {
      printf("Find block[%d]!\n",blkmatch-blks); 
      determine_blk_place_location(blkmatch,place_info);
      blkmatch->pcolumn=place_info->column;
      blkmatch->status=PLACED;
    }
    else
    {
      printf("Find net[%d]!\n",netmatch-vnets); 
      determine_net_place_location(netmatch,place_info);
      netmatch->pcolumn=place_info->column;
      netmatch->status=PLACED;
    }
  }
  return TRUE;
}
