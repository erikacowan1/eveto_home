#include "cbc_eveto.h"

//function that does a simple significance calculation
int eveto::calc_dumb_sig(
	TTree** cbc_trigger_tree_ptr,
	TTree** omicron_trigger_tree_ptr,
	bool verbose )
{

//at this point, all of the trees are loaded up with the proper indices. 
int dumb_sig;
int_dumb_sig_num;
for channel 0:

if value of j = value of k, 
then dumb_sig_num = (dumb_sig + 1)
and store values of trigger times in array trigs_2_veto[index]

else dumb_sig_num = dumb_sig_num

exit loop

dumb_sig = dumb_sig_num/num_line_om_trig_tree

(store dumb_sig in array)

then find max_sig, and save. 

