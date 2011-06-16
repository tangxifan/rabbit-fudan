#include <stdio.h>
#include <stdlib.h>

#include "rabbit_type.h"
#include "bb_type.h"
#include "place_route.h"
#include "device.h"

boolean
try_route(IN int nvnet,
          IN t_vnet* vnets,
          IN int nmarco,
          INOUT t_pr_marco* marcos,
          INOUT t_bb_array* bb_array
          )
{
  int imarco=0;
  float tmpcost=0.0;
  float mincost=0.0;
  t_pr_marco* mchn=NULL;
  t_pr_marco* marco=NULL;

  printf("Start routing...\n");

  set_route_vnets_on_bb(nvnet,vnets,bb_array);
  update_bb_array_route_cost(bb_array);
  set_route_icblks_on_bb(nmarco,marcos,bb_array);
  update_bb_array_route_cost(bb_array);

  while(1)
  {
    mincost=0.0;
	mchn=NULL;
    for (imarco=0;imarco<nmarco;++imarco)
    {
      marco=marcos+imarco;
      if ((ICBLOCK!=marco->type)&&(VDD!=marco->type)&&(GND!=marco->type)&&(UNROUTE==marco->rstatus))
      {
	    printf("Try route marco[%d]...",marco-marcos);
        tmpcost=try_route_marco_on_bb(marco,bb_array);
		printf("route cost: %f\n",tmpcost);
		if ((NULL==mchn)||(tmpcost<mincost))
        {
          mincost=tmpcost;
          mchn=marco;
        }
      }
    }
	printf("Marco[%d] has been chosen to route...\n",mchn-marcos);
    finish_route_marco_on_bb(mchn,bb_array);
	printf("Finish!\n");
    update_bb_array_route_cost(bb_array);
	printf("Updating bread board route cost...\n");
    if (TRUE==check_all_routed(nvnet,vnets,nmarco,marcos))
    {break;}
  }
  try_route_bias(nvnet,vnets,nmarco,marcos,bb_array);
  
  return TRUE;  
}
