/* route_path.c */
float cal_route_path_cost(t_location *src, t_location *des, t_bb_array *bb_array);
t_location *find_vnet_bbs(int *nbbs, t_vnet *vnet, t_bb_array *bb_array);
float find_path_cost_vnets(t_location *srcloc, t_location *desloc, t_vnet *src, t_vnet *des, t_bb_array *bb_array);
float try_right_route_pin_on_bb(t_pr_pin *spin, t_pr_pin *dpin, t_location *dloc, t_bb_array *bb_array);
float try_left_route_pin_on_bb(t_pr_pin *spin, t_pr_pin *dpin, t_location *dloc, t_bb_array *bb_array);
float try_route_marco_pin_on_bb(t_pr_pin *src, t_pr_pin *des, t_location *desloc, t_bb_array *bb_array);
float try_left_find_node(t_pr_pin *pin, t_location *leftloc, t_bb_array *bb_array);
float try_right_find_node(t_pr_pin *pin, t_location *rightloc, t_bb_array *bb_array);
int find_exloc_for_wire(IN t_location* exloc,IN t_location* wireloc, IN t_bb_array* bb_array);
