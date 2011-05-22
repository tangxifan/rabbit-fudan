#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>

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
  float mincost=try_route_marco_on_bb(marcos,bb_array);
  t_pr_marco* mchn=marcos;
  t_pr_marco* marco=NULL;
  set_route_vnets_on_bb(nvnet,vnets,bb_array);
  update_bb_array_route_cost(bb_array);
  set_route_icblks_on_bb(nmarco,marcos,bb_array);
  update_bb_array_route_cost(bb_array);
  while(1)
  {
    for (imarco=0;imarco<nmarco;++imarco)
    {
      marco=marcos+imarco;
      if ((ICBLOCK!=marco->type)&&(VDD!=marco->type)&&(GND!=marco->type))
      {
        tmpcost=try_route_marco_on_bb(marcos+imarco,bb_array);
        if (tmpcost<mincost)
        {
          mincost=tmpcost;
          mchn=marcos+imarco;
        }
      }
    }
    finish_route_marco_on_bb(mchn,bb_array);
    update_bb_array_route_cost(bb_array);
    if (check_all_routed(nvnet,vnets,nmarco,marcos))
    {break;}
  }
  try_route_bias(nvnet,vnets,nmarco,marcos,bb_array);
  
  return TRUE;  
}
