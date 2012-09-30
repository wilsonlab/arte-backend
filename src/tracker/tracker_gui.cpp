#include "tracker_gui.h"

TrackerGui::TrackerGui (int argc, char *argv[],
                        TRACKER_CALLBACK_FN run_callback, void* cb_data_r,
                        TRACKER_CALLBACK_FN new_settings, void* cb_data_s){

  tracker_window = new TrackerWindow(argc, argv, run_callback, cb_data_r,
                                     new_settings, cb_data_s);

}
