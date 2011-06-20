#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <bb_place.h>
#include <place_crit.h>
#include <place_common.h>
#include <place_util.h>

/***************Subroutines****************/
int
alloc_vnet_place_bb(IN int name,
                    INOUT int* curloc,
                    IN int blank,
                    IN int column,
                    IN int nvnet,
                    IN t_vnet* vnets,
					IN int wcapacity
                   );

int
alloc_blk_place_bb(IN int name,
                   INOUT int* curloc,
                   IN int blank,
                   IN int column,
                   IN int nblk,
                   IN t_pr_marco* blks
                  );

int
alloc_bb_place(IN int nblk,
               IN t_pr_marco* blks,
               IN int nvnet,
               IN t_vnet* vnets,
               IN t_bb_array* bb_array
               );

t_pr_marco*
find_blk_with_name(IN int name,
                   IN int column,
                   IN int nblk,
                   IN t_pr_marco* blks
                  );

t_vnet*
find_vnet_with_name(IN int name,
                    IN int column,
                    IN int nvnet,
                    IN t_vnet* vnets
                    );

int
detail_place_blk(INOUT t_pr_marco* blk,
                 IN int column,
                 IN int left,
                 IN int nmarco,
                 IN t_pr_marco* pr_marco,
                 IN int nvnet,
                 IN t_vnet* vnets,
                 IN t_bb_array* bb_array
                );

int
detail_place_vnet(INOUT t_vnet* vnet,
                  IN int column,
                  IN int left,
                  IN int nmarco,
                  IN t_pr_marco* pr_marco,
                  IN int nvnet,
                  IN t_vnet* vnets,
                  IN t_bb_array* bb_array
                 );

int
detail_place_big_vnet(IN int vnetoff,
                      IN t_vnet* vnet,
                      IN int wcapacity
                     );

/******************************************/


int
alloc_vnet_place_bb(IN int name,
                    INOUT int* curloc,
                    IN int blank,
                    IN int column,
                    IN int nvnet,
                    IN t_vnet* vnets,
					IN int wcapacity
                   )
{
  int inet=0;
  int baswidth;
  
  for (inet=0;inet<nvnet;++inet)
  {
    if ((column==(vnets+inet)->pcolumn)
	   &&(name==(vnets+inet)->name)
	   &&(SPECIAL==(vnets+inet)->type))
    {
      baswidth=find_vnet_basic_width(vnets+inet,wcapacity);

      vnets[inet].locnum=baswidth;
      vnets[inet].locations=(t_location*)malloc(baswidth*sizeof(t_location));
      if (NULL==vnets[inet].locations)
      {
        printf("Bread Board Placement: Fail to malloc locatons(Width:%d) for vnet[%d].\n",baswidth,inet);
        abort();
	    exit(1);
      }
      vnets[inet].locations->x=(*curloc);
	  vnets[inet].locations->y=0;

      (*curloc)=(*curloc)+blank+(vnets+inet)->pwidth;  
       return 1;
    }
  }
  return 0;
}

int
alloc_blk_place_bb(IN int name,
                   INOUT int* curloc,
                   IN int blank,
                   IN int column,
                   IN int nblk,
                   IN t_pr_marco* blks
                  )
{
  int iblk=0;
  for (iblk=0;iblk<nblk;++iblk)
  {
    if ((column==(blks+iblk)->pcolumn)&&(name==(blks+iblk)->name)&&(ICBLOCK==(blks+iblk)->type))
    {
      (blks+iblk)->location.x=(*curloc);
      (*curloc)=(*curloc)+blank+(blks+iblk)->pwidth;  
	  printf("Block[%d] location: (%d,%d) column: %d \n",iblk,blks[iblk].location.x,blks[iblk].location.y,column);
       return 1;
    }
  }
  return 0;
}

int
alloc_bb_place(IN int nblk,
               IN t_pr_marco* blks,
               IN int nvnet,
               IN t_vnet* vnets,
               IN t_bb_array* bb_array
               )
{
  int blank=0;
  int icol=0;
  int left=0;
  int right=0;
  int curloc=0;
  int ioff=0;
  int wcapacity=0;

  while(icol<bb_array->no_column)
  {
    if (0==bb_array->columns[icol].usedwidth)
	{icol++;continue;}
    left=bb_array->columns[icol].left;
    right=bb_array->columns[icol].right;
    blank=(bb_array->width-bb_array->columns[icol].usedwidth)/(right-left+3);
    curloc=blank;
	wcapacity=bb_array->columns[icol].width_capacity;
    for (ioff=left;ioff<(right+1);++ioff)
    {
      alloc_vnet_place_bb(ioff,&curloc,blank,icol,nvnet,vnets,wcapacity);
      alloc_blk_place_bb(ioff,&curloc,blank,icol,nblk,blks);
    }
    icol++;
  }
  return 1;
}


  
t_pr_marco*
find_blk_with_name(IN int name,
                   IN int column,
                   IN int nblk,
                   IN t_pr_marco* blks
                  )
{
  int iblk=0;
  for (iblk=0;iblk<nblk;++iblk)
  {
    if ((column==(blks+iblk)->pcolumn)&&(name==(blks+iblk)->name)&&(ICBLOCK==(blks+iblk)->type))
    {return (blks+iblk);}
  }
  return NULL;
}

t_vnet*
find_vnet_with_name(IN int name,
                    IN int column,
                    IN int nvnet,
                    IN t_vnet* vnets
                    )
{
  int inet=0;
  for (inet=0;inet<nvnet;++inet)
  {
    if ((column==(vnets+inet)->pcolumn)&&(name==(vnets+inet)->name)&&(SPECIAL==(vnets+inet)->type))
    {return (vnets+inet);}
  }
  return NULL;
}

int
detail_place_blk(INOUT t_pr_marco* blk,
                 IN int column,
                 IN int left,
                 IN int nmarco,
                 IN t_pr_marco* pr_marco,
                 IN int nvnet,
                 IN t_vnet* vnets,
                 IN t_bb_array* bb_array
                )
{
  int cur=blk->name;
  int ioff=0;
  int blkoff=0;
  t_pr_marco* lftblk=NULL;
  t_vnet* lftvnet=NULL;

  int ipin=0;
  t_vnet* blkvnet=NULL;
  int wcapacity=0;

  for (ioff=left;ioff<cur;++ioff)
  {
    lftblk=find_blk_with_name(ioff,column,nmarco,pr_marco); 
    lftvnet=find_vnet_with_name(ioff,column,nvnet,vnets);
    if ((NULL==lftblk)&&(NULL==lftvnet))
    {
      printf("Error: Fail to find the blk or vnet with given name(name:%d)\n",ioff);
      return FALSE;
    }
    else if ((NULL!=lftblk)&&(NULL!=lftvnet)) 
    {
      printf("Error: Exist both blk and vnet with the shared name(name:%d)\n",ioff);
      return FALSE;
    }
    else if (NULL!=lftblk)
    {blkoff+=cal_mm_close_in_vv(lftblk,blk);}
    else if (NULL!=lftvnet)
    {blkoff+=cal_vm_close(lftvnet,blk);}    
  }
  /*Find whether exist qualified big vnets of the blk*/
  wcapacity=bb_array->columns[column].width_capacity;
  for (ipin=0;ipin<blk->pinnum;++ipin)
  {
    if ((*(blk->pins+ipin))->offset<(int)(blk->device->width/2+1))
    {
      blkvnet=(*(blk->pins+ipin))->nets;
      /*Leave the normal virtual net to routing.*/
      blkoff+=find_mnet_basic_width(blkvnet,wcapacity)-1;
    }
  }
  blk->location.x+=blkoff;
  return 1;
}

/*
 *Notice that detail placement of the virtual net
 *is only for special virtual net.
 *Normal virtual net is considered in routing.
 */
int
detail_place_vnet(INOUT t_vnet* vnet,
                  IN int column,
                  IN int left,
                  IN int nmarco,
                  IN t_pr_marco* pr_marco,
                  IN int nvnet,
                  IN t_vnet* vnets,
                  IN t_bb_array* bb_array
                 )
{
  int cur=vnet->name;
  int ioff=0;
  int vnetoff=0;
  t_pr_marco* lftblk=NULL;
  t_vnet* lftvnet=NULL;

 // int ipin=0;
  int baswidth=0;
  int wcapacity=0;

  for (ioff=left;ioff<cur;++ioff)
  {
    vnetoff=0;
    lftblk=find_blk_with_name(ioff,column,nmarco,pr_marco); 
    lftvnet=find_vnet_with_name(ioff,column,nvnet,vnets);
    if ((NULL==lftblk)&&(NULL==lftvnet))
    {
      printf("Error: Fail to find the blk or vnet with given name(name:%d)\n",ioff);
      return FALSE;
    }
    else if ((NULL!=lftblk)&&(NULL!=lftvnet)) 
    {
      printf("Error: Exist both blk and vnet with the shared name(name:%d)\n",ioff);
      return FALSE;
    }
    else if (NULL!=lftblk)
    {vnetoff+=cal_mv_close(lftblk,vnet);}
    else if (NULL!=lftvnet)
    {vnetoff+=cal_vv_close(lftvnet,vnet);}    
  }
  /*Determine the virtual nets are too big to occupy several x*/
  wcapacity=bb_array->columns[column].width_capacity;
  baswidth=find_vnet_basic_width(vnet,wcapacity);

  if (baswidth>1)
  {
    detail_place_big_vnet(vnetoff,vnet,wcapacity);
  }
  else
  {
    vnet->locations->x+=vnetoff;
  }
  return 1;
}

int
detail_place_big_vnet(IN int vnetoff,
                      IN t_vnet* vnet,
                      IN int wcapacity
                     )
{
  int ioff=0;
  int total=vnet->numpin;
  int curoff=vnetoff;
  int remain=total-vnetoff;
  int tmp=0;
  while(1)
  { 
    if (0==ioff) {tmp=2;}
    else {tmp=3;} 
    if (vnetoff>(2*wcapacity-tmp)) 
    {
      vnet->locations[ioff].x+=(2*wcapacity-tmp);
      curoff-=(2*wcapacity-tmp);
    }
    else
    {
      vnet->locations[ioff].x+=curoff;
      break;
    }
    ioff++;
    if (!(ioff<vnet->locnum)) 
    {
      printf ("Error: Fail to place the big net\n");
      exit(1);
    }
  }
  
  while(1)
  {
    if (0==ioff) {tmp=2;}
    else {tmp=3;}

    ioff++;
    if (!(ioff<vnet->locnum)) 
    {
      printf ("Error: Fail to place the big net\n");
      exit(1);
    }
    
    if (curoff>0)
    {
      vnet->locations[ioff].x=vnet->locations[ioff-1].x+2*wcapacity-tmp-curoff;
      remain-=2*wcapacity-tmp-curoff;
      curoff=0;
    }
    else
    {
      if (remain>(2*wcapacity-tmp)) 
      {
        vnet->locations[ioff].x=vnet->locations[ioff-1].x+(2*wcapacity-tmp);
        remain-=(2*wcapacity-tmp);
      }
      else
      {
        vnet->locations[ioff].x=vnet->locations[ioff-1].x+remain;
        break;
      }
    }
  }
  return 0;
}

boolean
detail_place(IN int nvnet,
             IN t_vnet* vnets,
             IN int nmarco,
             INOUT t_pr_marco* pr_marco,
             INOUT t_bb_array* bb_array
            )
{
  int icol=0;
  int left=0;
  int right=0;
  int ioff=0;
  t_pr_marco* blk=NULL;
  t_vnet* vnet=NULL;
  
  printf("Start detail placement on bread board...\n");
  
  alloc_bb_place(nmarco,pr_marco,nvnet,vnets,bb_array);


  while (icol<bb_array->no_column)
  {
    printf("Detail Placement on column(%d)...\n",icol);

    if (0==bb_array->columns[icol].usedwidth)
	{
	  icol++;
	  continue;
	}


    left=bb_array->columns[icol].left;
    right=bb_array->columns[icol].right;
    blk=find_blk_with_name(ioff,icol,nmarco,pr_marco); 
    vnet=find_vnet_with_name(ioff,icol,nvnet,vnets);
    for (ioff=left;ioff<(right+1);++ioff)
    {
      if ((NULL==blk)&&(NULL==vnet))
      {
        printf("Error: Fail to find the blk or vnet with given name(name:%d),column(%d)\n",ioff,icol);
        return FALSE;
      }
      else if ((NULL!=blk)&&(NULL!=vnet)) 
      {
        printf("Error: Exist both blk and vnet with the shared name(name:%d)\n",ioff);
        return FALSE;
      }
      else if (NULL!=blk)
      {detail_place_blk(blk,icol,left,nmarco,pr_marco,nvnet,vnets,bb_array);}
      else if (NULL!=vnet)
      {detail_place_vnet(vnet,icol,left,nmarco,pr_marco,nvnet,vnets,bb_array);}
    }
    icol++;
  }
  printf("Detail placement Over...\n");
  return TRUE;
}
