#include <stdio.h>
#include <stdlib.h>

#include "rabbit_type.h"
#include "bb_type.h"

/*Global variant*/
t_bb_array bb_array;

/*Local Bread Board Info*/
/*Column Arch*/
int col_num=3;
int col_wcapacity=5;
int col_blank_height=2;
int col_width=64;
int col_height=2*col_wcapacity+col_blank_height;

/*Bias Arch*/
int bias_width=64;
int bias_height=2;
int bias_blank=2;
int bias_num=2*(col_num+1);

/*Malloc the columns*/
int 
create_columns(INOUT t_bb_array* bb_array)
{
  int icol=0;
  /*Determine colnumn number*/
  bb_array->no_column=col_num;
  /*Do malloc work*/
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
  (*curcol->base)=(*curcol);
  curcol->height=col_height;
  curcol->width=col_width;
  curcol->width_capacity=col_wcapacity;
  /*Give placement information*/
  curcol->usedwidth=0;
  curcol->left=UNKNOWN;
  curcol->right=UNKNOWN;
  /*Blank Information*/
  curcol->blank_start=curcol->base->y+curcol->width_capacity;
  curcol->blank_end=curcol->blank_start+col_blank_height-1;
  return 1;
}

int 
initial_bias(t_bb_bias* curbias,
             t_location* curbase
            )
{
  /*Give general arch information.*/
  (*curbias->base)=(*curcol);
  curbias->height=1;
  curbias->width=bias_width;
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
creat_bb_nodes(t_bb_array* bb_array)
{
  int ibb=0;
  /*Malloc height*/
  *(bb_array->bb_node)=(t_bb_node*)malloc(bb_array->height*sizeof(t_bb_node*));
  if (NULL==*(bb_array->bb_node))
  {
    printf("Fail to malloc the height for bread board architecture!\n");
    abort();
    exit(1);
  }
  /*Malloc width*/
  for (ibb=0;ibb<bb_array->height;++ibb)
  {
    bb_array->bb_node[ibb]=(t_bb_node*)malloc(bb_array->width*sizeof(t_bb_node));
    if (NULL==bb_array->bb_node[ibb])
    {
      printf("Fail to malloc the width for bread board architecture!\n");
      abort();
      exit(1);
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


  create_columns(bb_array);
  create_biases(bb_array);
  /*
   *Initialize the width and height of breadboard and
   *the column information.   
   */
  /*Do Bias initialize work*/
  set_location_value(&curbase,0,total_height);
  initial_bias(bb_array->biases+ibias,&curbase);
  ibias++;
  set_location_value(&curbase,0,total_height+1);
  initial_bias(bb_array->biases+ibias,&curbase);
  ibias++;
  /*End bias initialization*/ 
 
  total_width=col_width;
  total_height+=bias_height;
  for (icol=0;icol<bb_array->no_column;++icol)
  {
    total_height+=bias_blank;    
    /*Do column initialize work*/
    set_location_value(&curbase,0,total_height);
    initial_column(bb_array->columns+icol,&curbase);
    /*End column initialization*/
    total_height+=bb_array->columns[icol].height;
    total_height+=bias_blank;
    /*Do Bias initialize work*/
    set_location_value(&curbase,0,total_height);
    initial_bias(bb_array->biases+ibias,&curbase);
    ibias++;
    set_location_value(&curbase,0,total_height+1);
    initial_bias(bb_array->biases+ibias,&curbase);
    ibias++;
    /*End bias initialization*/ 
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
    given_type=BIAS_VDD;
    set_bias_type(bb_array->biases+ibias,given_type);
    ++ibias;
  }
  /*End bias initialization*/   

  /*Create all bread board nodes*/
  creat_bb_nodes(bb_array);    
  


  return 1;
}

