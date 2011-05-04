function test_daq_and_ufilt()

data1 = import_daq_data('3Hz.txt');
[ts,data2] = import_buffer_data('buffer.dat.unfilt');

plot(data1(4,:)); hold on;
plot(data2(1,:)+10);

xlim([0 1000]);
