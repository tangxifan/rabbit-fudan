#define BEND_COST 1.5
#define OCCP_COST 1
#define FLY_WIRE_COST  5

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
{BLANK,BIAS_NODE,NORMAL_NODE};

/*
 *When a breadboard(bb) node is set as occupied,
 *the node should not be placed any IC devices
 *during next steps in routing.
 */
enum e_bb_status
{FREE,OCCUPIED};

/*
 *When a bb node is set as UNROUTABLE, it may not be occupied.
 *But, when a bb node is set as OCCUPIED, it must be UNROUTABLE.
 *When a bb node is FREE but UNROUTABLE, it means the bb nodes
 *in the vertical line are routed with a virtual net.
 */
enum e_route_status
{ROUTABLE,UNROUTABLE};

enum e_bias_type
{NONE,VDD,GND};
/*
 *bb_node is the basic unit for breadboard structure.
 *Breadboard can be considered as an array of nodes
 *where some are unavailable for connection, some are 
 *only for VCC and GND connection, other are normal nodes
 *For each bb_node, the location should be given with
 *x and y.
 */
typedef struct s_bb_node
{
  int bb_index;
  enum e_bb_type type;
  /*We should label this bb node belong to which column.
   *If the node does not belong to any column.
   *The no. of column should be set as -1.
   */
  int column;
  t_location* location;
  /*Inner list reveals what nodes are connected to current node
   *inside the breadboard.
   */
  int ninner;
  t_bb_node **inners; //inner_list[0..num_set]
  /*Provide this bb node is routed by which pin of which virtual net*/
  t_pr_pin *pin;
  t_vnet *net;
  t_location* wired;
  enum e_bb_status status;
  enum e_route_status rstatus;
  enum e_bias_type bias_type;
  /*the route cost for this node.*/
  float rcost;
}
t_bb_node;

/*
 *A breadboard is composed of several columns.
 *base: The base location of the column on breadboard.
 *      With the determined height and width, we could 
 *      spot the column on the breadboard.
 *left: Left means the block or virtual net that has 
 *      placed on the left part of the column.
 *      left is an integer which in fact is the name
 *      of a certain t_pr_marco or t_vnet.
 *right:It is similar defined as the left.
 */
typedef struct s_bb_column
{
  t_location* base;
  int height;
  int width;
  int width_capacity;
  /*For place*/
  int usedwidth;
  int left;
  int right;
  /*For route*/
  /*Only one blank area is permitted in a column*/
  int blank_start;
  int blank_end;
}
t_bb_column;

/*
 *Height: The bias height should be 1.
 */
typedef struct s_bb_bias
{
  t_location* base;
  int height;
  int width;
  int former_column;
  int next_column;
  enum e_bias_type type;
}
t_bb_bias;

/*
 *A breadboard is a double-dimension array.
 *columns: A column is defined as the main part of
 *         the bread board for routing resources.
 *         Notice that the VDD and GND are exclude 
 *         from column.
 *reserve ratio: the ratio of bread board routing 
 *               resources reserved for special 
 *               virtual nets.
 */
typedef struct s_bb_array
{
  t_bb_node **bb_node; //bb_node[0..num_set]
  int width;
  int height;
  float reserve_ratio;
  int no_column;
  t_bb_column* columns;
  int nbias;
  t_bb_bias* biases;
}
t_bb_array;
