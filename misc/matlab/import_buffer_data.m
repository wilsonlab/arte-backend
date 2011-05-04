function [ts, data] = import_buffer_data(filename)

f = fopen(filename, 'rb');
n_buffers = fread(f, 1, 'uint32')
n_chans = fread(f, 1, 'uint16')
n_samps_per_chan_per_buffer = fread(f, 1, 'uint16')

% shape of final buffer will be chans in rows, samps in columns
% -200 is an error code for the plot - means this data never got written overb
data = -200.*ones(n_chans, n_samps_per_chan_per_buffer * n_buffers);

% shape of a raw data buffer is chans in cols, samps in rows
  this_buffer = zeros(n_chans,n_samps_per_chan_per_buffer);
%this_flat = reshape(this_buffer,1,[]);

for n = 1:n_buffers
%
%  for s = 1:n_samps_per_chan_per_buffer
%  for c = 1:n_chans
%  this_buffer(c,s) = fread(f,1,'int16');
%end
%end

this_buffer = fread(f, [1,numel(this_buffer)], 'int16');
this_buffer = reshape(this_buffer,n_chans,n_samps_per_chan_per_buffer);

 samp_ind = [1:n_samps_per_chan_per_buffer] + (n-1)*n_samps_per_chan_per_buffer;
%this_flat = fread(f,[1, numel(this_flat)], 'int16')
 % this_buffer = reshape(this_flat,[n_samps_per_chan_per_buffer,n_chans]);
%this_buffer = this_buffer';
%this_buffer = fread(f, [n_samps_per_chan_per_buffer, n_chans], 'int16');
  data(:, samp_ind) = this_buffer;
end

% file format doesn't include timestamps yet
ts = zeros(1, size(data,2));
