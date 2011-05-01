#include <stdio.h>
#include <stdlib.h>

#include <rabbit_types.h>
#include <util.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <setup_rabbit.h>


void 
alter_route_cost(IN t_bb_node* bb_node)
{
  int in=0;
  if (OCUPPIED==bb_node->status)
  {
    for (in=0;in<bb_node->ninner;++in)
    {*(bb_node->(inners+in))->rcost+=bb_node->rcost/ninner;}
  }
  return 1;
}

void
set_location_value(INOUT t_location* location,
                   IN int locx,
                   IN int locy
                  )
{
  location->x=locx;
  location->y=locy;
  return 1;
}

boolean
check_bb_node_blank(IN t_location* src,
                    IN t_bb_array* bb_array
                   )
{
  if (BLANK==bb_array->bb_node[src->x][src->y].type)
  {return TRUE;}
  else
  {return FALSE;}
}

boolean
check_bb_bias_type(IN t_location* src,
                   IN enum e_bias_type bias_type,
                   IN t_bb_array* bb_array
                  )
{
  if (bias_type==bb_array->bb_node[src->x][src->y].bias_type)
  {return TRUE;}
  else
  {return FALSE;}
}

boolean
check_bb_node_occupied(IN t_location* src,
                       IN t_bb_array* bb_array
                      )
{
  if (OCCUPIED==bb_array->bb_node[src->x][src->y].type)
  {return TRUE;}
  else
  {return FALSE;}
}


boolean
check_bb_node_unroutable(IN t_location* src,
                         IN t_bb_array* bb_array
                        )
{
  if (UNROUTABLE==bb_array->bb_node[src->x][src->y].rstatus)
  {return TRUE;}
  else
  {return FALSE;}
}

boolean
check_bb_node_vnet(IN t_location* src,
                   IN t_vnet* vnet,
                   IN t_bb_array* bb_array
                  )
{
  if (vnet==bb_array->bb_node[src->x][src->y].net)
  {return TRUE;}
  else
  {return FALSE;}
}

/*
 *Calculate the mahattan distance with given
 *source x,y and destination x,y.
 */
int 
find_manhattan_distance(IN t_location* src,
                        IN t_location* des
                        )
{
  return (abs(src.x-des.x)+abs(src.y-des.y));
}

float
get_bb_node_route_cost(IN t_location* src,
                       IN t_bb_array* bb_array
                      )
{
  return bb_array->bb_node[src->x][src->y].rcost;
}

void
set_bb_node_unroutable(IN t_location* src,
                       IN t_bb_array* bb_array                     
                      )
{
  bb_array->bb_node[src->x][src->y].rstatus=UNROUTABLE;
  return 1;
}

void
set_bb_net_unroutable(IN t_location* src,
                      IN t_bb_array* bb_array       
                     )
{
 int column=bb_array->bb_node[src->x][src->y].column;
 int iy=0;
 int in=bb_array->bb_node[src->x][src->y]->ninner;
 t_location loc=*(src);
 for (iy=0;iy<in;++iy)
 {
   loc=(*(bb_array->bb_node[src->x][src->y]->inners+iy))->location;
   if (!check_bb_node_blank(loc,bb_array))
   {set_bb_node_unroutable(&loc,bb_array);}
 }
 return 1;
}

void
set_bb_node_occupied(IN t_location* src,
                     IN t_bb_array* bb_array                     
                    )
{
  bb_array->bb_node[src->x][src->y].status=OCCUPIED;
  return 1;
}

t_location
find_top_inner_on_bb(IN t_location* src,
                     IN t_bb_array* bb_array
                    )
{
   t_location rt;
   int column=bb_array->bb_node[src->x][src->y].column;
   int iy=0;
   int ymin=src->y;
   int in=bb_array->bb_node[src->x][src->y]->ninner;
   t_location loc=*(src);
   for (iy=0;iy<in;++iy)
   {
     loc=(*(bb_array->bb_node[src->x][src->y]->inners+iy))->location;
     if ((loc.y<ymin)&&(!check_bb_node_occupied(loc,bb_array)))
     {ymin=loc.y;}
   }
   set_location_value(&rt,src->x,ymin);
   return rt;
}

t_location
find_near_node_on_bb(IN t_location* src,
                     IN t_bb_array* bb_array
                    )
{
   t_location rt;
   int column=bb_array->bb_node[src->x][src->y].column;
   int iy=0;
   int ymin=UNKNOWN;
   int in=bb_array->bb_node[src->x][src->y]->ninner;
   t_location loc=*(src);
   for (iy=0;iy<in;++iy)
   {
     loc=(*(bb_array->bb_node[src->x][src->y]->inners+iy))->location;
     if (!check_bb_node_occupied(loc,bb_array)))
     {     
       if  (UNKNOWN==ymin)
       {ymin=abs(loc.y-src->y);}
       else if (abs(loc.y-src->y)<ymin)
       {ymin=abs(loc.y-src->y);}
     }
   }
   set_location_value(&rt,src->x,ymin);
   return rt;
}

void
set_wired_on_bb(IN t_location* src,
                IN t_location* des,
                IN t_bb_array* bb_array
               )
{
  set_location_value(bb_array->bb_node[src->x][src->y].wired,des->x,des->y);
  return 1;
}

boolean
check_all_routed(IN int nvnet,
                 IN t_vnet* vnet,
                 IN int nmarco,
                 INOUT t_pr_marco* marcos
                )
{
  int imarco=0;
  int inet=0;
  for (inet=0;inet<nvnet;++inet)
  {
    if (UNROUTED==(vnet+inet)->rstatus)
    {return FALSE;}
  }
  for (imarco=0;imarco<nmarco;++imarco)
  {
    if (UNROUTED==(marcos+imarco)->rstatus)
    {return FALSE;}
  }
  return TRUE;
}
