/* route_bias.c */
int route_bias_on_bb(t_pr_marco *bias, t_bb_array *bb_array, enum e_bias_type bias_type);
int try_route_bias_vnet_on_bb(t_location *srcloc, t_location *desloc, t_vnet *vnet, enum e_bias_type bias_type, t_bb_array *bb_array);
int try_find_bias_on_bb(t_location *src, t_location *des, enum e_bias_type bias_type, t_bb_array *bb_array);
int try_route_bias(int nvnet, t_vnet *vnets, int nmarco, t_pr_marco *marcos, t_bb_array *bb_array);
