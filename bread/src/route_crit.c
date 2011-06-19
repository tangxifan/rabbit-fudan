#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <route_crit.h>
#include <route_path.h>
#include <route_common.h>
#include <place_common.h>
#include <place_crit.h>
#include <place_util.h>

/*
 *After detail placement on bread board,
 *the exact place of the block has been 
 *determined. However, the detail placement
 *only give the exact x of location which leaves
 *the y undecided. So, in this step, the y will
 *be determined.
 *In the middle of each column, BLANK nodes take
 *place, thus, the start point of y should consider
 *height of block and the height of column.
 *When the height of block exceed the column height,
 *the program will report error.
 *Ensure the block in set in the middle of the column
 *and the block does not placed on a BLANK node.
 */
int
determine_block_base_on_bb(IN t_pr_marco* blk,
                           IN t_bb_array* bb_array
                          )
{
  int column=blk->pcolumn;
  int bbheight=bb_array->columns[column].height;
 
  blk->location.y=bb_array->columns[column].base.y
                 +(int)((bbheight-blk->device->height)/2+1);

  if (blk->location.y>bb_array->columns[column].blank_start)
  {blk->location.y=bb_array->columns[column].blank_start;}

  if (bbheight<blk->device->height)
  {
    printf("The block height overwhelms the height of breadboard column!\n");
    abort();
    exit(1);
  }
  printf("block[%d] route base is (%d,%d)\n",blk->name,blk->location.x,blk->location.y);
  return 1;
}

/*
 *After determining the base of block location,
 *we intend to spot the locations of each pins
 *attached the block.
 *The following subroutine complete the task with
 *several steps.
 *I.  Obtain the location information about each pin
 *    and deliver pin locations.
 *II. Set the bread board nodes route information to 
 *    confirm the routed pins on it.
 */
int
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
      curpin->location.x=blk->location.x+curpin->offset;
      curpin->location.y=blk->location.y-1;
    }
    if (BOTTOM==curpin->loc)
    {
      curpin->location.x=blk->location.x+curpin->offset;
      curpin->location.y=blk->location.y+blk->device->height;
    }
    if (LEFT==curpin->loc)
    {
      curpin->location.x=blk->location.x-1;
      curpin->location.y=blk->location.y+curpin->offset;
    }
    if (RIGHT==curpin->loc)
    {
      curpin->location.x=blk->location.x+blk->device->width+1;
      curpin->location.y=blk->location.y+curpin->offset;
    }
    /*Check whether the location of pin on bread board have been occupied*/
    if (bb_array->bb_node[curpin->location.x][curpin->location.y].status!=FREE)
    {
      printf("Error:Fail to put the block[%d] pin on breadboard!\nThe bread board node[%d][%d] is not free!\n",blk->name,curpin->location.x,curpin->location.y);
      abort();
      exit(1);
    }
    else
    {
      bb_array->bb_node[curpin->location.x][curpin->location.y].pin=curpin;
    //bb_array->bb_node[curpin->location.x][curpin->location.y].net=curpin->nets;
	  set_bb_node_net_with_pin_extra_loc(curpin,bb_array);
      set_bb_node_occupied(&(curpin->location),bb_array);
      set_bb_node_unroutable(&(curpin->location),bb_array);
      set_bb_net_unroutable(&(curpin->location),bb_array);
    }
    
  }
  blk->rstatus=ROUTED;
  return 1;
}

/*
 *Set all the bread board nodes that are covered 
 *by the block as occupied.
 */
int
determine_bb_node_covered_by_blk(IN t_pr_marco* blk,
                                 IN t_bb_array* bb_array
                                )
{
  t_location base;
  int ix=0;
  int iy=0;
  for (ix=blk->location.x;ix<blk->device->width;++ix)
  {
    for (iy=blk->location.y;iy<blk->device->height;++iy)
    {
      set_location_value(&base,ix,iy);
      set_bb_node_occupied(&base,bb_array);
      set_bb_node_unroutable(&base,bb_array);
    }
  }
  return 1;
}
 
/*
 *Try to route all blocks on the bread board.
 *There are a few tasks should be completed.
 *I.  Determine the exact location of the blocks
 *    on bread board.
 *II. Route each pin of the block on the bread board.
 *III.Set the bread board nodes covered by the block.
 */
int
route_blk_on_bb(IN t_pr_marco* blk,
                IN t_bb_array* bb_array
               )
{
  /*Find the block available nodes on breadboard*/
  determine_block_base_on_bb(blk,bb_array);
  /*Set the pins of given blk on the breadboard*/
  determine_pins_of_blk_on_bb(blk,bb_array);
  /*Set the bread board nodes covered by block*/
  determine_bb_node_covered_by_blk(blk,bb_array);
  blk->rstatus=ROUTED;
  return 1;
}

int
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
 *To initially route a special virtual net 
 *on the bread board.
 *Several steps need to be done.
 *I. Set the route status of those bb node 
 *   used by virtual net as UNROUTABLE.
 *   If the one of the node is UNROUTABLE,
 *   the error will be reported.
 *II.Set wire connections for the virtual net.
 */
int
route_special_vnet_on_bb(IN t_vnet* vnet,
                         IN t_bb_array* bb_array
                        )
{
  int column=vnet->pcolumn;
  int iloc=0;
  int iy=0;
  int ystart=bb_array->columns[column].base.y;
  int yend=bb_array->columns[column].base.y+bb_array->columns[column].height;
  
  int blankstart=0;
  int blankend=0;
  t_location loc;
  t_location des;

  for (iloc=0;iloc<vnet->locnum;++iloc)
  {
    /*Complete I.*/
    for (iy=ystart;iy<yend;++iy)
    {
      if (ROUTABLE==bb_array->bb_node[vnet->locations[iloc].x][iy].rstatus)
      {
        bb_array->bb_node[vnet->locations[iloc].x][iy].rstatus=UNROUTABLE;
        bb_array->bb_node[vnet->locations[iloc].x][iy].net=vnet;
      }
      else if (BLANK!=bb_array->bb_node[vnet->locations[iloc].x][iy].type) 
      {
        printf("Error: the bb node[%d][%d] is unroutable when routing virtual net on it!\n",vnet->locations[iloc].x,iy);
        abort();
        exit(1);
      }
    }
    /*Complete II.*/
    blankstart=bb_array->columns[column].blank_start;
    blankend=bb_array->columns[column].blank_end;
    set_location_value(&loc,vnet->locations[iloc].x,blankstart-1);
    set_location_value(&des,vnet->locations[iloc].x,blankend+1);

    set_bb_node_occupied(&loc,bb_array);
    set_bb_node_occupied(&des,bb_array);

    set_wired_on_bb(&loc,&des,bb_array);
    set_wired_on_bb(&des,&loc,bb_array);
    if (vnet->locnum>iloc+1)
    {
      if (OCCUPIED!=bb_array->bb_node[vnet->locations[iloc].x][blankstart-2].status)
      {
        set_location_value(&loc,vnet->locations[iloc].x,blankstart-2);
        set_location_value(&des,vnet->locations[iloc+1].x,blankstart-2);

        set_bb_node_occupied(&loc,bb_array);
        set_bb_node_occupied(&des,bb_array);

        set_wired_on_bb(&loc,&des,bb_array);
        set_wired_on_bb(&des,&loc,bb_array);
      }
      else if (OCCUPIED!=bb_array->bb_node[vnet->locations[iloc].x][blankend+2].status)
      {
        set_location_value(&loc,vnet->locations[iloc].x,blankend+2);
        set_location_value(&des,vnet->locations[iloc+1].x,blankend+2);

        set_bb_node_occupied(&loc,bb_array);
        set_bb_node_occupied(&des,bb_array);

        set_wired_on_bb(&loc,&des,bb_array);
        set_wired_on_bb(&des,&loc,bb_array);
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

/*
 *To initially route a normal virtual net
 *on the bread board.
 *Complete several tasks below.
 *I.  Set the bread board route information
 *    for the normal virtual net.
 *II. Set the connections of the virtual net.
 *III.Alloc additional space on bread board
 *    for the virtual net.
 */
int
route_normal_vnet_on_bb(IN t_vnet* vnet,
                        IN t_bb_array* bb_array
                       )
{
  int ipin=0;
  int counter=0;
  t_pr_pin* curpin=NULL;
  t_pr_pin* srcpin=NULL;
  t_pr_pin* despin=NULL;
  t_location srcloc;
  t_location desloc;
  int vnet_loc_num=0;

  //int westi=0;

  /*
   *Determine the number of locations for the normal virtual net.
   */
  for (ipin=0;ipin<vnet->numpin;++ipin)
  {
    if (ICBLOCK==vnet->pins[ipin]->parent->type)
    {vnet_loc_num++;}
  }	
  
  vnet->locnum=vnet_loc_num;
  vnet->locations=(t_location*)malloc(vnet->locnum*sizeof(t_location));
  /*Give the locations of the normal virtual net.*/
  vnet_loc_num=0;
  for (ipin=0;ipin<vnet->numpin;++ipin)
  {
    if (ICBLOCK==vnet->pins[ipin]->parent->type)
	{
	  set_location_value(&(vnet->locations[vnet_loc_num]),vnet->pins[ipin]->location.x,0);
	  vnet_loc_num++;
	}
  }

  /*Complete I. and II.*/
  for (ipin=0;ipin<vnet->numpin;++ipin)
  {
    curpin=vnet->pins[ipin];
    if (check_parent_type(curpin,ICBLOCK))
    {
      counter++;
      if (1==counter)
      {srcpin=curpin;}
      else if (2==counter)
      {
        despin=curpin;
        find_top_inner_on_bb(&(srcpin->location),bb_array,&srcloc);
        find_top_inner_on_bb(&(despin->location),bb_array,&desloc);
        desloc.y=srcloc.y;
        set_wired_on_bb(&srcloc,&desloc,bb_array);
        set_wired_on_bb(&desloc,&srcloc,bb_array);
      }
      else if (counter>2)
      {
        srcpin=despin;
        despin=curpin;
        find_top_inner_on_bb(&(srcpin->location),bb_array,&srcloc);
        find_top_inner_on_bb(&(despin->location),bb_array,&desloc);
        desloc.y=srcloc.y;
        set_wired_on_bb(&srcloc,&desloc,bb_array);
        set_wired_on_bb(&desloc,&srcloc,bb_array);
      }
      set_bb_net_unroutable(&(curpin->location),bb_array);
    }
  }
  /*Complete III.*/
  if (vnet->locnum > 1)
  {alloc_additional_space_for_normal_vnet(vnet,bb_array);}
  vnet->rstatus=ROUTED;  
  return 1;
}

int
alloc_additional_space_for_normal_vnet(IN t_vnet* vnet,
                                       IN t_bb_array* bb_array
                                      )
{
  int westi=vnet->locnum;
  int ipin=0;
  t_pr_pin* curpin=NULL;
  t_location loclft={0};
  t_location locrgt={0};
  t_location locchn={0};
  t_location loccur={0};
  t_location locsrc={0};
  t_location locdes={0};
  float lftcost=0.0;
  float rgtcost=0.0;
  float mincost=UNKNOWN;
  /*Check whether additional space is needed to alloc.*/
  while(westi>0)
  {
    /*Find the lowest cost for additional space.*/
    while(ipin<vnet->numpin)
    {
      curpin=*(vnet->pins+ipin);
      if (check_parent_type(curpin,ICBLOCK))
      {
        lftcost=try_left_find_node(curpin,&loclft,bb_array);
        rgtcost=try_right_find_node(curpin,&locrgt,bb_array);
        if (UNKNOWN==mincost)
        {
          if (lftcost<rgtcost)
          {mincost=lftcost;}
          else
          {mincost=rgtcost;}
        }        
        if (lftcost<mincost)
        {mincost=lftcost;locchn=loclft;loccur=(curpin->location);}
        if (rgtcost<mincost)
        {mincost=rgtcost;locchn=locrgt;loccur=(curpin->location);}
      }
      ipin++;
    } 
    /*Alloc the addtional space*/
    set_bb_node_occupied(&loccur,bb_array);
    set_bb_node_occupied(&locchn,bb_array);
    find_top_inner_on_bb(&loccur,bb_array,&locsrc);
    find_top_inner_on_bb(&locchn,bb_array,&locdes);
    locdes.y=locsrc.y;
    set_bb_node_unroutable(&loccur,bb_array);
    set_bb_node_unroutable(&locchn,bb_array);
    set_wired_on_bb(&locsrc,&locdes,bb_array);
    set_wired_on_bb(&locdes,&locsrc,bb_array);
    set_bb_node_occupied(&locsrc,bb_array);
    set_bb_node_occupied(&locdes,bb_array);
    westi--;
  }
  return 1;
}



int
set_route_vnets_on_bb(IN int nvnet,
                      IN t_vnet* vnets,
                      IN t_bb_array* bb_array
                      )
{
  int inet=0;
  for (inet=0;inet<nvnet;++inet)
  {
    if (SPECIAL==(vnets+inet)->type)
    {route_special_vnet_on_bb(vnets+inet,bb_array);}
    else if (NORMAL==(vnets+inet)->type)
    {route_normal_vnet_on_bb(vnets+inet,bb_array);}
    else
    {
      printf("Error: detect undefined type of virtual net!\n");
      abort();
      exit(1);
    }
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
    srcloc=&(marco->pins[0]->location);
    desloc=&(marco->pins[marco->pinnum-1]->location);
    srcnet=marco->pins[0]->nets;
    desnet=marco->pins[marco->pinnum-1]->nets;
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

int
finish_route_marco_on_bb(IN t_pr_marco* marco,
                         IN t_bb_array* bb_array
                        )
{
  t_pr_pin* pina=*(marco->pins);
  t_pr_pin* pinb=*(marco->pins+marco->pinnum-1);
  /*Additioanl locations(destination)*/
  t_location loca;
  t_location locb;
  t_location wloc;
  float rcosta=try_route_marco_pin_on_bb(pina,pinb,&loca,bb_array);
  float rcostb=try_route_marco_pin_on_bb(pinb,pina,&locb,bb_array);
  if (rcosta>rcostb)
  {
    /*Remain the pin b. Revise the pin a.
	 *This means, the pinb is just set as what it is.
	 *The pina has an extra location, so, some work on it should be done.
	 */
    pina->exloc=pina->location;
    pina->location=locb;
	bb_array->bb_node[pina->location.x][pina->location.y].pin=pina;
	bb_array->bb_node[pinb->location.x][pinb->location.y].pin=pinb;
    set_bb_node_occupied(&(pina->location),bb_array);
    set_bb_node_occupied(&(pinb->location),bb_array);

	/*When the given extra location does not belong to a virtual net,
	 *that is, extra location is a free virtual net. In this situation,
	 *the extra location could be set as an additional part for the virtual
	 *net, which could optimize the routing quality.
	 */
    if (FALSE==check_bb_node_unroutable(&locb,bb_array))
    {
      set_bb_net_unroutable(&locb,bb_array);
      set_bb_node_unroutable(&locb,bb_array);
      find_near_node_on_bb(&locb,bb_array,&wloc);
	  find_exloc_for_wire(&(pina->exloc),&wloc,bb_array);
      set_wired_on_bb(&wloc,&(pina->exloc),bb_array);
      set_wired_on_bb(&(pina->exloc),&wloc,bb_array);
      set_bb_node_occupied(&(pina->exloc),bb_array);
      set_bb_node_occupied(&wloc,bb_array);
	  set_bb_node_net_with_pin_extra_loc(pina,bb_array);
    }
  } 
  else
  {
    /*Remain the pin a. Revise the pin b*/
    pinb->exloc=pinb->location;
    pinb->location=loca;
	bb_array->bb_node[pina->location.x][pina->location.y].pin=pina;
	bb_array->bb_node[pinb->location.x][pinb->location.y].pin=pinb;
    set_bb_node_occupied(&(pina->location),bb_array);
    set_bb_node_occupied(&(pinb->location),bb_array);
    if (FALSE==check_bb_node_unroutable(&loca,bb_array))
    {
      set_bb_net_unroutable(&loca,bb_array);
      set_bb_node_unroutable(&loca,bb_array);
      find_near_node_on_bb(&loca,bb_array,&wloc);
	  find_exloc_for_wire(&(pinb->exloc),&wloc,bb_array);
      set_wired_on_bb(&wloc,&(pinb->exloc),bb_array);
      set_wired_on_bb(&(pinb->exloc),&wloc,bb_array);
      set_bb_node_occupied(&(pinb->exloc),bb_array);
      set_bb_node_occupied(&wloc,bb_array);
	  set_bb_node_net_with_pin_extra_loc(pinb,bb_array);
    }
  }
  marco->rstatus=ROUTED;
  return 1;
}





/*
 *Update the route cost of each bb node on the bread board.
 */
int
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
