// object represents 1 tetrode.  Takes raw data in full buffers, filters the needed channels,
// stores the filtered data, scans it for spikes, sends the spikes to a drawing object, and to
// central file-saving object

#include "trode.h"

void trode_filter_data(Trode *, float64 *, int);

Trode::Trode(){
}

Trode::Trode(char *tn, ArteOpt *opt){

}

Trode::~Trode(){

}
