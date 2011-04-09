/*
 *Author: Tang xifan
 */
/*
 *For routing purpose, specified type of each marco should 
 *be detailed defined.
 */
enum e_route_type
{ ICBLOCK = -1,METALWIRE = 0,CAPACITANCE = 1,RESISTOR = 2, VCC = 3, GND = 4};

typedef struct s_route_pin
{
  int pin_index;
  t_route_pin *next_pin;
  t_route_node *next_node;
  t_bb_node *bb_node
}
t_route_pin;

typedef struct s_route_node;
{
  int route_index;
  enum e_route_type route_type;
  t_icdev *ptr_dev;
  char *name;
  int x_loc;
  int y_loc;
  int width;
  int height;
  int area;
  t_bb_node *bb_node_list;//bb_node_list[0..num_set]
  t_route_pin *pin_list; //pin_list[0..num_set]
}
t_route_node;


/*******************New struct for place and route***********************/
enum e_pr_type
{RESISTOR,CAPACITANCE,ICBOLCK,VDD,GND,DIODE,OTHER};

typedef struct s_vnet
{
  /*For place*/
  int numpin;
  t_pr_pin *pins;
  /*For struct*/
  t_vnet *next;  
}
t_vnet;

typedef struct s_pr_pin
{
  /*For Place*/
  int numnet;
  t_vnet *nets;
  /*For Route*/
  enum e_pin_location loc;
  int offset;
  /*For struct*/
  t_pr_marco *parent;
  t_pr_pin *next;
}
t_pr_pin;

typedef struct s_pr_marco
{
  /*For Place*/
  enum e_pr_type type;
  int pinnum;
  t_pin *pins;
  /*For Route*/
  t_icdev *device;
  int loc_x;
  int lox_y;
  /*For struct*/
  t_pr_marco *next;
}
t_pr_marco;


