#include <stdio.h>
#include <stdlib.h>

#include <rabbit_types.h>
#include <util.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <setup_rabbit.h>


void
alloc_vnet_place_bb(IN int poff,
                    INOUT int* curloc,
                    IN int blank,
                    IN int column,
                    IN int nvnet,
                    IN t_vnet* vnets
                   )
{
  int inet=0;
  for (inet=0;inet<nvnet;++inet)
  {
    if (column==(vnets+inet)->pcolumn)&&(poff==(vnets+inet)->name)
    {
      (vnets+inet)->loc_x=(*curloc);
      (*curloc)=(*curloc)+blank+(vnets+inet)->pwidth;  
       return 1;
    }
  }
  return 0;
}

void
alloc_blk_place_bb(IN int poff
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
    if (column==(blks+iblk)->pcolumn)&&(poff==(blks+iblk)->name)
    {
      (blks+iblk)->loc_x=(*curloc);
      (*curloc)=(*curloc)+blank+(blks+iblk)->pwidth;  
       return 1;
    }
  }
  return 0;
}

void
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
  while(icol<bb_array->no_column)
  {
    left=bb_array->(columns+icol)->left;
    right=bb_array->(columns+icol)->right;
    blank=(bb_array.width-bb_array->(columns+icol)->usedwidth)/(right-left+1);
    curloc=blank;
    for (ioff=left;ioff<(right+1);++ioff)
    {
      alloc_vnet_place_bb(ioff,&curloc,blank,icol,nvnet,vnets);
      alloc_blk_place_bb(ioff,&curloc,blank,icol,nblk,blks);
    }
    icol++;
  }
  return 1;
}
  
