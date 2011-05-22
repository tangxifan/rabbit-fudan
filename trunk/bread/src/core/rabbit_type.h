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

typedef enum e_boolean{
	FALSE = 0,
	TRUE
}boolean;

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

#define RESISTOR_MAX_LENGTH 10
#define RESISTOR_MIN_LENGTH 2
#define CAPASITOR_MAX_LENGTH 5
#define CAPASITOR_MIN_LENGTH 2

/*
 *For each unit for placement, pin_location should be 
 *pointed out. When the unit is a specific node, the 
 *pin_location will be NONE.Assume the IC block is 
 *a rectangle of which pins are distibuted on each side. 
 */
enum e_pin_location
{TOP = 0, LEFT = 1, RIGHT = 2, BOTTOM = 3};

/* prototypes of structures*/
typedef struct s_vnet t_vnet;
typedef struct s_pr_pin t_pr_pin;
typedef struct s_pr_marco t_pr_marco;
typedef struct s_pr_node t_pr_node;
typedef struct s_place_info t_place_info;
typedef struct s_location t_location;
typedef struct s_bb_node t_bb_node;
typedef struct s_bb_column t_bb_column;
typedef struct s_bb_bias t_bb_bias;
typedef struct s_bb_array t_bb_array;
typedef struct s_net_info t_net_info;
typedef struct s_rb_node t_rb_node;
typedef struct s_dev_pin t_dev_pin;
typedef struct s_icdev t_icdev;

struct s_location
{
  int name;
  int x;
  int y;
};
