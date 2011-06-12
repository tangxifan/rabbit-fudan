#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>

/******************Subroutines*****************/
int
update_pr_marco(IN int nmarco,
                INOUT t_pr_marco* pr_marco,
                IN int nblk,
                IN t_pr_marco* icblks
                );



/**********************************************/

/*
 *Create a array for storing ic blocks.
 *First, select these qualified ic blocks,
 *Then put them into the array.
 */
int
create_icblk_array(INOUT int* nblk,
                   INOUT t_pr_marco* icblks,
                   IN int nmarco,
                   IN t_pr_marco *marcos
                  )
{
  int imarco=0;
  int iblk=0;
  enum e_pr_type mcotype=0;
  (*nblk)=0;
  
  printf("Counting IC blocks...");
  for (imarco=0;imarco<nmarco;++imarco)
  {
    mcotype=(marcos+imarco)->type;
    if (ICBLOCK==mcotype)
    {iblk++;}
  }
  printf("Result:%d\n",iblk);
  (*nblk)=iblk;
  icblks=(t_pr_marco*)malloc((*nblk)*sizeof(t_pr_marco));
  printf("Creating IC blocks...\n");

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

int
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
  free(icblks);
}


/*
 *Check whether unable to find the start block
 *when all blocks has been labeled as STARTED
 */
void
check_start_error(IN int nblk,
                  IN t_pr_marco* blks,
                  IN int nvnet,
                  IN t_vnet* vnets
                 )
{
  int iblk=0;
  int inet=0;
  boolean status_err=TRUE;
  for (iblk=0;iblk<nblk;++iblk)
  {
    if (UNSTART==(blks+iblk)->sstart)
    {status_err=FALSE;}
  }
  for(inet=0;inet<nvnet;++inet)
  {
    if (UNSTART==(vnets+inet)->sstart)
    {status_err=FALSE;}
  }
  if (TRUE==status_err)
  {
    printf("Fail to find the proper IC block or virtual net to start placement!\n");
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
                 IN t_pr_marco* blks,
                 IN int nvnet,
				 IN t_vnet* vnets
                )
{
  int iblk=0;
  int inet=0;
  boolean place_over=TRUE;

  if (nblk!=0)
  {
    for (iblk=0;iblk<nblk;++iblk)
    {
      if (UNPLACED==(blks+iblk)->status)
      {place_over=FALSE;return place_over;}
    }
  }
  
  if (nvnet!=0)
  {
    for (inet=0;inet<nvnet;++inet)
    {
      if ((UNPLACED==vnets[inet].status)&&(SPECIAL==vnets[inet].type))
      {place_over=FALSE;return place_over;}
    }
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
int
check_all_placed(IN int nblk,
                 IN t_pr_marco* blks,
                 IN int nvnet,
                 IN t_vnet* vnets
                )
{

  boolean place_overflow=check_place_over(nblk,blks,nvnet,vnets);

  if (FALSE==place_overflow)
  {
    printf("Error:Unable to place all the IC blocks on the limit size of breadboard!\n");
    printf("Please modify software settings or alter the size of breadboard.\n");
    abort();
    exit(1);
  }
  return 1;
}

int 
initial_place_info(t_place_info* place_info)
{
  place_info->left=(t_pr_node*)malloc(sizeof(t_pr_node));
  place_info->right=(t_pr_node*)malloc(sizeof(t_pr_node));
  return 1;
}

int 
clear_left_right_place_info(t_place_info* place_info)
{
  place_info->left->flag=TRUE;
  place_info->left->mnext=NULL;
  place_info->left->vnext=NULL;
  place_info->right->flag=TRUE;
  place_info->right->mnext=NULL;
  place_info->right->vnext=NULL;
  return 1;
}

int 
get_left_place_info(t_place_info place_info)
{
  if (TRUE==place_info.left->flag)
  {
    return place_info.left->mnext->name;
  }
  else
  {
    return place_info.left->vnext->name;
  }
}

int 
get_right_place_info(t_place_info place_info)
{
  if (TRUE==place_info.right->flag)
  {
    return place_info.right->mnext->name;
  }
  else
  {
    return place_info.right->vnext->name;
  }
}
