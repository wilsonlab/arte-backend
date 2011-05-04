function [data,b1,flat1] = import_daq_data(filename)

f = fopen(filename, 'rb');
n_buffers = fread(f,1,'uint32')
n_chans = fread(f, 1, 'uint16')
n_samps_per_chan_per_buffer = fread(f,1,'uint16')

% -100 is an error code for the plot - means that data never got written over
data = -100.*ones(n_chans, (n_buffers * n_samps_per_chan_per_buffer));

this_data = zeros(n_samps_per_chan_per_buffer, n_chans);
flat1 = reshape(this_data,1,[]);
this_flat = flat1;
size(this_flat)
for n = 0:(n_buffers-1)
  this_flat = fread(f, [1,numel(flat1)], 'int16');
%this_flat = this_flat';
%size(this_flat)
  this_data = reshape(this_flat,[n_chans,n_samps_per_chan_per_buffer]);
%this_data = this_data';
  %this_data = fread(f, [n_samps_per_chan_per_buffer, n_chans], 'int16');
  if(n == 0)
    b1 = this_data;
    flat1 = this_flat;
  end
data(:, [1:n_samps_per_chan_per_buffer]+ n * n_samps_per_chan_per_buffer) = this_data;
end

