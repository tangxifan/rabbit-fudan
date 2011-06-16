/* place_common.c */
int check_pin_direct_connect(t_pr_pin *pin, t_pr_marco *des);
boolean check_parent_type(t_pr_pin *src, enum e_pr_type tcomp);
int cal_mv_attract(t_pr_marco *src, t_vnet *des);
int cal_vv_close(t_vnet *src, t_vnet *des);
int cal_mv_close(t_pr_marco *src, t_vnet *des);
int cal_vm_close(t_vnet *src, t_pr_marco *des);
int cal_mm_close_in_vv(t_pr_marco *src, t_pr_marco *des);
int cal_mm_close(t_pr_marco *src, t_pr_marco *des);
int find_mv_close(t_pr_marco *src, t_vnet *des);
int find_vm_close(t_vnet *src, t_pr_marco *des);
int find_mm_close(t_pr_marco *src, t_pr_marco *des);
int find_vnet_basic_width(t_vnet *vnet, int wcapacity);
int find_mnet_basic_width(t_vnet *vnet, int wcapacity);
