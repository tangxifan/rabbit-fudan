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
 *For each unit for placement, pin_location should be 
 *pointed out. When the unit is a specific node, the 
 *pin_location will be NONE.Assume the IC block is 
 *a rectangle of which pins are distibuted on each side. 
 */
enum e_pin_location
{NONE = -1, TOP = 0, LEFT = 1, RIGHT = 2; BOTTOM = 3};

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
typedef struct s_pin
{
  int index;
  enum e_pin_location pin_loc;
  int pin_offset;
}
t_pin;

/*
 *IC blocks is a comparatively universal struct
 *IC chips, MOSFETs, BJTs, Diodes and other
 *sort of devices could be defined as IC blocks.
 */
typedef struct s_icblock
{
  char *name;
  int dev_index;
  int length;
  int width;
  int area;
  int pin_num;
  /*For IC chips or MOSFET, BJT only*/
  t_pin *pin_list;//pin_list[0..num_list]
  /*For scalable devie such as resistance and capacitance*/
  int max_length;
  int min_length;
}
t_icblock;



