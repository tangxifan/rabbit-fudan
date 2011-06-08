/*
 *Functions open to other source file
 */
boolean
check_vnet(IN t_vnet* vnet);

int 
set_unplaced_marco(IN int nmarco,
                   IN t_pr_marco* marcos
                  );

int 
set_unstart_marco(IN int nmarco,
                  IN t_pr_marco* marcos
                  );

int 
set_unpoint_marco(IN t_pr_marco* marco);

int 
count_pointed_pins(IN t_pr_marco* marco);



