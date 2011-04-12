/*
 *Author: Tang xifan
 */

/*******************New struct for place and route***********************/
enum e_pr_type
{RESISTOR,CAPACITANCE,ICBLOCK,VDD,GND,DIODE,OTHER};

enum e_pr_status
{PLACED,UNPLACED};

enum e_width_status
{COUNTED,UNCOUNT};

enum e_vnet_type
{SPECIAL,NORMAL};

enum e_pr_start
{STARTED,UNSTART};

enum e_pin_width_status
{POINTED,UNPOINT};

typedef struct s_vnet
{
  /*For place*/
  enum e_vnet_type type;
  enum e_pr_status status;
  enum e_width_status width_status;
  int numpin;
  t_pr_pin *pins;
  int placed_column;
  /*For struct*/
  t_vnet *next;  
}
t_vnet;

typedef struct s_pr_pin
{
  /*For Place*/
  int numnet;
  t_vnet *nets;
  enum e_pin_width_status wstatus;
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

typedef struct s_pr_marco
{
  /*For Place*/
  enum e_pr_type type;
  enum e_pr_status status;
  enum e_pr_start start;
  int pinnum;
  t_pin *pins;
  int block_width;
  /*For Route*/
  t_icdev *device;
  int loc_x;
  int loc_y;
  int placed_column;
  /*For struct*/
  t_pr_marco *next;
}
t_pr_marco;


