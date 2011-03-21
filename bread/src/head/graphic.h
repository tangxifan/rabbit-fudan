/*
 *The types defined below are used for constructing 
 *graphics when read input netlist.
 *Author: Tangxifan
 */
enum e_graph_type
{ MARCO,WIRE,VCC,GND,INPUT,OUTPUT};

typedef struct s_graph_node
{
  int pin_index;
  t_graph_marco *next;
}
t_graph_node;

typedef struct s_graph_marco
{
	enum e_graph_type marco_type;
	int pin_num;
    t_graph_node *pin_list; //pinlist[0..num_set]
}
t_graph_marco;

/*
 *These types below are used for breadboard structs.
 */
/*
 *We depend on enum bb_type to label different types 
 *of nodes on breadboard. 
 *BLANK is referred to those nodes which are unavailable 
 *for connection.
 *BIAS_NODE is those nodes only connected to bias voltage,
 *for instance, VCC and GND.
 *NORMAL_NODE represents ordinary nodes.
 */
enum e_bb_type
{ BLANK,BIAS_NODE,NORMAL_NODE};

/*
 *bb_node is the basic unit for breadboard structure.
 *Breadboard can be considered as an array of nodes
 *where some are unavailable for connection, some are 
 *only for VCC and GND connection, other are normal nodes
 *For each bb_node, the location should be given with
 *loc_x and loc_y.
 */
typedef struct s_bb_node
{
  int bb_index;
  enum e_bb_type bb_type;
  int  loc_x;
  int  loc_y;
  //If block_avail is true, this node could be occupied by
  //IC blocks. Or the node could be placed with a IC block.
  boolean block_avail;
  //These are only defined for black node.
  //Initial value is 1 which means the same size as normal
  //nodes.
  int  blank_width = 1;
  int  black_height = 1;
}
t_bb_node;

/*
 *A breadboard is a double-dimension array.
 */
/*typedef struct s_bb_array
 *{
 *	t_bb_node **bb_node; //bb_node[0..num_set]
 *}
 *t_bb_array;
 */


