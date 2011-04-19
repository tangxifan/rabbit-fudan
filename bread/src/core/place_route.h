/*
 *Author: Tang xifan
 */

/*******************New struct for place and route***********************/
enum e_pr_type
{RESISTOR,CAPACITANCE,ICBLOCK,VDD,GND,DIODE,OTHER};

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

typedef struct s_vnet
{
  int name;
  /*General*/
  enum e_vnet_type type;
  int numpin;
  t_pr_pin *pins;
  /*For place*/
  enum e_pr_status status;
  enum e_pr_start sstart;
  enum e_pwidth_status spwidth;
  float pcost;
  int pcolumn;
  int pwidth;
  int loc_x;
  int loc_y;
  /*For struct*/
  t_pr_node next;  
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
  int loc_x;
  int loc_y;
  /*For struct*/
  t_pr_marco *parent;
  t_pr_pin *next;
}
t_pr_pin;

/*
 *detail_offset: the offset of a IC BLOCK in its
 *               place area.(count from left edge)
 */
typedef struct s_pr_marco
{
  /*General*/
  int name;
  t_icdev *device;
  enum e_pr_type type;
  int pinnum;
  t_pin *pins;
  /*For Place*/
  float pcost;
  int pwidth;
  enum e_pr_status status;
  enum e_pr_start sstart;
  /*For Route*/
  int priority;
  int detail_offset;
  int loc_x;
  int loc_y;
  int pcolumn;
  /*For struct*/
  t_pr_node next;
}
t_pr_marco;

typedef struct s_pr_node
{
  boolean flag;
  t_pr_marco* mnext;
  t_vnet* vnext;
}
t_pr_node;

typedef struct s_place_info
{
  int bb_pwidth;
  int cur_width;
  int column;
  s_pr_node left;
  s_pr_node right;
}
t_place_info;
