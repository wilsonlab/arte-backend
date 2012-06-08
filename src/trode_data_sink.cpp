#include "trode_data_sink.h"

TrodeDataSink::TrodeDataSink( ArteTrodeOptPb &trode_opt )
{

  id =                 trode_opt.source_trode();
  trode_opt_version =  trode_opt.trode_opt_version();
  daq_id =             trode_opt.daq_id();

  for(int i = 0; i < trode_opt.chans_size(); i++){

    ArteChanOptPb this_chan = trode_opt.chans(i);
    input_gain.push_back( this_chan.gain() );
    threshold.push_back(  this_chan.threshold() );
    daq_chans.push_back( this_chan.daq_chan );
    
