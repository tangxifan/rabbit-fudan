#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>



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
  int xstart=bb_array->columns[column].base.x;
  int iy=0;
  int ystart=bb_array->columns[column].base.y;
  int xend=ix+bb_array->columns[column].width;
  int yend=iy+bb_array->columns[column].height;
  int ibb=0;
  int icol=0;
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

  
  bbs=(t_location*)malloc((*nbbs)*sizeof(t_location));
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
  float tmpcost=0.0;
  t_location* rloc=&(spin->location);
  t_location tmploc={0};
  t_location nearloc={0};
  int ix=spin->location.x;
  int iy=spin->location.y;
  int max_len=spin->parent->device->max_length;
  int itop=max_len+ix;
  int iend=bb_array->columns[spin->nets->pcolumn].base.x
          +bb_array->columns[spin->nets->pcolumn].width;
  if (itop>iend)
  {itop=iend;}

  /*Search the right side for a routable bb node*/
  ix+=spin->parent->device->min_length;
  while(ix<itop)
  {
    tmpcost=0.0;
    set_location_value(&tmploc,ix,iy);
    if (FALSE==check_bb_node_unroutable(&tmploc,bb_array))
    {
      set_location_value(&tmploc,ix,iy);
    }
    else if (TRUE==check_bb_node_vnet(&tmploc,spin->nets,bb_array))
    {
	  if (FALSE==check_bb_node_occupied(&tmploc,bb_array))
      {
	    set_location_value(&tmploc,ix,iy);
        rcost=tmpcost;
        (*dloc)=tmploc;
        break;
	  }
      else
	  {
	    find_near_node_on_bb(&tmploc,bb_array,&nearloc);
        if (UNKNOWN==nearloc.y)
        {ix++;continue;}
	  }
    }
    else
    {ix++;continue;}
    tmpcost+=(float)find_manhattan_distance(rloc,&tmploc);
    tmpcost+=(float)find_manhattan_distance(&tmploc,&(dpin->location));
    tmpcost+=get_bb_node_route_cost(rloc,bb_array);
    tmpcost+=get_bb_node_route_cost(&tmploc,bb_array);
    tmpcost+=get_bb_node_route_cost(&(dpin->location),bb_array);
    if (rcost=0.0)
    {
      rcost=tmpcost;
      (*dloc)=tmploc;
    }
    else if (rcost>tmpcost)
    { 
      rcost=tmpcost;
      (*dloc)=tmploc;
    }
    ix++;
  }
  if  (1==DEBUG)
  {printf ("Try Right: Chosen location(%d,%d), Route Cost: %f\n",dloc->x,dloc->y,rcost);}
  return rcost; 
}

/*
 *Search the left side of source pin(spin)
 *to find a reasonable location for extra destination pin.
 */
float
try_left_route_pin_on_bb(IN t_pr_pin* spin,
                         IN t_pr_pin* dpin,
                         IN t_location* dloc,
                         IN t_bb_array* bb_array
                         )
{
  float rcost=UNKNOWN;
  float tmpcost=0.0;
  t_location* rloc=&(spin->location);
  t_location tmploc={0};
  t_location nearloc={0};
  int ix=spin->location.x;
  int iy=spin->location.y;
  int max_len=spin->parent->device->max_length;
  int itop=ix-max_len;
  int iend=bb_array->columns[spin->nets->pcolumn].base.x;
  if (itop<iend)
  {itop=iend;}
 
  ix-=spin->parent->device->min_length;
  /*Search the left side for a routable bb node*/
  while(ix>itop)
  {
    tmpcost=0.0;
    set_location_value(&tmploc,ix,iy);
    if (FALSE==check_bb_node_unroutable(&tmploc,bb_array))
    {
      set_location_value(&tmploc,ix,iy);
    }
    else if (TRUE==check_bb_node_vnet(&tmploc,spin->nets,bb_array))
    {
	  if (FALSE==check_bb_node_occupied(&tmploc,bb_array))
	  {
	    set_location_value(&tmploc,ix,iy);
        rcost=tmpcost;
        (*dloc)=tmploc;
		break;
	  }
	  else
      {
	    find_near_node_on_bb(&tmploc,bb_array,&nearloc);
        if (UNKNOWN==nearloc.y)
        {ix--;continue;}
      }
    }
    else
    {ix--;continue;}
	if (1==DEBUG)
	{printf("Current tmpcost is %f\n",tmpcost);}
    tmpcost+=(float)find_manhattan_distance(rloc,&tmploc);
	if (1==DEBUG)
	{printf("Current tmpcost is %f\n",tmpcost);}
    tmpcost+=(float)find_manhattan_distance(&tmploc,&(dpin->location));
	if (1==DEBUG)
	{printf("Current tmpcost is %f\n",tmpcost);}
    tempcost+=get_bb_node_route_cost(rloc,bb_array);
	if (1==DEBUG)
	{printf("Current tmpcost is %f\n",tmpcost);}
    tmpcost+=get_bb_node_route_cost(&tmploc,bb_array);
	if (1==DEBUG)
	{printf("Current tmpcost is %f\n",tmpcost);}
    tmpcost+=get_bb_node_route_cost(&(dpin->location),bb_array);
	if (1==DEBUG)
	{printf("Current tmpcost is %f\n",tmpcost);}
    if (UNKNOWN==rcost)
    {
      rcost=tmpcost;
      (*dloc)=tmploc;
    }
    else if (rcost>tmpcost)
    { 
      rcost=tmpcost;
      (*dloc)=tmploc;
    }
    ix--;
  }
  if  (1==DEBUG)
  {printf ("Try Left: Chosen location(%d,%d), Route Cost: %f\n",dloc->x,dloc->y,rcost);}
  return rcost; 
}

float
try_route_marco_pin_on_bb(IN t_pr_pin* src,
                          IN t_pr_pin* des,
                          IN t_location* desloc,
                          IN t_bb_array* bb_array
                         )
{
  t_location loclft={0};
  t_location locrgt={0};
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
  return UNKNOWN;
}

float
try_left_find_node(IN t_pr_pin* pin,
                   IN t_location* leftloc,
                   IN t_bb_array* bb_array
                  )
{
  int ix=pin->parent->location.x;
  int itop=bb_array->columns[pin->parent->pcolumn].base.x;
  t_location* loc=NULL;
  float cost=0.0;
  while (ix>itop)
  {
    set_location_value(loc,ix,pin->location.y);
    if (FALSE==check_bb_node_unroutable(loc,bb_array))
    {
      set_location_value(leftloc,ix,pin->location.y);
      cost=find_manhattan_distance(leftloc,loc)
          +get_bb_node_route_cost(leftloc,bb_array)
          +get_bb_node_route_cost(loc,bb_array);
    }
    ix--;
  }
  return cost;
}

float
try_right_find_node(IN t_pr_pin* pin,
                    IN t_location* rightloc,
                    IN t_bb_array* bb_array
                   )
{
  int ix=pin->parent->location.x
        +pin->parent->device->width;
  int itop=bb_array->columns[pin->parent->pcolumn].base.x
          +bb_array->columns[pin->parent->pcolumn].width;
  t_location* loc=NULL;
  float cost=0.0;
  while (ix<itop)
  {
    set_location_value(loc,ix,pin->location.y);
    if (FALSE==check_bb_node_unroutable(loc,bb_array))
    {
      set_location_value(rightloc,ix,pin->location.y);
      cost=find_manhattan_distance(rightloc,loc)
          +get_bb_node_route_cost(rightloc,bb_array)
          +get_bb_node_route_cost(loc,bb_array);
    }
    ix++;
  }
  return cost;
}
