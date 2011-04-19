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
          IN t_vnet* vnets,
          IN int nmarco,
          INOUT t_pr_marco* pr_marco,
          INOUT t_bb_array* bb_array
         )
{
  int nblk=0;
  int iblk=0;
  t_pr_marco* icblks=NULL;
  t_pr_marco* starter_blk=NULL;
  t_pr_marco* blkmatch=NULL;
  t_place_info place_info;

  int inet=0;
  int icolumn=0;
  int ncolumn=bb_array->no_column;
  int wcapacity=0;
  
  place_info.column=0;

  create_icblk_array(&nblk,icblks,nmarco,pr_marco);
  while(place_info.column<ncolumn)
  {
    place_info->cur_width=0;
    place_info->bb_pwidth=(1-bb_array->reserve_ratio)*bb_array->width;
    clear_left_right_place_info(&place_info);
    wcapacity=bb_array->(columns+place_info.column)->width_capacity;
    find_starter(nblk,icblks,nvnet,vnets,&place_info,*bb_array);
    while(1)
    {
      if (FALSE==find_match(nblk,icblks,nvnet,vnets,bb_array,&place_info))
      {
        climbing_place(nblk,icblks,nvnet,vnets,bb_array,&place_info);
        break;
      }
    }
    /*Operation on the bb_array information*/
    bb_array->(columns+place_info.column)->usedwidth=place_info->cur_width;
    bb_array->(columns+place_info.column)->left=get_left_place_info(place_info);
    bb_array->(columns+place_info.column)->right=get_left_place_info(place_info);
    if (TRUE==check_all_placed(nblk,icblks,nvnet,vnets))
    {break;}
    place_info.column++;
  }
  check_all_placed(nblk,icblks,nvnet,vnets);
  update_marco_free_icblks(nmarco,pr_marco,nblk,icblks);
  return TRUE;
}

/*
 *Do the climbing placement.
 */
void
climbing_place(IN int nblk,
               INOUT t_pr_marco* blks,
               IN int nvnet,
               IN t_vnet* vnets,
               IN t_bb_array bb_array,
               IN t_place_info* place_info
               )
{
  int iblk=0;
  int inet=0;
  int widthtmp=0;
  int minwid=0;
  boolean chntype=TRUE;
  t_pr_marco* blkchn=NULL;
  t_vnet* netchn=NULL;
  int wcapacity=bb_array.columns[place_info->column].width_capacity;
  
  minwid=find_blk_pwidth(blks,wcapacity);
  widthtmp=find_vnet_pwidth(vnets,wcapacity);
  if (minwid>widthtmp)
  {minwid=widthtmp;}
 
  while(1)
  {
    for(iblk=0;iblk<nblk;++iblk)
    {
      if (UNPLACED==(iblk+blks)->status)
      {
        widthtmp=find_blk_pwidth((blks+iblk),wcapacity);
        if (minwid>widthtmp)
        {
          minwid=widthtmp;
          blkchn=blks+iblk;
          chntype=TRUE;
        }
      }
    }
    for(inet=0;inet<nvnet;++inet)
    {
      if (UNPLACED==(inet+vnets)->status)
      {
        widthtmp=find_vnet_pwidth((vnets+inet),wcapacity);
        if (minwid>widthtmp)
        {
          minwid=widthtmp;
          netchn=vnets+inet;
          chntype=FALSE;
        }
      }
    }
        
    if ((minwid+place_info->cur_width)<place_info->bb_pwidth)
    {
      if (TRUE==chntype)
      {
        determine_blk_place_location(blkchn,place_info);
        blkchn->pcolumn=place_info->column;
        blkchn->status=PLACED;
      }
      else
      {
        determine_net_place_location(netchn,place_info);
        netchn->pcolumn=place_info->column;
        netchn->status=PLACED;
      }
    }
    else
    {break;}
  }
  return 1;
}





