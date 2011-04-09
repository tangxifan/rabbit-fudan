/*
 *There are some types only for placement.
 *Author: Tang xifan
 */
/*
 *Placement proccess the reasonable place of IC blocks
 *and nodes which do not belong to IC blocks.
 */
enum e_place_type
{ICBLOCK, SPEC_NODE};

/*
 *For each pin on unit, attribution should be specified.
 *And the index of pin should also be labelled.
 *And the pointer of pin connected should be given as well.
 */
typedef struct s_place_pin
{
	enum e_pin_location attri_loc;
	int pin_index;
	t_place_pin *next_pin;
	t_place_node *next_node;
}
t_place_pin;

typedef struct s_place_node
{
  int place_index;
  enum e_place_type place_type;
  t_place_pin *pin_list; //pin[0..num_set]
  //Detailed information for IC blocks.
  int width;
  int height;
  int x_loc;
  int y_loc;
}
t_place_node;

/*
 *For routing purpose, specified type of each marco should 
 *be detailed defined.
 */
enum e_route_type
{ ICBLOCK = -1,METALWIRE = 0,CAPACITANCE = 1,RESISTOR = 2, VCC = 3, GND = 4};

typedef struct s_route_pin
{
  int pin_index;
  s_route_pin *next_pin;
  s_route_node *next_node;
}
t_route_pin;

typedef struct s_route_node;
{
  int route_index;
  enum e_route_type route_type;
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