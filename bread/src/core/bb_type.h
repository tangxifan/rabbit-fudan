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
 *When a breadboard(bb) node is set as occupied,
 *the node should not be placed any IC devices
 *during next steps in routing.
 */
enum e_bb_state
{FREE,OCCUPIED};
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
  //Inner list reveals what nodes are connected to current node
  //inside the breadboard.
  t_bb_node *innerlst; //inner_list[0..num_set]
  //If block_avail is true, this node could be occupied by
  //IC blocks. Or the node could be placed with a IC block.
  boolean block_avail;
  enum e_bb_state node_state;
  //We should label this bb node belong to which column
  int column;
}
t_bb_node;

/*
 *A breadboard is composed of several columns.
 */
typedef struct s_bb_column
{
  int width_capacity;
  int height;
  int width;
  int start_x;
  int start_y;
}
t_bb_column;

/*
 *A breadboard is a double-dimension array.
 */
typedef struct s_bb_array
{
  t_bb_node **bb_node; //bb_node[0..num_set]
  int width;
  int height;
  int no_column;
  t_bb_column* columns;
}
t_bb_array;
