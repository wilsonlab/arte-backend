function u = mArteSpikeViewer(varargin)
warning off;
%-------------------------
%       NETWORK VARS
%-------------------------
args.host = '10.121.43.56';
args.rxPort = 6302;
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

aSP(1) = ArteSpikePlot(num2str(args.rxPort));
aSP(2) = ArteSpikePlot(num2str(args.rxPort+1));

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
        
        u(1) = udp(args.host, args.txPort, 'LocalPort', args.rxPort, ...
            'InputBufferSize', 2048, 'byteOrder', args.byteOrder);
        
         u(2) = udp(args.host, args.txPort+1, 'LocalPort', args.rxPort+1, ...
             'InputBufferSize', 2048, 'byteOrder', args.byteOrder);
%         
        set(u,'DatagramReceivedFcn', @udpPacketRxCallback);
        
        
        aSP(1).udpObj = u(1);
        aSP(2).udpObj = u(2);

        %         aSP(2).udpObj = u(2);
        

    end

    function startNetworkRx()
        fopen(u);
        udpRxConnectionOpen = true;
    end

    function udpPacketRxCallback(obj, event)
        %disp(['Packet Rx:', num2str(get(obj,'LocalPort'))]);
        if strcmp(get(obj,'Status'), 'closed')
            return
        end
        data = fread(obj, 145, 'uint16');
        nPacket = nPacket + 1;
        if (args.dispPacketNumber)
            disp(['Packets recieved:', num2str(nPacket)]);
        end
        if numel(data)<145
            %disp('truncated buffer - resetting connection');
            fclose(u);
            fopen(u);
            return
        end
       
        %figObjs = plotArteSpikes(figObjs, bufferToSpike(data), u)
        spike = bufferToSpike(data);
        if get(obj,'localport')==args.rxPort
            aSP(1).plotSpike(spike);
        else
            aSP(2).plotSpike(spike);
        end
        %drawnow();
    end

    function data = bufferToSpike(data)
       if ~args.parseWithMex
            data = handParseBuffer(data);
       else
            data = mxParseSpikeBuffer(data);
       end
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

