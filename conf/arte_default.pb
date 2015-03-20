arte_setup {
  daqs {
    dev_name: "Dev1"
    id: 0
    in_filename: "none"
    out_filename: "none"
    chans {
      gain: 1
      daq_chan: 0
    }
    chans {
      gain: 1
      daq_chan: 1
    }
    chans {
      gain: 1
      daq_chan: 2
    }
    chans {
      gain: 1
      daq_chan: 3
    }
    chans {
      gain: 1
      daq_chan: 4
    }
    chans {
      gain: 1
      daq_chan: 5
    }
    chans {
      gain: 1
      daq_chan: 6
    }
    chans {
      gain: 1
      daq_chan: 7
    }
    chans {
      gain: 1
      daq_chan: 8
    }
    chans {
      gain: 1
      daq_chan: 9
    }
    chans {
      gain: 1
      daq_chan: 10
    }
    chans {
      gain: 1
      daq_chan: 11
    }
    chans {
      gain: 1
      daq_chan: 12
    }
    chans {
      gain: 1
      daq_chan: 13
    }
    chans {
      gain: 1
      daq_chan: 14
    }
    chans {
      gain: 1
      daq_chan: 15
    }
    chans {
      gain: 1
      daq_chan: 16
    }
    chans {
      gain: 1
      daq_chan: 17
    }
    chans {
      gain: 1
      daq_chan: 18
    }
    chans {
      gain: 1
      daq_chan: 19
    }
    chans {
      gain: 1
      daq_chan: 20
    }
    chans {
      gain: 1
      daq_chan: 21
    }
    chans {
      gain: 1
      daq_chan: 22
    }
    chans {
      gain: 1
      daq_chan: 23
    }
    chans {
      gain: 1
      daq_chan: 24
    }
    chans {
      gain: 1
      daq_chan: 25
    }
    chans {
      gain: 1
      daq_chan: 26
    }
    chans {
      gain: 1
      daq_chan: 27
    }
    chans {
      gain: 1
      daq_chan: 28
    }
    chans {
      gain: 1
      daq_chan: 29
    }
    chans {
      gain: 1
      daq_chan: 30
    }
    chans {
      gain: 1
      daq_chan: 31
    }
    buffer_time_samps: 32
    buffer_n_chans: 32
    is_master: true
    is_multiplexing: false
  }
  daqs {
    dev_name: "Dev2"
    id: 1
    in_filename: "none"
    out_filename: "none"
    chans {
      gain: 1
      daq_chan: 0
    }
    chans {
      gain: 1
      daq_chan: 1
    }
    chans {
      gain: 1
      daq_chan: 2
    }
    chans {
      gain: 1
      daq_chan: 3
    }
    chans {
      gain: 1
      daq_chan: 4
    }
    chans {
      gain: 1
      daq_chan: 5
    }
    chans {
      gain: 1
      daq_chan: 6
    }
    chans {
      gain: 1
      daq_chan: 7
    }
    chans {
      gain: 1
      daq_chan: 8
    }
    chans {
      gain: 1
      daq_chan: 9
    }
    chans {
      gain: 1
      daq_chan: 10
    }
    chans {
      gain: 1
      daq_chan: 11
    }
    chans {
      gain: 1
      daq_chan: 12
    }
    chans {
      gain: 1
      daq_chan: 13
    }
    chans {
      gain: 1
      daq_chan: 14
    }
    chans {
      gain: 1
      daq_chan: 15
    }
    chans {
      gain: 1
      daq_chan: 16
    }
    chans {
      gain: 1
      daq_chan: 17
    }
    chans {
      gain: 1
      daq_chan: 18
    }
    chans {
      gain: 1
      daq_chan: 19
    }
    chans {
      gain: 1
      daq_chan: 20
    }
    chans {
      gain: 1
      daq_chan: 21
    }
    chans {
      gain: 1
      daq_chan: 22
    }
    chans {
      gain: 1
      daq_chan: 23
    }
    chans {
      gain: 1
      daq_chan: 24
    }
    chans {
      gain: 1
      daq_chan: 25
    }
    chans {
      gain: 1
      daq_chan: 26
    }
    chans {
      gain: 1
      daq_chan: 27
    }
    chans {
      gain: 1
      daq_chan: 28
    }
    chans {
      gain: 1
      daq_chan: 29
    }
    chans {
      gain: 1
      daq_chan: 30
    }
    chans {
      gain: 1
      daq_chan: 31
    }
    buffer_time_samps: 32
    buffer_n_chans: 32
    is_master: false
    is_multiplexing: false
  }
  command_port {
    command_host: "10.121.43.10"
    port: "5233"
    secondary_port: "5234"
  }
  host_list {
    host: "host"
    ip_addy: "1.1.2.54"
  }
  host_list {
    host: "host"
    ip_addy: "1.1.2.52"
  }
  filters {
    filter_name: "300to6000HzOrder4"
    numerators: 1
    numerators: 0
    numerators: -1
    numerators: 1
    numerators: 0
    numerators: -1
    denominators: 1
    denominators: -1.9171146
    denominators: 0.920735
    denominators: 1
    denominators: -0.51850516
    denominators: 0.24147114
    multiplier: 0.41499832
    multiplier: 0.41499832
    order: 4
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "300to6000HzOrder2"
    numerators: 1
    numerators: 0
    numerators: -1
    denominators: 1
    denominators: -1.1822397
    denominators: 0.2297195
    multiplier: 0.38514024
    order: 2
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "600to6000HzOrder4"
    numerators: 1
    numerators: 0
    numerators: -1
    numerators: 1
    numerators: 0
    numerators: -1
    denominators: 1
    denominators: -1.8368623
    denominators: 0.85181189
    denominators: 1
    denominators: -0.57004511
    denominators: 0.27811104
    multiplier: 0.39765674
    multiplier: 0.39765674
    order: 4
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "0p1to475HzOrder4"
    numerators: 1
    numerators: 0
    numerators: -1
    numerators: 1
    numerators: 0
    numerators: -1
    denominators: 1
    denominators: -1.9999722
    denominators: 0.99997222
    denominators: 1
    denominators: -1.8683273
    denominators: 0.87647921
    multiplier: 0.045143869
    multiplier: 0.045143869
    order: 4
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "1to475HzOrder4"
    numerators: 1
    numerators: 0
    numerators: -1
    numerators: 1
    numerators: 0
    numerators: -1
    denominators: 1
    denominators: -1.9997224
    denominators: 0.99972236
    denominators: 1
    denominators: -1.8687942
    denominators: 0.87691742
    multiplier: 0.045061007
    multiplier: 0.045061007
    order: 4
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "100to475HzOrder4"
    numerators: 1
    numerators: 0
    numerators: -1
    numerators: 1
    numerators: 0
    numerators: -1
    denominators: 1
    denominators: -1.9148045
    denominators: 0.92130888
    denominators: 1
    denominators: -1.9775841
    denominators: 0.97807389
    multiplier: 0.035885703
    multiplier: 0.035885703
    order: 4
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "1to200HzOrder2"
    numerators: 1
    numerators: 0
    numerators: -1
    denominators: 0
    denominators: -1.9616629
    denominators: 0.9616704
    multiplier: 0.019164799
    order: 2
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "1to200HzOrder4"
    numerators: 1
    numerators: 0
    numerators: -1
    numerators: 1
    numerators: 0
    numerators: -1
    denominators: 1
    denominators: -1.9997224
    denominators: 0.99972236
    denominators: 1
    denominators: -1.9450176
    denominators: 0.9465034
    multiplier: 0.019271193
    multiplier: 0.019271193
    order: 4
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "allPassOrder4"
    numerators: 1
    numerators: 0
    numerators: 0
    numerators: 1
    numerators: 0
    numerators: 0
    denominators: 0
    denominators: 0
    denominators: 0
    denominators: 0
    denominators: 0
    denominators: 0
    multiplier: 1
    multiplier: 1
    order: 4
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "3pointRect"
    numerators: 1
    numerators: 1
    numerators: 1
    denominators: 0
    denominators: 0
    denominators: 0
    multiplier: 0.33333
    order: 2
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "LP_60_Order2"
    numerators: 1
    numerators: 2
    numerators: 1
    denominators: 1
    denominators: -1.9833395
    denominators: 0.98347723
    multiplier: 3.4410779e-05
    order: 2
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
  filters {
    filter_name: "LP_60_Order4"
    numerators: 1
    numerators: 2
    numerators: 1
    numerators: 1
    numerators: 2
    numerators: 1
    denominators: 1
    denominators: -1.9908857
    denominators: 0.9910239
    denominators: 1
    denominators: -1.9783292
    denominators: 0.97846645
    multiplier: 3.4541703e-05
    multiplier: 3.4323846e-05
    order: 4
    filtfilt_invalid_samps: 0
    regenerate_coefs: false
  }
}
arte_session {
  main_filename: "artedate.data"
  default_trode {
    source_trode: 400
    trode_opt_version: 0
    chans {
      gain: 10000
      threshold: 0.61
      daq_chan: 0
      source_trode: 400
    }
    chans {
      gain: 10000
      threshold: 0.61
      daq_chan: 0
      source_trode: 400
    }
    chans {
      gain: 10000
      threshold: 0.61
      daq_chan: 1
      source_trode: 400
    }
    chans {
      gain: 10000
      threshold: 0.61
      daq_chan: 1
      source_trode: 400
    }
    samps_before_trig: 5
    samps_after_trig: 26
    refractory_period_samps: 32
    port: "none"
    filter_name: "allPassOrder4"
    daq_id: 0
  }
  trodes {
    source_trode: 0
    trode_opt_version: 0
    chans {
      daq_chan: 0
      source_trode: 0
    }
    chans {
      daq_chan: 1
      source_trode: 0
    }
    chans {
      daq_chan: 2
      source_trode: 0
    }
    chans {
      daq_chan: 3
      source_trode: 0
    }
    port: "5300"
  }
  trodes {
    source_trode: 1
    trode_opt_version: 0
    chans {
      threshold: 0.61
      daq_chan: 4
      source_trode: 1
    }
    chans {
      threshold: 0.61
      daq_chan: 5
      source_trode: 1
    }
    chans {
      threshold: 6.1
      daq_chan: 6
      source_trode: 1
    }
    chans {
      threshold: 0.61
      daq_chan: 7
      source_trode: 1
    }
    port: "5301"
  }
  trodes {
    source_trode: 2
    trode_opt_version: 0
    chans {
      daq_chan: 8
      source_trode: 2
    }
    chans {
      daq_chan: 9
      source_trode: 2
    }
    chans {
      daq_chan: 10
      source_trode: 2
    }
    chans {
      daq_chan: 11
      source_trode: 2
    }
    port: "5302"
  }
  trodes {
    source_trode: 3
    trode_opt_version: 0
    chans {
      threshold: 0.61
      daq_chan: 12
      source_trode: 3
    }
    chans {
      threshold: 0.61
      daq_chan: 13
      source_trode: 3
    }
    chans {
      threshold: 0.61
      daq_chan: 14
      source_trode: 3
    }
    chans {
      threshold: 0.61
      daq_chan: 15
      source_trode: 3
    }
    port: "5303"
  }
  trodes {
    source_trode: 4
    trode_opt_version: 0
    chans {
      threshold: 0.61
      daq_chan: 16
      source_trode: 4
    }
    chans {
      threshold: 0.61
      daq_chan: 17
      source_trode: 4
    }
    chans {
      threshold: 0.61
      daq_chan: 18
      source_trode: 4
    }
    chans {
      threshold: 0.61
      daq_chan: 19
      source_trode: 4
    }
    port: "5304"
  }
  trodes {
    source_trode: 5
    trode_opt_version: 0
    chans {
      threshold: 0.61
      daq_chan: 20
      source_trode: 5
    }
    chans {
      threshold: 0.61
      daq_chan: 21
      source_trode: 5
    }
    chans {
      threshold: 0.61
      daq_chan: 22
      source_trode: 5
    }
    chans {
      threshold: 0.61
      daq_chan: 23
      source_trode: 5
    }
    port: "5305"
  }
  trodes {
    source_trode: 6
    trode_opt_version: 0
    chans {
      threshold: 0.61
      daq_chan: 24
      source_trode: 6
    }
    chans {
      threshold: 6.1
      daq_chan: 25
      source_trode: 6
    }
    chans {
      threshold: 0.61
      daq_chan: 26
      source_trode: 6
    }
    chans {
      threshold: 0.61
      daq_chan: 27
      source_trode: 6
    }
    port: "5306"
  }
  trodes {
    source_trode: 7
    trode_opt_version: 0
    chans {
      threshold: 0.61
      daq_chan: 28
      source_trode: 7
    }
    chans {
      threshold: 0.61
      daq_chan: 29
      source_trode: 7
    }
    chans {
      threshold: 6.1
      daq_chan: 30
      source_trode: 7
    }
    chans {
      threshold: 0.61
      daq_chan: 31
      source_trode: 7
    }
    port: "5307"
  }
  trodes {
    source_trode: 8
    trode_opt_version: 0
    chans {
      daq_chan: 0
      source_trode: 8
    }
    chans {
      daq_chan: 1
      source_trode: 8
    }
    chans {
      daq_chan: 2
      source_trode: 8
    }
    chans {
      daq_chan: 3
      source_trode: 8
    }
    port: "5308"
    daq_id: 1
  }
  trodes {
    source_trode: 9
    trode_opt_version: 0
    chans {
      daq_chan: 4
      source_trode: 9
    }
    chans {
      daq_chan: 5
      source_trode: 9
    }
    chans {
      daq_chan: 6
      source_trode: 9
    }
    chans {
      daq_chan: 7
      source_trode: 9
    }
    port: "5309"
    daq_id: 1
  }
  trodes {
    source_trode: 10
    trode_opt_version: 0
    chans {
      daq_chan: 8
      source_trode: 10
    }
    chans {
      daq_chan: 9
      source_trode: 10
    }
    chans {
      daq_chan: 10
      source_trode: 10
    }
    chans {
      daq_chan: 11
      source_trode: 10
    }
    port: "5310"
    daq_id: 1
  }
  trodes {
    source_trode: 11
    trode_opt_version: 0
    chans {
      daq_chan: 12
      source_trode: 11
    }
    chans {
      daq_chan: 13
      source_trode: 11
    }
    chans {
      daq_chan: 14
      source_trode: 11
    }
    chans {
      daq_chan: 15
      source_trode: 11
    }
    port: "5311"
    daq_id: 1
  }
  trodes {
    source_trode: 12
    trode_opt_version: 0
    chans {
      daq_chan: 16
      source_trode: 12
    }
    chans {
      daq_chan: 17
      source_trode: 12
    }
    chans {
      daq_chan: 18
      source_trode: 12
    }
    chans {
      daq_chan: 19
      source_trode: 12
    }
    port: "5312"
    daq_id: 1
  }
  trodes {
    source_trode: 13
    trode_opt_version: 0
    chans {
      daq_chan: 20
      source_trode: 13
    }
    chans {
      daq_chan: 21
      source_trode: 13
    }
    chans {
      daq_chan: 22
      source_trode: 13
    }
    chans {
      daq_chan: 23
      source_trode: 13
    }
    port: "5313"
    daq_id: 1
  }
  trodes {
    source_trode: 14
    trode_opt_version: 0
    chans {
      daq_chan: 24
      source_trode: 14
    }
    chans {
      daq_chan: 25
      source_trode: 14
    }
    chans {
      daq_chan: 26
      source_trode: 14
    }
    chans {
      daq_chan: 27
      source_trode: 14
    }
    port: "5314"
    daq_id: 1
  }
  trodes {
    source_trode: 15
    trode_opt_version: 0
    chans {
      daq_chan: 28
      source_trode: 15
    }
    chans {
      daq_chan: 29
      source_trode: 15
    }
    chans {
      daq_chan: 30
      source_trode: 15
    }
    chans {
      daq_chan: 31
      source_trode: 15
    }
    port: "5315"
    daq_id: 1
  }
  default_lfpbank {
    source_lfpbank: 0
    lfp_opt_version: 0
    chans {
      gain: 1
      daq_chan: 3
      source_trode: 0
    }
    chans {
      gain: 1
      daq_chan: 7
      source_trode: 1
    }
    chans {
      gain: 1
      daq_chan: 11
      source_trode: 2
    }
    chans {
      gain: 1
      daq_chan: 14
      source_trode: 3
    }
    chans {
      gain: 1
      daq_chan: 19
      source_trode: 4
    }
    chans {
      gain: 1
      daq_chan: 23
      source_trode: 5
    }
    chans {
      gain: 1
      daq_chan: 27
      source_trode: 6
    }
    chans {
      gain: 1
      daq_chan: 29
      source_trode: 7
    }
    keep_nth_sample: 16
    port: "5227"
    filter_name: "allPassOrder4"
    daq_id: 0
  }
  lfpbanks {
    source_lfpbank: 0
    lfp_opt_version: 0
    chans {
      gain: 1
      daq_chan: 3
      source_trode: 0
    }
    chans {
      gain: 1
      daq_chan: 7
      source_trode: 1
    }
    chans {
      gain: 1
      daq_chan: 11
      source_trode: 2
    }
    chans {
      gain: 1
      daq_chan: 14
      source_trode: 3
    }
    chans {
      gain: 1
      daq_chan: 19
      source_trode: 4
    }
    chans {
      gain: 1
      daq_chan: 23
      source_trode: 5
    }
    chans {
      gain: 1
      daq_chan: 27
      source_trode: 6
    }
    chans {
      gain: 1
      daq_chan: 29
      source_trode: 7
    }
    keep_nth_sample: 16
    port: "5227"
    filter_name: "allPassOrder4"
    daq_id: 0
  }
  lfpbanks {
    source_lfpbank: 1
    lfp_opt_version: 0
    chans {
      gain: 1
      daq_chan: 3
      source_trode: 8
    }
    chans {
      gain: 1
      daq_chan: 7
      source_trode: 9
    }
    chans {
      gain: 1
      daq_chan: 11
      source_trode: 10
    }
    chans {
      gain: 1
      daq_chan: 14
      source_trode: 11
    }
    chans {
      gain: 1
      daq_chan: 19
      source_trode: 12
    }
    chans {
      gain: 1
      daq_chan: 23
      source_trode: 13
    }
    chans {
      gain: 1
      daq_chan: 27
      source_trode: 14
    }
    chans {
      gain: 1
      daq_chan: 29
      source_trode: 15
    }
    keep_nth_sample: 16
    port: "5228"
    filter_name: "allPassOrder4"
    daq_id: 1
  }
}
