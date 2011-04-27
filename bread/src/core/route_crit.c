#include <stdio.h>
#include <stdlib.h>

#include <rabbit_types.h>
#include <util.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <setup_rabbit.h>

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

void
determine_block_base_on_bb(IN t_pr_marco* blk,
                           IN t_bb_array* bb_array
                          )
{
  int column=blk->pcolumn;
  int bbheight=bb_array->(columns+column)->height;
 
  blk->location.y=bb_array->(columns+column)->base->y
                 +int((bbheigth-blk->device->height)/2);

  if (blk->location.y>bb_array->(columns+column)->blank_start)
  {blk->location.y=bb_array->(columns+column)->blank_start;}

  if (bbheight<blk->device->height)
  {
    printf("The block height overwhelms the height of breadboard column!\n");
    abort();
    exit(1);
  }
  return 1;
}

void
determine_pins_of_blk_on_bb(IN t_pr_marco* blk,
                            IN t_bb_array* bb_array
                           )
{
  int ipin=0;
  t_pr_pin* curpin=NULL;
  for(ipin=0;ipin<blk->pinnum;++ipin)
  {
    curpin=*(blk->pins+ipin);
    if (TOP==curpin->loc)
    {
      curpin->location->x=blk->location->x+curpin->offset;
      curpin->location->y=blk->location->y-1;
    }
    if (BOTTOM==curpin->loc)
    {
      curpin->location->x=blk->location->x+curpin->offset;
      curpin->location->y=blk->location->y+blk->height+1;
    }
    if (LEFT==curpin->loc)
    {
      curpin->location->x=blk->location->x-1;
      curpin->location->y=blk->location->y+curpin->offset;
    }
    if (RIGHT==curpin->loc)
    {
      curpin->location->x=blk->location->x+blk->width+1;
      curpin->location->y=blk->location->y+curpin->offset;
    }
    /*Check whether the location of pin on bread board have been occupied*/
    if (bb_array->bb_node[curpin->location->x][curpin->location->y].status!=FREE)
    {
      printf("Error:Fail to put the pin on breadboard\n");
      abort();
      exit(1);
    }
    else
    {
      bb_array->bb_node[curpin->location->x][curpin->location->y].pin=curpin;
      bb_array->bb_node[curpin->location->x][curpin->location->y].net=curpin->nets;
      set_bb_net_unroutable(curpin->location,bb_array);
      set_bb_node_occupied(curpin->location,bb_array);
      set_bb_node_unroutable(curpin->location,bb_array);
      curpin->nets->rstatus=ROUTED;
    }
  }
  blk->rstatus=ROUTED;
  return 1;
}


void
route_blk_on_bb(IN t_pr_marco* blk,
                IN t_bb_array* bb_array
               )
{
  /*Find the block available nodes on breadboard*/
  determine_block_base_on_bb(blk,bb_array);
  /*Set the pins of given blk on the breadboard*/
  determine_pins_of_blk_on_bb(blk,bb_array);
  blk->rstatus=ROUTED;
  return 1;
}

void
set_route_icblks_on_bb(IN int nblk,
                       IN t_pr_marco* blks,
                       IN t_bb_array* bb_array
                      )
{
  int iblk=0;
  for (iblk=0;iblk<nblk;++iblk)
  {
    if (ICBLOCK==(blks+iblk)->type)
    {route_blk_on_bb(blks+iblk,bb_array);}
  }
  return 1;
}

/*
 *To route a virtual net on the bread board.
 *Several steps need to be done.
 *I. Set the route status of those bb node 
 *   used by virtual net as UNROUTABLE.
 *   If the one of the node is UNROUTABLE,
 *   the error will be reported.
 *II.Set wire connections for the virtual net.
 */
void
route_vnet_on_bb(IN t_vnet* vnet,
                 IN t_bb_array* bb_array
                )
{
  int column=vnet->pcolumn;
  int iloc=0;
  int iy=0;
  int ystart=bb_array->(columns+column)->base->y;
  int yend=bb_array->(columns+column)->base->y+bb_array->(columns+column)->height;
  
  int blankstart=0;
  int blankend=0;

  for (iloc=0;iloc<vnet->locnum;++iloc)
  {
    /*Complete I.*/
    for (iy=ystart;iy<yend;++iy)
    {
      if (ROUTABLE==bb_array->bb_node[vnet->(location+iloc)->x][iy].rstatus) 
      {bb_array->bb_node[vnet->(location+iloc)->x][iy].rstatus=UNROUTABLE;}
      else
      {
        printf("Error: the bb node[%d][%d] is unroutable when routing virtual net on it!\n",vnet->(location+iloc)->x,iy);
        abort();
        exit(1);
      }
    }
    /*Complete II.*/
    blankstart=bb_array->(columns+column)->blank_start;
    blankend=bb_array->(columns+column)->blank_end;
    bb_array->bb_node[vnet->(location+iloc)->x][blankstart-1].status=OCCUPIED;
    set_location_value(bb_array->bb_node[vnet->(location+iloc)->x][blankstart-1].wired,vnet->(location+iloc)->x,blankend+1);
    bb_array->bb_node[vnet->(location+iloc)->x][blankend+1].status=OCCUPIED;
    set_location_value(bb_array->bb_node[vnet->(location+iloc)->x][blankend+1].wired,vnet->(location+iloc)->x,blankstart-1);
    if (locnum>iloc+1)
    {
      if (OCCUPIED!=bb_array->bb_node[vnet->(location+iloc)->x][blankstart-2].status)
      {
        bb_array->bb_node[vnet->(location+iloc)->x][blankstart-2]=OCCUPIED;
        set_location_value(bb_array->bb_node[vnet->(location+iloc)->x][blankstart-2].wired,vnet->(location+iloc+1)->x,blankstart-2);
        bb_array->bb_node[vnet->(location+iloc+1)->x][blankstart-2]=OCCUPIED;
        set_location_value(bb_array->bb_node[vnet->(location+iloc+1)->x][blankstart-2].wired,vnet->(location+iloc)->x,blankstart-2);
      }
      else if (OCCUPIED!=bb_array->bb_node[vnet->(location+iloc)->x][blankend+2].status)
      {
        bb_array->bb_node[vnet->(location+iloc)->x][blankend+2]=OCCUPIED;
        set_location_value(bb_array->bb_node[vnet->(location+iloc)->x][blankend+2].wired,vnet->(location+iloc+1)->x,blankend+2);
        bb_array->bb_node[vnet->(location+iloc+1)->x][blankend+2]=OCCUPIED;
        set_location_value(bb_array->bb_node[vnet->(location+iloc+1)->x][blankend+2].wired,vnet->(location+iloc)->x,blankend+2);
      }
      else
      {
        printf("Error: Unable to route the wire connections of the virtual net on bread board!\n");
        abort();
        exit(1);
      }
    }
  }
  vnet->rstatus=ROUTED;
  return 1;
}

void
set_route_vnets_on_bb(IN int nvnet,
                      IN t_vnet* vnets,
                      IN t_bb_array* bb_array
                      )
{
  int inet=0;
  for (inet=0;inet<nvnet;++inet)
  {
    if (SPECIAL==(vnets+inet)->type)
    {route_vnet_on_bb(vnets+inet,bb_array);}
  }
  return 1;
}

/*
 *To find the lowest cost way in routing the 
 *marco on bread board.
 *Notice that the marco should be a two-port device(RCD).
 *Multi-port device are routed in another function.
 *This subroutine try to route the marco on bread board. 
 *Route locations are temporarily stored in the struct t_locations.
 *When the route engine decide to route the marco in this round,
 *the route locations are available and bring convience in routing.
 */
float
try_route_marco_on_bb(IN t_pr_marco* marco,
                      IN t_bb_array* bb_array
                     )
{
  int rcost=0.0;
  
  t_location* srcloc=NULL;
  t_location* desloc=NULL;
  t_vnet* srcnet=NULL;
  t_vnet* desnet=NULL;

  if ((ICBLOCK==marco->type)||(VDD==marco->type)||(GND==marco->type))
  {return rcost;}
  else
  {
    if (marco->pinnum!=2)
    {
      printf("Warning: the marco have %d pins which is expected to be 2!\n",marco->pinnum);
      printf("Warning: only two pins of the marco will be routed!\n");
    }
    srcloc=&((*(marco->pins))->location);
    desloc=&((*(marco->pins+marco->pinnum-1))->location);
    srcnet=(*(marco->pins))->nets;
    desnet=(*(marco->pins+marco->pinnum-1))->nets;
    rcost=find_path_cost_vnets(srcloc,desloc,srcnet,desnet,bb_array);
    return rcost;
  }
}

/*
 *After a certain marco is decided to be routed in this round on 
 *the bread board, this subroutine will be used.
 *Notice the marco should be a two-port device.
 *The subroutine will rely on the structs t_locations to determine
 *the route location on bread board. Then try to spot a location on the
 *bread board and route both pins of the marco. And the update the 
 *route information of the bread board including status and cost.
 */
void
finish_route_marco_on_bb(IN t_pr_marco* marco,
                         IN t_bb_array* bb_array
                        )
{
  t_pr_pin* pina=*(marco->pins);
  t_pr_pin* pinb=*(marco->pins+marco->pinnum-1);
  t_location loca;
  t_location locb;
  float rcosta=try_route_marco_pin_on_bb(pina,pinb,loca,bb_array);
  float rcostb=try_route_marco_pin_on_bb(pinb,pina,locb,bb_array);
  if (rcosta>rcostb)
  {
    /*Remain the pin b. Revise the pin a*/
    pina->exloc=pina->location;
    *(pina->location)=locb;
    set_bb_net_unroutable(locb,bb_array);
    set_bb_node_unroutable(locb,bb_array);
    set_bb_node_occupied(locb,bb_array);
    set_bb_node_occupied(loca,bb_array);
  } 
  else
  {
    /*Remain the pin a. Revise the pin b*/
    pinb->exloc=pinb->location;
    *(pinb->location)=loca;
    set_bb_net_unroutable(loca,bb_array);
    set_bb_node_unroutable(loca,bb_array);
    set_bb_node_occupied(loca,bb_array);
    set_bb_node_occupied(locb,bb_array);
  }
  marco->rstatus=ROUTED;
  return 1;
}

/*
 *Update the route cost of each bb node on the bread board.
 */
void
update_bb_array_route_cost(INOUT t_bb_array* bb_array)
{
  int ix=0;
  int iy=0;
  for (ix=0;ix<bb_array->width;++ix)
  {
    for (iy=0;iy<bb_array->height;++iy)
    {
      alter_route_cost(&(bb_array->bb_node[ix][iy]));
    }
  }
  return 1;
}
