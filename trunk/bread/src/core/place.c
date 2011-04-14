#include <stdio.h>
#include <stdlib.h>

#include <rabbit_types.h>
#include <util.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <setup_rabbit.h>


/*
 *Complete the placement of all IC blocks on the 
 *breadboard.Return TRUE if the placement succeed.
 *There are several steps during placement.
 *
 */
boolean
try_place(IN int nvnet,
          IN t_vnet* vnet,
          IN int nmarco,
          INOUT t_pr_marco* pr_marco,
          INOUT t_bb_array bb_array
         )
{
  int nblk=0;
  int iblk=0;
  t_pr_marco* icblks=NULL;
  t_pr_marco* starter_blk=NULL;
  t_pr_marco* blkmatch=NULL;

  int inet=0;
  
  int const bb_pwidth=(1-FLEX_RATIO)*bb_array.width;  
  int icolumn=0;
  int ncolumn=bb_array.no_column;
  int wcapacity=0;
  int blkwd=0;
  int blkwdtmp=0;

  create_icblk_array(&nblk,icblks,nmarco,pr_marco);
  while(icolumn<ncolumn)
  {
    blkwd=0;
    wcapacity=bb_array.columns[icolumn].width_capacity;
    while(1)
    {
      starter_blk=find_starter_block(nblk,icblks);
      blkwd=cal_block_width(starter_blk,nblk,icblks,nvnet,vnet,wcapacity);
      starter_blk->start=STARTED;
      if ((blkwd<bb_pwidth)&&(blkwd==bb_pwidth))
      {
        starter_blk->status=PLACED;
        starter_blk->placed_column=icolumn;
        set_unstart_marco(nblk,icblks);
        starter_blk->place_width=blkwd;
        break;
      }
      check_start_error(nblk,icblks);
    }
    while(1)
    {
      find_match_block(blkmatch,nblk,icblks);
      blktmp=blkwd;
      blkmatch->place_width=cal_block_width(blkmatch,nblk,icblks,nvnet,vnet,wcapacity);
      blkwd+=blkmatch;
      if ((blkwd<bb_pwidth)&&(blkwd==bb_pwidth))
      {
        blkmatch->placed_column=icolumn;
        blkmatch->status=PLACED;
      }
      else
      {
        blkwd=blktmp;
        blkmatch->place_width=0;
        climbing_place(icolumn,bb_pwidth,&blkwd,nblk,icblks,nvnet,vnet,wcapacity);
        place_on_bb(nblk,icblks,icolumn,blkwd,bb_array)
        break;
      }
    }
    if (TRUE==check_place_over(nblk,icblks))
    {break;}
    icolumn++;
  }
  check_all_placed(nblk,icblks);
  update_marco_free_icblks(nmarco,pr_marco,nblk,icblks);
  return TRUE;
}

/*
 *When the placement, the finishing work include
 *I. Update the pr_marcos from these icblks because 
 *   icblks are part of the copy of pr_marcos.
 *II. Free the icblks.
 */
void
update_marco_free_icblks(IN int nmarco,
                         INOUT t_pr_marco* pr_marco,
                         IN int nblk,
                         IN t_pr_marco* icblks
                         )
{
  update_pr_marco(nmarco,pr_marco,nblk,icblks);
  free_icblks(nblk,icblks);
}

void 
update_pr_marco(IN int nmarco,
                INOUT t_pr_marco* pr_marco,
                IN int nblk,
                IN t_pr_marco* icblks
                )
{
  int imarco=0;
  int iblk=0;
  for (imarco=0;imarco<nmarco;++imarco)
  {
    if (ICBLOCK==(pr_marco+imarco)->type)
    {
      for(iblk=0;iblk<nblk;++iblk)
      {
        if ((pr_marco+imarco)->name==(icblks+iblk)->name)
        {pr_marco[imarco]=icblks[iblk];}
      }
    }
  }
  return 1;
}

void
free_icblks(IN int nblk,
            IN t_pr_marco* icblks
           )
{
  int iblk;
  for(iblk=0;iblk<nblk;++iblk)
  {free(icblks+iblk);}
  return 1;
}

/*
 *Check whether unable to find the start block
 *when all blocks has been labeled as STARTED
 */
void
check_start_error(IN int nblk,
                  IN t_pr_marco* blks
                 )
{
  int iblk=0;
  boolean status_err=TRUE;
  for (iblk=0;iblk<nblk;++iblk)
  {
    if (UNSTART==(blks+iblk)->start)
    {status_err=FALSE;}
  }
  if (TRUE==status_err)
  {
    printf("Fail to find the proper IC block to start placement!\n");
    abort();
    exit(1);
  }
}

/*
 *Check whether all the ic blocks have been placed.
 *This sub is quote during placement. If the ic blocks
 *have been placed, the placement will be over.
 */
boolean
check_place_over(IN int nblk,
                 IN t_pr_marco* icblks
                )
{
  int iblk=0;
  boolean place_over=TRUE;
  for (iblk=0;iblk<nblk;++iblk)
  {
    if (UNPLACED==(blks+iblk)->start)
    {place_over=FALSE;return place_over;}
  }
  return place_over;
}

/*
 *Check whether all the ic blocks have been placed.
 *This sub is quote after placement. If the ic blocks
 *have not been placed, which means the bread board is 
 *incapable of contain so many IC blocks, the program will
 *end and hint users to change parameters of software or the 
 *size of bread board.
 */
void
check_all_placed(IN int nblk,
                 IN t_pr_marco* icblks
                )
{
  int iblk=0;
  boolean place_overflow=TRUE;
  for (iblk=0;iblk<nblk;++iblk)
  {
    if (UNPLACED==(blks+iblk)->start)
    {place_overflow=FALSE;}
  }
  if (FALSE==place_overflow)
  {
    printf("Error:Unable to place all the IC blocks on the limit size of breadboard!\n");
    printf("Please modified software settings or alter the size of breadboard.\n");
    abort();
    exit(1);
  }
  return 1;
}

/*
 *Roughly put these blocks in the selected
 *column.
 */
void
place_on_bb(IN int nblk,
            INOUT t_pr_marco* icblks,
            IN int icolumn,
            IN int blkwidth,
            IN t_bb_array bb_array
           )
{
  int iblk=0;
  t_pr_marco* blktmp=NULL;
  int inter=0;  
  int ninter=0;
  int curr_width=0;

  for(iblk=0;iblk<nblk;++iblk)
  {
    blktmp=icblks+iblk;
    if ((PLACED==blktmp->status)&&(icolumn==blktmp->placed_column))
    {ninter++;}
  }

  ninter++;
  inter=int((bb_array.width-blkwidth)/ninter);
    
  for(iblk=0;iblk<nblk;++iblk)
  {
    blktmp=icblks+iblk;
    if ((PLACED==blktmp->status)&&(icolumn==blktmp->placed_column))
    {
      curr_width+=blktmp->place_width;
      blktmp->loc_x=bb_array.(column+icolumn)->start_x+inter+curr_width;
      blktmp->loc_y=bb_array.(column+icolumn)->start_y;
    }
  }
  
  return 1;
}
