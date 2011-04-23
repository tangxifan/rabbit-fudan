#include <stdio.h>
#include <stdlib.h>

#include <rabbit_types.h>
#include <util.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <setup_rabbit.h>

void
set_route_blks_on_bb(IN int nblk,
                     IN t_pr_marco* blks,
                     IN t_bb_array* bb_array
                    )
{
  int iblk=0;
  for (iblk=0;iblk<nblk;++iblk)
  {
    if (ICBLOCK==(blks+iblk)->type)
    {
      
    }
  }
  return 1;
}

void
set_route_vnets_on_bb(IN int nvnet,
                      IN t_vnet* vnets,
                      IN t_bb_array* bb_array
                      )
{

}
