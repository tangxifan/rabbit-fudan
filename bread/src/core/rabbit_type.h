/*
 *This file includes most of types defined in RABBIT
 *Author: Tangxifan 
 */

/*
 *The following definition are used to annotate the
 *input, output or inout parameters for functions.
 *For example,
 *return_type 
 *function_name ( IN    parameterA,
 *               OUT   parameterB,
 *               INOUT parameterC,
 *			     );
 */
#define IN 
#define OUT
#define INOUT

#define TRUE 1
#define FALSE 0

/*
 *DEBUG is defined for lauching debug mode.
 */
#define DEBUG 1

/*
 *Set a co-efficient for the flexible usage of bread board
 *In most cases, we won't fully utlize the routing resources
 *of the given bread board. Thus it is reasonable to leave some 
 *resource available to cope with unexpected problems.
 */
#define FLEX_RATIO 0.2

#define UNKNOWN -1

#define MAX_FILENAME_LENGTH 500
#define BUF_SIZE 500

/*
 *For each unit for placement, pin_location should be 
 *pointed out. When the unit is a specific node, the 
 *pin_location will be NONE.Assume the IC block is 
 *a rectangle of which pins are distibuted on each side. 
 */
enum e_pin_location
{NONE = -1, TOP = 0, LEFT = 1, RIGHT = 2, BOTTOM = 3};

typedef struct s_location
{
  int name;
  int x;
  int y;
}
t_location;












