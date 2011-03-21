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
  int width;
  int height;
  int area;
  t_bb_node *bb_node_list;//bb_node_list[0..num_set]
  t_route_pin *pin_list; //pin_list[0..num_set]
}
t_route_node;
