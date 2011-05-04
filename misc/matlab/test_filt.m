hold off;
%[d_dat,b1] = import_daq_data('3Hz.txt');
%plot(d_dat(8,:)+100,'o'); 
%hold on;


[ts,data] = import_buffer_data('buffer.dat.unfilt');
plot(data(2 ,:),'*-');

[ts,data2] = import_buffer_data('buffer.dat.filt');
hold on;
plot(data2(2,:),'go-');

xlim([0 256]);
