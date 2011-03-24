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
 *For each unit for placement, pin_location should be 
 *pointed out. When the unit is a specific node, the 
 *pin_location will be NONE.Assume the IC block is 
 *a rectangle of which pins are distibuted on each side. 
 */
enum e_pin_location
{NONE = -1, TOP = 0, LEFT = 1, RIGHT = 2; BOTTOM = 3};

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
}
t_place_node;
