/* bb_place.c */
int alloc_vnet_place_bb(int name, int *curloc, int blank, int column, int nvnet, t_vnet *vnets, int wcapacity);
int alloc_blk_place_bb(int name, int *curloc, int blank, int column, int nblk, t_pr_marco *blks);
int alloc_bb_place(int nblk, t_pr_marco *blks, int nvnet, t_vnet *vnets, t_bb_array *bb_array);
t_pr_marco *find_blk_with_name(int name, int column, int nblk, t_pr_marco *blks);
t_vnet *find_vnet_with_name(int name, int column, int nvnet, t_vnet *vnets);
int detail_place_blk(t_pr_marco *blk, int column, int left, int nmarco, t_pr_marco *pr_marco, int nvnet, t_vnet *vnets, t_bb_array *bb_array);
int detail_place_vnet(t_vnet *vnet, int column, int left, int nmarco, t_pr_marco *pr_marco, int nvnet, t_vnet *vnets, t_bb_array *bb_array);
int detail_place_big_vnet(int vnetoff, t_vnet *vnet, int wcapacity);
boolean detail_place(int nvnet, t_vnet *vnets, int nmarco, t_pr_marco *pr_marco, t_bb_array *bb_array);
