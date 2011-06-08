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





