function u = mArteSpikeViewer(varargin)
warning off;
%-------------------------
%       NETWORK VARS
%-------------------------
args.host = '10.121.43.56';
args.rxPort = 6300;
args.txPort = 10000;
args.udpObjBufferSize = 1024*8;
args.packetWordSize = 'int16';
args.byteOrder = 'bigEndian';
args.dataOrderFlag = true;
args.enable_network = true;
args.dispPacketNumber = false;
nPacket = 0;

args.parseWithMex = false;
args.correctDataOffset = true;

u = udp;
nPacket = 0;
udpRxConnectionOpen = false;

if strcmp(args.packetWordSize, 'int16')
    args.bufferTrimTop = 8;
    args.sampleSize = 1;
%     args.bufferTrimBottom = 4*32 + args.bufferTrimTop;
else
   error('Invalid packetWordSize');
end

%-------------------------
%       Data Vars
%-------------------------

args.nSamp = 32;
args.nChan = 4;
args.sampsPerBuffer = args.nChan * args.nSamp * args.sampleSize;
args.dataIdx =  args.bufferTrimTop+1 : args.bufferTrimTop + args.sampsPerBuffer;



disp(' ');
disp('Starting up');

v(1) = plotArteSpikes([],[],[]);
set(v(1).fig,'Name', 'Spikes on port 6300');
v(2) = plotArteSpikes([],[],[]);
set(v(2).fig,'Name', 'Spikes on port 6301');
v(3) = plotArteSpikes([],[],[]);
set(v(3).fig,'Name', 'Spikes on port 6302');
v(4) = plotArteSpikes([],[],[]);
set(v(4).fig,'Name', 'Spikes on port 6303');

packets =[];
tic;
prevTime = toc;
deltaTime = .025;

if args.enable_network
    initNetwork();
    startNetworkRx();
else
    disp('Starting up without network');
end




% -------------------------------------------
%   Network and Buffer Related Function
% -------------------------------------------

    function initNetwork()
       

        disp('Setting up network'); 
        disp(['    host:' args.host, ' port:', num2str(args.rxPort)]);
        
        u(1) = udp(args.host, args.txPort, 'LocalPort', args.rxPort); 
        u(2) = udp(args.host, args.txPort+1, 'LocalPort', args.rxPort+1); 
        u(3) = udp(args.host, args.txPort+1, 'LocalPort', args.rxPort+2);
        u(4) = udp(args.host, args.txPort+1, 'LocalPort', args.rxPort+3);
        
        packets(1).buffer = [];
        packets(2).buffer = [];
        packets(3).buffer = [];
        packets(4).buffer = [];
        set(u,'DatagramReceivedFcn', @udpPacketRxCallback,...
            'InputBufferSize', 2048, 'byteOrder', args.byteOrder);
    end

    function startNetworkRx()
        fopen(u);
        udpRxConnectionOpen = true;
    end

    function udpPacketRxCallback(obj, event)
        if strcmp(get(obj,'Status'), 'closed')
            return
        end
        data = fread(obj, 145, 'uint16');
        nPacket = nPacket + 1;
        if (args.dispPacketNumber)
            disp(['Packets recieved:', num2str(nPacket)]);
        end
        if numel(data)<145
            fclose(u);
            fopen(u);
            return
        end
       
        spike = bufferToSpike(data);
        
        if get(obj,'localport')==args.rxPort
            packets(1).buffer(:,:,end+1) = spike;
        end
        if get(obj,'localport')==args.rxPort+1
            packets(2).buffer(:,:,end+1) = spike;
        end
        if get(obj,'localport')==args.rxPort+2
            packets(3).buffer(:,:,end+1) = spike;
        end
        if get(obj,'localport')==args.rxPort+3
            packets(4).buffer(:,:,end+1) = spike;
        end
        
        curTime = toc;
        
        if (curTime > prevTime + deltaTime)
            prevTime = curTime;
            plotPackets();
        end
%        get(obj,'localport')
        %drawnow();
    end

    function plotPackets
        for i = 1:size(packets(1))
            plotArteSpikes(v(1),packets(1).buffer(:,:,i));
        end
        for i = 1:size(packets(2))
            plotArteSpikes(v(2),packets(2).buffer(:,:,i)); 
        end
        for i = 1:size(packets(3))
            plotArteSpikes(v(3),packets(3).buffer(:,:,i));
        end
        for i = 1:size(packets(4))
            plotArteSpikes(v(4),packets(4).buffer(:,:,i));
        end
        drawnow();
        
    end
       
    function data = bufferToSpike(data)
        data = handParseBuffer(data);
    end

    function data = handParseBuffer(data)
        if args.correctDataOffset
           data = data + 2^15;
           idx = data>2^16;
           data(idx) = data(idx) - 2^16;
        end
        if args.dataOrderFlag 
            data = reshape( data( args.dataIdx), args.nChan, args.nSamp )';
        else
            data = reshape( data( args.dataIdx), args.nSamp, args.nChan );
        end
    end

    function shutItDownFcn(obj, event)
        disp('Shutting down the entire program');
        fclose(u);
        pause(.5);
        delete(l);
        delete(conFig);
        if ishandle(figObjs.fig)
            set(figObjs.fig,'DeleteFcn', []);
            delete(figObjs.fig);
        end
       
    end

end

