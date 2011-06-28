function u = strippedDownReceiver(varargin)

args.host = '10.121.43.56';
args.rxPort = 4000;
args.txPort = 10000; % shouldn't matter as we aren't txing
args.udpObjBufferSize = 1024*8;

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
        u = udp(args.host, args.txPort, 'LocalPort', args.rxPort);
        set(u,'DatagramReceivedFcn', @udpPacketRxCallback);
        fopen(u);
    end

    function udpPacketRxCallback(obj, event)

        data = fread(obj, 512, 'uint16');
        %nPacket = nPacket + 1;
        %disp(['Packets recieved:', num2str(nPacket)]);
        disp('Packet read');
    end
end

