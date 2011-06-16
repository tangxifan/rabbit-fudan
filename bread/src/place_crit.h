/*Functions quoted in placement*/
int
find_starter(IN int nblk,
             IN t_pr_marco* blks,
             IN int nvnet,
             IN t_vnet* vnets,
             IN t_place_info *place_info,
             IN t_bb_array bb_array
             );

boolean
find_match(IN int nblk,
           IN t_pr_marco* blks,
           IN int nvnet,
           IN t_vnet* vnets,
           IN t_bb_array bb_array,
           INOUT t_place_info* place_info
          );

int
determine_net_place_location(IN t_vnet* net,
                             INOUT t_place_info* place_info
                            );

int
determine_blk_place_location(IN t_pr_marco* blk,
                             INOUT t_place_info* place_info
                            );

int
find_vnet_pwidth(IN t_vnet* vnet,
                 IN int wcapacity);

int
find_blk_pwidth(IN int nblk,
                IN t_pr_marco* blks,
                IN t_pr_marco* blk,
                IN int wcapacity
               );




