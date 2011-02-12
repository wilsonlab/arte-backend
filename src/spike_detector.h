
enum spikemode_t {THRESHOLD=0, LOCALMAX=1};

class SpikeDetector {

 public:
  
  spikemode_t mode;
  int n_chans;
  
