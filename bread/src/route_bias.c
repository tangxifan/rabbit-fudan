#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>

int
route_bias_on_bb(IN t_pr_marco* bias,
                 IN t_bb_array* bb_array,
                 IN enum e_bias_type bias_type
               )
{
  int ipin=0;
  t_vnet* vnet=NULL;
  /*Source location belongs to virtual net.*/
  /*Destination location belongs to bias nodes.*/
  t_location* srcloc=NULL;
  t_location* desloc=NULL;
  for(ipin=0;ipin<bias->pinnum;++ipin)
  {
    vnet=(*(bias->pins+ipin))->nets;
    try_route_bias_vnet_on_bb(srcloc,desloc,vnet,bias_type,bb_array);
    set_bb_node_occupied(srcloc,bb_array);
    set_bb_node_occupied(desloc,bb_array);
    set_wired_on_bb(srcloc,desloc,bb_array);
    set_wired_on_bb(desloc,srcloc,bb_array);
  }
  return 1;
}

int
try_route_bias_vnet_on_bb(IN t_location* srcloc,
                          IN t_location* desloc,
                          IN t_vnet* vnet,
                          IN enum e_bias_type bias_type,
                          IN t_bb_array* bb_array
                         )
{
  int ix=0;
  int iy=0;
  t_location* srctmp=NULL;
  t_location* destmp=NULL;
  for (ix=0;ix<bb_array->width;++ix)
  {
    for (iy=0;iy<bb_array->height;++iy)
    {
      set_location_value(srctmp,ix,iy);
      if (check_bb_node_vnet(srctmp,vnet,bb_array)&&(!check_bb_node_occupied(srctmp,bb_array)))
      {try_find_bias_on_bb(srctmp,destmp,bias_type,bb_array);}
    }
  }
  return 1;
}

int
try_find_bias_on_bb(IN t_location* src,
                    IN t_location* des,
                    IN enum e_bias_type bias_type,
                    IN t_bb_array* bb_array
                   )
{
  int ix=0;
  int iy=0;
  float mincost=UNKNOWN;
  float tmpcost=0.0;
  t_location* destmp=NULL;
  for (ix=0;ix<bb_array->width;++ix)
  {
    for (iy=0;iy<bb_array->height;++iy)
    {
      set_location_value(destmp,ix,iy);
      if (check_bb_bias_type(destmp,bias_type,bb_array)&&(!check_bb_node_occupied(src,bb_array)))
      {
        tmpcost=find_manhattan_distance(src,destmp)
               +get_bb_node_route_cost(src,bb_array)
               +get_bb_node_route_cost(destmp,bb_array);
        if ((UNKNOWN==mincost)||(mincost>tmpcost))
        {mincost=tmpcost;des=destmp;}
      }
    }
  }
  return 1;
}


/*
 *
 */
int
try_route_bias(IN int nvnet,
               IN t_vnet* vnets,
               IN int nmarco,
               IN t_pr_marco* marcos,
               IN t_bb_array* bb_array
              )
{
  int imarco=0;
  t_pr_marco* mvdd=NULL;
  t_pr_marco* mgnd=NULL;
  for(imarco=0;imarco<nmarco;++imarco)
  {
    if (VDD==(marcos+imarco)->type)
    {mvdd=marcos+imarco;}
    else if (GND==(marcos+imarco)->type)
    {mgnd=marcos+imarco;}
  }
  //determine_vdd_gnd_location_on_bb(mvdd,mgnd,bb_array);
  route_bias_on_bb(mvdd,bb_array,VDD);
  route_bias_on_bb(mgnd,bb_array,GND);
  return 1;
}




