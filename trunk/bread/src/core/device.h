/*
 *The head file concentrate on device lib structs.
 *Author: Tang xifan
 */

/*
 *Universal announncement:
 *Use char *name to locate what the device is.
 *For example, a 74LS123 device will be named 
 *as follows,
 * *name='74ls123';
 *Index is used for quick matching device.
 */

/*
 *When the GEDA file has been read in, 
 *use the following struct to store netlist
 *information
 */
typedef struct s_net_info
{
  enum e_pin_location pin_loc;
  int pin_offset;
  char *dev_name;
}
t_net_info;

/*
 *The node will be convert to SPEC node when placing
 *To be awared, the node exclude the nodes of IC blocks
 */
typedef struct s_rb_node
{
  int index;
  int crit_num;
  t_net_info *net_list;
}
t_rb_node;

/*
 *Pins of certain devices need more detailed 
 *information, such as its location.
 *The following graph is useful in illustration.
 *               
 *                   top
 *              0  1  2  3  4
 *              |  |  |  |  |
 *            ----------------
 * left  0 -- |              | --0    right
 *       1 -- |              | --1
 *            ----------------
 *             |  |  |  |  |
 *             0  1  2  3  4
 *                bottom
 *Number 0..4 is the offset of pins' location.
 */
typedef struct s_dev_pin
{
  int index;
  enum e_pin_location loc;
  int offset;
}
t_dev_pin;

/*
 *IC blocks is a comparatively universal struct
 *IC chips, MOSFETs, BJTs, Diodes and other
 *sort of devices could be defined as IC blocks.
 */
typedef struct s_icdev
{
  char *name;
  int dev_index;
  int height;
  int width;
  int area;
  int pin_num;
  /*For IC chips or MOSFET, BJT only*/
  t_dev_pin *pinls//pinls[0..num_list]
  /*For scalable devie such as resistance and capacitance*/
  int max_length;
  int min_length;
}
t_icdev;



