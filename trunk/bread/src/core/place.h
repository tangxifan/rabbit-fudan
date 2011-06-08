/*
 *Complete the placement of all IC blocks on the 
 *breadboard.Return TRUE if the placement succeed.
 *There are several steps during placement.
 *
 */
boolean
try_place(IN int nvnet,
          IN t_vnet* vnets,
          IN int nmarco,
          INOUT t_pr_marco* pr_marco,
          INOUT t_bb_array* bb_array
         );
