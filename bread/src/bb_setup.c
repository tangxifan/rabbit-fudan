#include <stdio.h>
#include <stdlib.h>

#include "rabbit_type.h"
#include "bb_type.h"
#include "bb_setup.h"

/*Global variant*/
t_bb_array bb_array;

/*Local Bread Board Info*/
/*Column Arch*/
int col_num=3;
int col_wcapacity=5;
int col_blank_height=2;
int col_width=64;
int col_height=2*5/*col_wcapacity*/+2/*col_blank_height*/;

/*Bias Arch*/
int bias_offset=2;
int bias_width=60;
int bias_height=2;
int bias_blank=2;
int bias_num=2*(3/*col_num*/+1);
int bias_wcapacity=5;

/*Route Cost*/
float initial_route_cost=1.0;

/************Subroutines***************/
int 
create_columns(INOUT t_bb_array* bb_array);

int 
create_biases(INOUT t_bb_array* bb_array);

int 
initial_column(t_bb_column* curcol,
               t_location* curbase
              );

int 
initial_bias(t_bb_bias* curbias,
             t_location* curbase
            );

int 
set_bias_type(t_bb_bias* curbias,
              enum e_bias_type given_type
             );

int 
create_bb_nodes(t_bb_array* bb_array);

int 
initial_bb_bodes(INOUT t_bb_array* bb_array);

int 
initial_single_bb_node(INOUT t_bb_array* bb_array,
                       IN int x,
                       IN int y
                       );

int 
initial_column_nodes(INOUT t_bb_array* bb_array);

int 
initial_bias_nodes(INOUT t_bb_array* bb_array);

int 
initial_bias_inners(INOUT t_bb_array* bb_array,
                    IN int x,
                    IN int y
                    );

int 
initial_normal_inners(INOUT t_bb_array* bb_array,
                      IN int x,
                      IN int y
                     );

int 
initial_inners(INOUT t_bb_array* bb_array);
/*************************************/

/*Malloc the columns*/
int 
create_columns(INOUT t_bb_array* bb_array)
{
  int icol=0;
  /*Determine colnumn number*/
  bb_array->no_column=col_num;
  /*Do mall(oc work*/
  bb_array->columns=(t_bb_column*)malloc(bb_array->no_column*sizeof(t_bb_column));
  if (NULL==bb_array->columns)
  {
    printf("Fail to malloc the columns for bread board architecture!\n");
    abort();
    exit(1);
  }
  return 1;
}

int 
create_biases(INOUT t_bb_array* bb_array)
{
  /*Determine bias number*/
  bb_array->nbias=bias_num;
  /*Do malloc work*/
  bb_array->biases=(t_bb_bias*)malloc(bb_array->nbias*sizeof(t_bb_bias));
  if (NULL==bb_array->biases)
  {
    printf("Fail to malloc the biases for bread board architecture!\n");
    abort();
    exit(1);
  }
  return 1;
}


/*
 *Initialize the columns of bread board.
 *Give initial values.
 */
int 
initial_column(t_bb_column* curcol,
               t_location* curbase
              )
{
  /*Give general arch information.*/
  curcol->base=(*curbase);
  curcol->height=col_height;
  curcol->width=col_width;
  curcol->width_capacity=col_wcapacity;
  /*Give placement information*/
  curcol->usedwidth=0;
  curcol->left=UNKNOWN;
  curcol->right=UNKNOWN;
  /*Blank Information*/
  curcol->blank_start=curcol->base.y+curcol->width_capacity;
  curcol->blank_end=curcol->blank_start+col_blank_height-1;
  return 1;
}

int 
initial_bias(t_bb_bias* curbias,
             t_location* curbase
            )
{
  /*Give general arch information.*/
  curbias->base=(*curbase);
  curbias->height=1;
  curbias->width=bias_width;
  curbias->width_capacity=bias_wcapacity;
  curbias->type=BIAS_NONE;
  return 1;
}

int 
set_bias_type(t_bb_bias* curbias,
              enum e_bias_type given_type
             )
{
  curbias->type=given_type;
  return 1;
}

int 
create_bb_nodes(t_bb_array* bb_array)
{
  int ibb=0;
  /*Malloc width*/
  bb_array->bb_node=(t_bb_node**)malloc(bb_array->width*sizeof(t_bb_node*));
  if (NULL==(bb_array->bb_node))
  {
    printf("Fail to malloc the width  for bread board architecture!\n");
    abort();
    exit(1);
  }
  /*Malloc height*/
  for (ibb=0;ibb<bb_array->width;++ibb)
  {
    bb_array->bb_node[ibb]=(t_bb_node*)malloc(bb_array->height*sizeof(t_bb_node));
    if (NULL==bb_array->bb_node[ibb])
    {
      printf("Fail to malloc the height for bread board architecture!\n");
      abort();
      exit(1);
    }    
  }

  return 1;
}

int 
initial_bb_bodes(INOUT t_bb_array* bb_array)
{
  int ix=0;
  int iy=0;
  
  for (ix=0;ix<bb_array->width;++ix)
  {
    for (iy=0;iy<bb_array->height;++iy)
    {
      initial_single_bb_node(bb_array,ix,iy);
    }
  }
  return 1;
}

int 
initial_single_bb_node(INOUT t_bb_array* bb_array,
                       IN int x,
                       IN int y
                       )
{
  bb_array->bb_node[x][y].bb_index=UNKNOWN;
  bb_array->bb_node[x][y].type=BLANK;
  bb_array->bb_node[x][y].column=UNKNOWN;
  bb_array->bb_node[x][y].ninner=UNKNOWN;
  bb_array->bb_node[x][y].inners=NULL;
  bb_array->bb_node[x][y].pin=NULL;
  bb_array->bb_node[x][y].net=NULL;
  //bb_array->bb_node[x][y].wired=NULL;
  bb_array->bb_node[x][y].status=FREE;
  bb_array->bb_node[x][y].rstatus=ROUTABLE;
  bb_array->bb_node[x][y].bias_type=BIAS_NONE;
  bb_array->bb_node[x][y].rcost=initial_route_cost;
  set_location_value(&(bb_array->bb_node[x][y].location),x,y);
  return 1;
}


int 
initial_column_nodes(INOUT t_bb_array* bb_array)
{
  int icol=0;
  int ix=0;
  int iy=0;
  int topx=0;
  int topy=0;

  for (icol=0;icol<bb_array->no_column;++icol)
  {
    topx=bb_array->columns[icol].base.x+bb_array->columns[icol].width;
    topy=bb_array->columns[icol].base.y+bb_array->columns[icol].height;
    for (ix=bb_array->columns[icol].base.x;ix<topx;++ix)
    {
      for (iy=bb_array->columns[icol].base.y;iy<topy;++iy)
      {
        if ((iy<bb_array->columns[icol].blank_start)||(iy>bb_array->columns[icol].blank_end))
        {bb_array->bb_node[ix][iy].type=NORMAL_NODE;}
        bb_array->bb_node[ix][iy].column=icol;
      }
    }
  }  
  return 1;
}

/*Set all bias nodes. Give them identity*/
int 
initial_bias_nodes(INOUT t_bb_array* bb_array)
{
  int ibias=0;
  int ix=0;
  int y=0;
  int iwcap=0;
  int topx=0;
  int topy=0;
  
  /*Aware the blank nodes!*/
  for (ibias=0;ibias<bb_array->nbias;++ibias)
  {
    y=bb_array->biases[ibias].base.y;
	topx=bb_array->biases[ibias].base.x+bb_array->biases[ibias].width;
    for (ix=bb_array->biases[ibias].base.x;ix<topx;)
    {
      if (ix<(int)bb_array->width/2)
      {
        for (iwcap=0;iwcap<bb_array->biases[ibias].width_capacity;++iwcap)
        {
          bb_array->bb_node[ix][y].type=BIAS_NODE;
          bb_array->bb_node[ix][y].bias_type=bb_array->biases[ibias].type;
          bb_array->bb_node[ix][y].bias=ibias;
          ix++;
          if (FALSE==(ix<topx))
          {return 1;}
        }
        ix++;
        if (FALSE==(ix<topx))
        {return 1;}
      }
      else
      {
        ix++;
        if (FALSE==(ix<topx))
        {return 1;}
        for (iwcap=0;iwcap<bb_array->biases[ibias].width_capacity;++iwcap)
        {
          bb_array->bb_node[ix][y].type=BIAS_NODE;
          bb_array->bb_node[ix][y].bias_type=bb_array->biases[ibias].type;
          bb_array->bb_node[ix][y].bias=ibias;
          ix++;
          if (FALSE==(ix<topx))
          {return 1;}
        }
      }
    }
  }
  return 1;
}

int 
initial_bias_inners(INOUT t_bb_array* bb_array,
                    IN int x,
                    IN int y
                    )
{
  int ibias=bb_array->bb_node[x][y].bias;
  int tmpx=0;
  int tmpy=0;
  int iin=0;

  bb_array->bb_node[x][y].ninner=bb_array->biases[ibias].width_capacity-1;
  bb_array->bb_node[x][y].inners=(t_bb_node**)malloc(bb_array->bb_node[x][y].ninner*sizeof(t_bb_node*));
  if (NULL==bb_array->bb_node[x][y].inners)
  {
    printf("Fail to malloc the inners for the node: (x,y) (%d,%d).\n",x,y);
    abort();
    exit(1);
  }
  /*Try find similar nodes on the left*/
  tmpx=x;
  tmpy=y;
  while(iin<bb_array->bb_node[x][y].ninner)
  {
    tmpx--;
    if ((tmpx>(bb_array->width-1))||(tmpx<0))
	{break;}
	if (bb_array->bb_node[tmpx][tmpy].type!=bb_array->bb_node[x][y].type)
    {break;}
    else
    {
      bb_array->bb_node[x][y].inners[iin]=&(bb_array->bb_node[tmpx][tmpy]);
      iin++;
    }
  }
  /*Try find similar nodes on the right*/
  tmpx=x;
  tmpy=y;
  while(iin<bb_array->bb_node[x][y].ninner)
  {
    tmpx++;
    if ((tmpx>(bb_array->width-1))||(tmpx<0))
	{break;}
    if (bb_array->bb_node[tmpx][tmpy].type!=bb_array->bb_node[x][y].type)
    {break;}
    else
    {
      bb_array->bb_node[x][y].inners[iin]=&(bb_array->bb_node[tmpx][tmpy]);
      iin++;
    }
  }

  return 1;
}

int 
initial_normal_inners(INOUT t_bb_array* bb_array,
                      IN int x,
                      IN int y
                     )
{
  int icol=bb_array->bb_node[x][y].column;
  int tmpx=0;
  int tmpy=0;
  int iin=0;

  bb_array->bb_node[x][y].ninner=bb_array->biases[icol].width_capacity-1;
  bb_array->bb_node[x][y].inners=(t_bb_node**)malloc(bb_array->bb_node[x][y].ninner*sizeof(t_bb_node*));
  if (NULL==bb_array->bb_node[x][y].inners)
  {
    printf("Fail to malloc the inners for the node: (x,y) (%d,%d).\n",x,y);
    abort();
    exit(1);
  }
  /*Try find similar nodes on the top*/
  tmpx=x;
  tmpy=y;
  while(iin<bb_array->bb_node[x][y].ninner)
  {
    tmpy--;
    if ((tmpy>(bb_array->height-1))||(tmpx<0))
	{break;}
    if (bb_array->bb_node[tmpx][tmpy].type!=bb_array->bb_node[x][y].type)
    {break;}
    else
    {
      bb_array->bb_node[x][y].inners[iin]=&(bb_array->bb_node[tmpx][tmpy]);
      iin++;
    }
  }
  /*Try find similar nodes on the bottom*/
  tmpx=x;
  tmpy=y;
  while(iin<bb_array->bb_node[x][y].ninner)
  {
    tmpy++;
    if ((tmpy>(bb_array->height-1))||(tmpx<0))
	{break;}
    if (bb_array->bb_node[tmpx][tmpy].type!=bb_array->bb_node[x][y].type)
    {break;}
    else
    {
      bb_array->bb_node[x][y].inners[iin]=&(bb_array->bb_node[tmpx][tmpy]);
      iin++;
    }
  }
  
  return 1;
}


int 
initial_inners(INOUT t_bb_array* bb_array)
{
  int ix=0;
  int iy=0;
  
  for (ix=0;ix<bb_array->width;++ix)
  {
    for (iy=0;iy<bb_array->height;++iy)
    {
      if (BIAS_NODE==bb_array->bb_node[ix][iy].type)
      {initial_bias_inners(bb_array,ix,iy);}
      else if (NORMAL_NODE==bb_array->bb_node[ix][iy].type)
      {initial_normal_inners(bb_array,ix,iy);}
    }
  }
  return 1;
}

int 
setup_breadboard(t_bb_array* bb_array)
{
  t_location curbase={0};
  int total_height=0;
  int total_width=0;

  int icol=0;
  int ibias=0;  
  enum e_bias_type given_type;
  
  bb_array->reserve_ratio=0.0;
  /*Create Columns and Biases.*/
  create_columns(bb_array);
  printf("Create the Columns for bread board...\n");

  create_biases(bb_array);
  printf("Create the biases for bread board...\n");
  /*
   *Initialize the width and height of breadboard and
   *the column information.   
   */
  /*Do Bias initialize work*/
  set_location_value(&curbase,bias_offset,total_height);
  initial_bias(bb_array->biases+ibias,&curbase);
  ibias++;
  printf("Initial the biases(No.%d) for bread board...\n",ibias);
  set_location_value(&curbase,bias_offset,total_height+1);
  initial_bias(bb_array->biases+ibias,&curbase);
  ibias++;
  printf("Initial the biases(No.%d) for bread board...\n",ibias);
 /*End bias initialization*/ 

  total_width=col_width;
  total_height+=bias_height;
  for (icol=0;icol<bb_array->no_column;++icol)
  {
    total_height+=bias_blank;    
    /*Do column initialize work*/
    set_location_value(&curbase,0,total_height);
    initial_column(bb_array->columns+icol,&curbase);
    printf("Initial the column(No.%d) for bread board...\n",icol);
    /*End column initialization*/
    total_height+=bb_array->columns[icol].height;
    total_height+=bias_blank;
    /*Do Bias initialize work*/
    set_location_value(&curbase,bias_offset,total_height);
    initial_bias(bb_array->biases+ibias,&curbase);
    ibias++;
    printf("Initial the biases(No.%d) for bread board...\n",ibias);
    set_location_value(&curbase,bias_offset,total_height+1);
    initial_bias(bb_array->biases+ibias,&curbase);
    ibias++;
    /*End bias initialization*/ 
    printf("Initial the biases(No.%d) for bread board...\n",ibias);
    total_height+=bias_height;
  }
  bb_array->width=total_width;
  bb_array->height=total_height;
  
  /*Do Bias initialize work*/
  for (ibias=0;ibias<bb_array->nbias;)
  {
    given_type=BIAS_GND;
    set_bias_type(bb_array->biases+ibias,given_type);
    ++ibias;
    printf("Set Bias(no.%d) types...\n",ibias);
    given_type=BIAS_VDD;
    set_bias_type(bb_array->biases+ibias,given_type);
    ++ibias;
    printf("Set Bias(no.%d) types...\n",ibias);
  }
  /*End bias initialization*/   
  /*Create all bread board nodes*/
  create_bb_nodes(bb_array);    
  printf("Create bread board nodes...\n");
  /*Initial all bread board nodes*/  
  initial_bb_bodes(bb_array);
  printf("Initial bread board nodes...\n");
  /*Initial column bread board nodes*/
  initial_column_nodes(bb_array);
  printf("Initial column nodes...\n");
  /*Initial bias bread board nodes*/
  initial_bias_nodes(bb_array);
  printf("Initial bias nodes...\n");
  /*Initial the inner connections*/
  initial_inners(bb_array);
  printf("Initial inner nodes...\n");

  return 1;
}

 


