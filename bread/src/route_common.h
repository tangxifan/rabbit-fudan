/* This file was automatically generated.  Do not edit! */
boolean check_all_routed(IN int nvnet,IN t_vnet *vnet,IN int nmarco,INOUT t_pr_marco *marcos);
int set_wired_on_bb(IN t_location *src,IN t_location *des,IN t_bb_array *bb_array);
int find_near_node_on_bb(IN t_location *src,IN t_bb_array *bb_array,IN t_location *rtloc);
int find_top_inner_on_bb(IN t_location *src,IN t_bb_array *bb_array,IN t_location *rtloc);
int set_bb_node_occupied(IN t_location *src,IN t_bb_array *bb_array);
int set_bb_net_unroutable(IN t_location *src,IN t_bb_array *bb_array);
int set_bb_node_unroutable(IN t_location *src,IN t_bb_array *bb_array);
float get_bb_node_route_cost(IN t_location *src,IN t_bb_array *bb_array);
int find_manhattan_distance(IN t_location *src,IN t_location *des);
boolean check_bb_node_vnet(IN t_location *src,IN t_vnet *vnet,IN t_bb_array *bb_array);
boolean check_bb_node_unroutable(IN t_location *src,IN t_bb_array *bb_array);
boolean check_bb_node_occupied(IN t_location *src,IN t_bb_array *bb_array);
boolean check_bb_bias_type(IN t_location *src,IN enum e_bias_type bias_type,IN t_bb_array *bb_array);
boolean check_bb_node_blank(IN t_location *src,IN t_bb_array *bb_array);
int set_location_value(INOUT t_location *location,IN int locx,IN int locy);
int alter_route_cost(IN t_bb_node *bb_node);
