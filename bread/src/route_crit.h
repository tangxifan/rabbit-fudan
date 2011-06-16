/* route_crit.c */
int determine_block_base_on_bb(t_pr_marco *blk, t_bb_array *bb_array);
int determine_pins_of_blk_on_bb(t_pr_marco *blk, t_bb_array *bb_array);
int determine_bb_node_covered_by_blk(t_pr_marco *blk, t_bb_array *bb_array);
int route_blk_on_bb(t_pr_marco *blk, t_bb_array *bb_array);
int set_route_icblks_on_bb(int nblk, t_pr_marco *blks, t_bb_array *bb_array);
int route_special_vnet_on_bb(t_vnet *vnet, t_bb_array *bb_array);
int route_normal_vnet_on_bb(t_vnet *vnet, t_bb_array *bb_array);
int alloc_additional_space_for_normal_vnet(t_vnet *vnet, t_bb_array *bb_array);
int set_route_vnets_on_bb(int nvnet, t_vnet *vnets, t_bb_array *bb_array);
float try_route_marco_on_bb(t_pr_marco *marco, t_bb_array *bb_array);
int finish_route_marco_on_bb(t_pr_marco *marco, t_bb_array *bb_array);
int update_bb_array_route_cost(t_bb_array *bb_array);
