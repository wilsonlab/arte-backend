function u = arteSpikeViewer(varargin)
warning off;
% NETWORK VARS
args.host = '10.121.43.56';
args.rxPort = 6303;
args.txPort = 10000;
args.udpObjBufferSize = 1024*8;
args.packetWordSize = 'int16';
args.byteOrder = 'bigEndian';

args.dataOrderFlag = true;

% Data Vars
args.nSamp = 32;
args.nChan = 4;

%Plotting Vars
args.linestyle = '-';
args.dispPacketNumber = false;
args.plotFrameNumber = true;

%Misc Vars
args.VOLT_MAX = (2^16);
args.WAVE_MAX = 2^15*1.3;
args.WAVE_MIN = 2^14 * 1.5;
args.D_WAVE = args.WAVE_MAX - args.WAVE_MIN;

args.parseWithMex = false;
args.correctDataOffset = true;


if strcmp(args.packetWordSize, 'int16')
    args.bufferTrimTop = 8;
    args.sampleSize = 1;
%     args.bufferTrimBottom = 4*32 + args.bufferTrimTop;
else
   error('Invalid packetWordSize');
end

args.sampsPerBuffer = args.nChan * args.nSamp * args.sampleSize;
args.dataIdx =  args.bufferTrimTop+1 : args.bufferTrimTop + args.sampsPerBuffer;
%disp(['Predicted number of samples per buffer: ',  num2str(args.sampsPerBuffer)]);



% -------------------------------------------
%   UI Elements
% -------------------------------------------

fig = [];
waveAxes = [];
projAxes = [];
dx = [];
dy = [];
dpix = [];

waveLine = [];
projImage = [];
imgMat = [];
packetNumText = [];


% -------------------------------------------
%   Networking variables
% -------------------------------------------

u = [];
nPacket = 0;
udpRxConnectionOpen = false;

% -------------------------------------------
%  Start Everything up
% -------------------------------------------

disp(' ');
disp('Starting up');
initFigures;
    
initWaveformAxes();
initWaveformLines();

initProjectionAxes();    
initProjectionImages();

drawnow();

initNetwork();
%startNetworkRx();



% -------------------------------------------
%   Network and Buffer Related Function
% -------------------------------------------

    function initNetwork()
        
        disp('Setting up network'); 
        disp(['    host:' args.host, ' port:', num2str(args.rxPort)]);
        
        u = udp(args.host, args.txPort, 'LocalPort', args.rxPort, ...
            'InputBufferSize', 2048, 'byteOrder', args.byteOrder);
        set(u,'DatagramReceivedFcn', @udpPacketRxCallback);
        
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
            %disp('truncated buffer - resetting connection');
            fclose(u);
            fopen(u);
            return
        end
        
        plotBuffer(bufferToSpike(data));
        drawnow();
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
    %function parseNetworkBuffer(buffer)
    %end
    

% -------------------------------------------
%   GUI Related Functions
% -------------------------------------------
   function initFigures()
        disp('Initializing figures');
        fig(1) = figure('Position', [50 300 600 600], 'toolbar', 'none', 'NumberTitle', 'off', 'Name', 'Waveforms', 'color' ,'k');
        fig(2) = figure('Position', [660 300 900 600], 'toolbar', 'none', 'NumberTitle', 'off', 'Name', 'Projections', 'color' ,'k');
        set(fig,'DeleteFcn', @shutItDownFcn);
        
    end

            
    function initWaveformAxes()
        disp('Initializing waveform plotting axes');
        waveAxes(1) = axes('Parent', fig(1), 'Position', [.01 .01 .48 .48]);
        waveAxes(2) = axes('Parent', fig(1), 'Position', [.51 .01 .48 .48]);
        waveAxes(3) = axes('Parent', fig(1), 'Position', [.01 .51 .48 .48]);
        waveAxes(4) = axes('Parent', fig(1), 'Position', [.51 .51 .48 .48]);
        
        set(waveAxes, 'XLim', [1 args.nSamp], 'YLim', [args.WAVE_MIN args.WAVE_MAX], ...
            'Xtick', [], 'YTick', [], 'box', 'on', ...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
    end

    function initProjectionAxes()
        disp('Initializing projection plotting axes');
        projAxes(1) = axes('Parent', fig(2), 'Position', [.01 .01 .32 .48]);
        projAxes(2) = axes('Parent', fig(2), 'Position', [.34 .01 .32 .48]);
        projAxes(3) = axes('Parent', fig(2), 'Position', [.67 .01 .32 .48]);
        projAxes(4) = axes('Parent', fig(2), 'Position', [.01 .51 .32 .48]); 
        projAxes(5) = axes('Parent', fig(2), 'Position', [.34 .51 .32 .48]); 
        projAxes(6) = axes('Parent', fig(2), 'Position', [.67 .51 .32 .48]); 
        
        set(projAxes, 'Box', 'on', 'Color', 'k', 'xcolor', 'w', 'ycolor', 'w',...
            'Xlim', [0, args.D_WAVE], 'YLim', [0, args.D_WAVE]);
        linkaxes(projAxes);
     end
    

    function initWaveformLines()
        disp('Initializing the waveform lines');
        
        waveLine(1) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', waveAxes(1), 'Color', 'w', 'linestyle', args.linestyle);
        waveLine(2) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', waveAxes(2), 'Color', 'w', 'linestyle', args.linestyle);
        waveLine(3) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', waveAxes(3), 'Color', 'w', 'linestyle', args.linestyle);
        waveLine(4) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', waveAxes(4), 'Color', 'w', 'linestyle', args.linestyle);
        packetNumText = text(2,args.WAVE_MAX - args.WAVE_MAX * .05, 'Frame:0', 'color', 'w', 'parent', waveAxes(3), 'fontsize', 12);
    end
    
    function initProjectionImages()
        
        disp('Initializing the projection images');
        set(projAxes(1),'Units', 'pixels');
        dp = get(projAxes(1),'Position');
        dx = dp(3);
        dy = dp(4);
        
        dpix = min([dx, dy]);
        imgMat{1} = zeros(dpix, dpix,3);
        imgMat{2} = zeros(dpix, dpix,3);
        imgMat{3} = zeros(dpix, dpix,3);
        imgMat{4} = zeros(dpix, dpix,3);
        imgMat{5} = zeros(dpix, dpix,3);
        imgMat{6} = zeros(dpix, dpix,3);
        
        projImage(1) = image([],[],imgMat{1}, 'parent', projAxes(1));
        projImage(2) = image([],[],imgMat{2}, 'parent', projAxes(2));
        projImage(3) = image([],[],imgMat{3}, 'parent', projAxes(3));
        projImage(4) = image([],[],imgMat{4}, 'parent', projAxes(4));
        projImage(5) = image([],[],imgMat{5}, 'parent', projAxes(5));
        projImage(6) = image([],[],imgMat{6}, 'parent', projAxes(6));
        set(projAxes,'xcolor', 'w', 'ycolor', 'w','Xtick', [],'YTick', [], 'Box', 'on',...
            'YDir', 'normal');
        
      
        
    end

    function plotBuffer(buffer)
        plotWaveforms(buffer);
        plotProjections(buffer);
    end

    function plotWaveforms(waveform)

        if (~exist('waveLine', 'var'))
            initWaveformLines;
        end
        if args.plotFrameNumber
            %waveform(1:2,1) = args.VOLT_MAX * ( mod(nPacket, 50)/50);
            set(packetNumText,'String', strcat('Spike:',num2str(nPacket)) );    
        end
        
        set(waveLine(1), 'YData', waveform(:,1));
        set(waveLine(2), 'YData', waveform(:,2));
        set(waveLine(3), 'YData', waveform(:,3));
        set(waveLine(4), 'YData', waveform(:,4));
   
    end

    function plotProjections(waveform)
        spikeAmplitude = max(waveform) - min(waveform);
        px = voltToPixel(spikeAmplitude);
        imgMat{1}(px(1), px(2), 1:3) = 1;
        imgMat{2}(px(1), px(3), 1:3) = 1;
        imgMat{3}(px(1), px(4), 1:3) = 1;
        imgMat{4}(px(2), px(3), 1:3) = 1;
        imgMat{5}(px(2), px(4), 1:3) = 1;
        imgMat{6}(px(3), px(4), 1:3) = 1;
        
        set(projImage(1), 'CData', imgMat{1});
        set(projImage(2), 'CData', imgMat{2});
        set(projImage(3), 'CData', imgMat{3});
        set(projImage(4), 'CData', imgMat{4});
        set(projImage(5), 'CData', imgMat{5});
        set(projImage(6), 'CData', imgMat{6});
              
    end

    function pixels = voltToPixel(maxes)
        
        %pixels = floor( ((maxes - args.WAVE_MIN)./args.WAVE_MAX) .* dpix);
        pixels = floor( maxes./args.D_WAVE .* dpix);
        pixels(pixels<1) =1;
        pixels(pixels>dpix) = dpix;
    end
% -------------------------------------------
%  MISC Functions
% -------------------------------------------

    function shutItDownFcn(obj, event)
        set(fig,'DeleteFcn', []);
        
        disp('Shutting down');
        disp(' ');
        if udpRxConnectionOpen
            udpRxConnectionOpen = false;
            fclose(u);
        end
        delete(fig);
    end

   
end

