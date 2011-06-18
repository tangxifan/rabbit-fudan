
t_pr_marco** 
create_icblk_array(INOUT int* nblk,
                   IN int nmarco,
                   IN t_pr_marco *marcos
                  );

void
update_marco_free_icblks(IN int nmarco,
                         INOUT t_pr_marco* pr_marco,
                         IN int nblk,
                         IN t_pr_marco** icblks
                         );

boolean
check_place_over(IN int nblk,
                 IN t_pr_marco** blks,
                 IN int nvnet,
				 IN t_vnet* vnets
                );

int
check_all_placed(IN int nblk,
                 IN t_pr_marco** blks,
                 IN int nvnet,
                 IN t_vnet* vnets
                );
int
initial_place_info(t_place_info* place_info);

int 
clear_left_right_place_info(t_place_info* place_info);

int 
get_left_place_info(t_place_info place_info);

int 
get_right_place_info(t_place_info place_info);

void
check_start_error(IN int nblk,
                  IN t_pr_marco** blks,
                  IN int nvnet,
                  IN t_vnet* vnets
                 );
