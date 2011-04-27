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


float
cal_route_path_cost(IN t_location* src,
                    IN t_location* des,
                    IN t_bb_array* bb_array
                   )
{
  float path_cost=0.0;
  int dist=find_manhattan_distance(src,des);
  path_cost+=get_bb_node_route_cost(src,bb_array);
  path_cost+=get_bb_node_route_cost(des,bb_array);
  path_cost+=(float)dist;  
  return path_cost;
}

t_location*
find_vnet_bbs(IN int* nbbs,
              IN t_vnet* vnet,
              IN t_bb_array* bb_array
             )
{
  int column=vnet->pcolumn;
  int ix=0;
  int xstart=bb_array->(columns+column)->base->x;
  int iy=0;
  int ystart=bb_array->(columns+column)->base->y;
  int xend=ix+bb_array->(columns+column)->width;
  int yend=iy+bb_array->(columns+column)->height;
  int ibb=0;
  t_location* bbs=NULL;

  (*nbbs)=0;
  for (ix=xstart;ix<xend;++ix)
  {
    for(iy=ystart;iy<yend;++iy)
    {
      if ((vnet==bb_array->bb_node[ix][iy].net)&&(FREE==bb_array->bb_node[ix][iy].status))
      {(*nbbs)++;}
    }
  }
  
  bbs=(t_location*)my_malloc((*nbbs)*sizeof(t_location));
  for (ix=xstart;ix<xend;++ix)
  {
    for(iy=ystart;iy<yend;++iy)
    {
      if ((vnet==bb_array->bb_node[ix][iy].net)&&(FREE==bb_array->bb_node[ix][iy].status))
      {
       set_location_value(bbs+ibb,ix,iy);
       ibb++;
      }
    }
  }
  return bbs;
}

float
find_path_cost_vnets(IN t_location* srcloc,
                     IN t_location* desloc,
                     IN t_vnet* src,
                     IN t_vnet* des,
                     IN t_bb_array* bb_array
                    )
{
  int ibb=0;
  int jbb=0;
  float tmpcost=0.0;
  int nsrcbb=0;
  int ndesbb=0;
  t_location* srcbbs=find_vnet_bbs(&nsrcbb,src,bb_array);
  t_location* desbbs=find_vnet_bbs(&ndesbb,des,bb_array);
  float rcost=cal_route_path_cost(srcbbs+ibb,desbbs+jbb,bb_array);
  set_location_value(srcloc,(srcbbs+ibb)->x,(srcbbs+ibb)->y);
  set_location_value(desloc,(desbbs+jbb)->x,(desbbs+jbb)->y);

  for (ibb=0;ibb<nsrcbb;++ibb)  
  {
    for (jbb=0;jbb<ndesbb;++jbb)
    {
      tmpcost=cal_route_path_cost(srcbbs+ibb,desbbs+jbb,bb_array);
      if (tmpcost<rcost)
      {
        rcost=tmpcost;
        set_location_value(srcloc,(srcbbs+ibb)->x,(srcbbs+ibb)->y);
        set_location_value(desloc,(desbbs+jbb)->x,(desbbs+jbb)->y);
      }
    }
  }
  return rcost;
}



float
try_right_route_pin_on_bb(IN t_pr_pin* spin,
                          IN t_pr_pin* dpin,
                          IN t_location* dloc,
                          IN t_bb_array* bb_array
                         )
{
  float rcost=0.0;
  t_location* rloc=spin->location;
  int ix=spin->location->x;
  int iy=spin->location->y;
  int itop=bb_array->(columns+spin->parent->pcolumn)->base->x
          +bb_array->(columns+spin->parent->pcolumn)->width;
  int max_len=pin->parent->device->max_length;
//int itop=ix+pin->parent->device->max_length+1;
  /*Search the left side for a routable bb node*/
  while(ix<itop)
  {
    if (ROUTABLE==bb_array->bb_node[ix][iy]->rstatus)
    {
      set_location_value(dloc,ix,iy);
      break;
    }
    if ((ix-spin->location->x-max_len)>0)
    {rcost=UNKNOWN;return rcost;}
    ix++;
  }
  rcost+=find_manhattan_distance(rloc,dloc);
  rcost+=find_manhattan_distance(dloc,dpin->location);
  rcost+=get_bb_node_route_cost(rloc,bb_array);
  rcost+=get_bb_node_route_cost(dloc,bb_array);
  rcost+=get_bb_node_route_cost(dpin->location,bb_array);
  return rcost; 
}

float
try_left_route_pin_on_bb(IN t_pr_pin* spin,
                         IN t_pr_pin* dpin,
                         IN t_location* dloc,
                         IN t_bb_array* bb_array
                         )
{
  float rcost=0.0;
  t_location* rloc=spin->location;
  int ix=spin->location->x;
  int iy=spin->location->y;
  int itop=bb_array->(columns+spin->parent->pcolumn)->base->x-1;
  int max_len=pin->parent->device->max_length;
//int itop=ix+pin->parent->device->max_length+1;
  /*Search the left side for a routable bb node*/
  while(ix>itop)
  {
    if (ROUTABLE==bb_array->bb_node[ix][iy]->rstatus)
    {
      set_location_value(dloc,ix,iy);
      break;
    }
    if ((spin->location->x-ix-max_len)>0)
    {rcost=UNKNOWN;return rcost;}
    ix--;
  }
  rcost+=find_manhattan_distance(rloc,dloc);
  rcost+=find_manhattan_distance(dloc,dpin->location);
  rcost+=get_bb_node_route_cost(rloc,bb_array);
  rcost+=get_bb_node_route_cost(dloc,bb_array);
  rcost+=get_bb_node_route_cost(dpin->location,bb_array);
  return rcost; 
}

float
try_route_marco_pin_on_bb(IN t_pr_pin* src,
                          IN t_pr_pin* des,
                          IN t_location* desloc,
                          IN t_bb_array* bb_array
                         )
{
  t_location loclft=NULL;
  t_location locrgt=NULL;
  float cleft=try_left_route_pin_on_bb(src,des,&loclft,bb_array);
  float cright=try_right_route_pin_on_bb(src,des,&locrgt,bb_array);
  if ((UNKNOWN==cleft)&&(UNKNOWN==cright))
  {
    printf("Error: Fail to route the pin on the bread board!\n");
    abort();
    exit(1);
  }
  else if (UNKNOWN==cleft)
  {
    (*desloc)=locrgt;
    return cright;
  }
  else if (UNKNOWN==cright)  
  {
    (*desloc)=loclft;
    return cleft;  
  }
  else
  {
    if (cleft<cright)
    {
      (*desloc)=loclft;
      return cleft;
    }
    else
    {
      (*desloc)=locrgt;
      return cright;
    }
  } 
}


