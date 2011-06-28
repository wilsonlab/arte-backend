function u = strippedDownReceiver(varargin)

args.host = '10.121.43.56';
args.txPort = 5000; % shouldn't matter as we aren't txing

% -------------------------------------------
%   Networking variables
% -------------------------------------------

u = [];
nPacket = 0;

% -------------------------------------------
%  Start Everything up
% -------------------------------------------

initNetwork();

% -------------------------------------------
%   Network and Buffer Related Function
% -------------------------------------------
    function initNetwork()
        u = udp(args.host, args.txPort);
        fopen(u);
    end

	while(true)
	fwrite(u, rand(100,1));
	pause(.01);

	end
	
end

