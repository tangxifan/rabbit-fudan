/*
 *Author: Tang xifan
 */

/*******************New struct for place and route***********************/
/*
 *There are four sorts in the type.
 *RCD:resistor, capacitance and diode. These are
 *    all two-port device.
 *ICBLOCK: Multi-port device are catagoried as ICBLOCK
 *         which will be heavily considered in placement.
 */


enum e_pr_type
{NONE,RCD,ICBLOCK,VDD,GND};

/*
 *This status flag is used during placemnt
 *to mark this block has been placed.
 */
enum e_pr_status
{PLACED,UNPLACED};

/*
 *This enum is critical in distinguish virtual nets.
 *SPECIAL Net is those has none direct connections with
 *IC blocks.
 */
enum e_vnet_type
{SPECIAL,NORMAL};

/*
 *The start status is used in choosing starter block.
 *If all the block has been labelled as STARTED while
 *none suitable block has been chosen. The placement 
 *will be aborted.
 */
enum e_pr_start
{STARTED,UNSTART};

enum e_route_status
{ROUTED,UNROUTE};

/*
 *In calculating block place width from indirect 
 *attractions, this status is used. In some cases,
 *several pins on the source block are connected 
 *to the same pin on destination block. With this 
 *status, the repeating calculation on indirect 
 *connections could be avoid so that the block place
 *width would not be unrealistic large.  
 */
enum e_pwidth_status
{POINTED,UNPOINT};

struct s_vnet
{
  int name;
  /*General*/
  enum e_vnet_type type;
  int numpin;
  t_pr_pin **pins;
  /*For place*/
  enum e_pr_status status;
  enum e_pr_start sstart;
  enum e_pwidth_status spwidth;
  float pcost;
  int pcolumn;
  int pwidth;
  /*For route*/
  int locnum;
  t_location* locations;
  int bbnum;
  t_bb_node **bb_nodes;
  enum e_route_status rstatus;
  /*For struct*/
  struct s_pr_node *next;  
};

struct s_pr_pin
{
  /*For Place*/
  int numnet;
  t_vnet *nets;
  /*For Route*/
  enum e_pin_location loc;
  int offset;
  int pin_no;
  t_location location;
  /*For route*/
  t_location exloc;
  /*For struct*/
  t_pr_marco *parent;
  t_pr_pin *next;
};

/*
 *detail_offset: the offset of a IC BLOCK in its
 *               place area.(count from left edge)
 */
struct s_pr_marco
{
  /*General*/
  int name;
  t_icdev *device;
  enum e_pr_type type;
  int pinnum;
  t_pr_pin **pins;
  /*For Place*/
  float pcost;
  int pwidth;
  enum e_pr_status status;
  enum e_pr_start sstart;
  /*For Route*/
  int priority;
  int detail_offset;
  t_location location;
  int pcolumn;
  enum e_route_status rstatus;
  /*For struct*/
  struct s_pr_node *next;
  /* zxhmike: to store its name */
  char* label;
};

/*
 *If the flag is TRUE, the pr node store a marco
 *and the vnet pointer is NULL.
 *If the flag is FALSE, the pr node store a vnet
 *and the marco pointer is NULL.
 */
struct s_pr_node
{
  boolean flag;
  t_pr_marco* mnext;
  t_vnet* vnext;
};

/*
 *The place information struct is used during
 *placement for storing placement information.
 */
struct s_place_info
{
  int bb_pwidth;
  int cur_width;
  int column;
  t_pr_node* left;
  t_pr_node* right;
};
