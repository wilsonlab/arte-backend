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
args.linestyle = '.';
args.dispPacketNumber = false;
args.plotFrameNumber = true;
args.colors = 'rygc';

%Misc Vars
args.VOLT_MAX = (2^16);
args.WAVE_MAX = 2^15*1.4;
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
        fig(1) = figure('Position', [22 80 530 1000], 'toolbar', 'none',...
            'NumberTitle', 'off', 'Name', 'Arte Matlab Tetrode Viewer', 'color' ,'k');
        set(fig,'DeleteFcn', @shutItDownFcn);
        
    end

            
    function initWaveformAxes()
        disp('Initializing waveform plotting axes');
        waveAxes(1) = axes('Parent', fig(1), 'Position', [.01 .75 .24 .24]);
        waveAxes(2) = axes('Parent', fig(1), 'Position', [.26 .75 .24 .24]);
        waveAxes(3) = axes('Parent', fig(1), 'Position', [.51 .75 .24 .24]);
        waveAxes(4) = axes('Parent', fig(1), 'Position', [.76 .75 .23 .24]);
        
        set(waveAxes, 'XLim', [1 args.nSamp], 'YLim', [args.WAVE_MIN args.WAVE_MAX], ...
            'Xtick', [], 'YTick', [], 'box', 'on', ...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
    end

    function initProjectionAxes()
        disp('Initializing projection plotting axes');
        projAxes(1) = axes('Parent', fig(1), 'Position', [.01 .50 .49 .24]);
        projAxes(2) = axes('Parent', fig(1), 'Position', [.51 .50 .48 .24]);
        projAxes(3) = axes('Parent', fig(1), 'Position', [.01 .25 .49 .24]);
        projAxes(4) = axes('Parent', fig(1), 'Position', [.51 .25 .48 .24]); 
        projAxes(5) = axes('Parent', fig(1), 'Position', [.01 .01 .49 .23]); 
        projAxes(6) = axes('Parent', fig(1), 'Position', [.51 .01 .48 .23]); 
        
        set(projAxes, 'Box', 'on', 'Color', 'k', 'xcolor', 'w', 'ycolor', 'w',...
            'Xlim', [0, args.D_WAVE], 'YLim', [0, args.D_WAVE]);
        linkaxes(projAxes);
     end
    

    function initWaveformLines()
        disp('Initializing the waveform lines');
        
        waveLine(1) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', waveAxes(1), 'Color', args.colors(1), 'linestyle', args.linestyle);
        waveLine(2) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', waveAxes(2), 'Color', args.colors(2), 'linestyle', args.linestyle);
        waveLine(3) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', waveAxes(3), 'Color', args.colors(3), 'linestyle', args.linestyle);
        waveLine(4) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', waveAxes(4), 'Color', args.colors(4), 'linestyle', args.linestyle);
        packetNumText = text(2,args.WAVE_MAX - args.WAVE_MAX * .025, 'Spike Count:0', 'color', 'w', 'parent', waveAxes(1), 'fontsize', 10);
    end
    
    function initProjectionImages()
        
        disp('Initializing the projection images');
        
        set(projAxes(1),'Units', 'pixels');
        dp = get(projAxes(1),'Position');
        set(projAxes(1),'Units','normalized');
        
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
            set(packetNumText,'String', strcat('Spike Count:',num2str(nPacket)) );    
        end
        
        set(waveLine(1), 'YData', waveform(:,1));
        set(waveLine(2), 'YData', waveform(:,2));
        set(waveLine(3), 'YData', waveform(:,3));
        set(waveLine(4), 'YData', waveform(:,4));
   
    end

    function plotProjections(waveform)
        spikeAmplitude = max(waveform) - min(waveform);
        px = voltToPixel(spikeAmplitude);
        
        [~, i] = max(px);
        c = [];
        switch i
            case 1
                c = [1 0 0];
            case 2
                c = [1 1 0];
            case 3
                c = [0 1 0];
            case 4
                c = [0 1 1];
            otherwise
                c = [1 1 1];
        end
        imgMat{1}(px(1), px(2), 1:3) = c;
        imgMat{2}(px(1), px(3), 1:3) = c;
        imgMat{3}(px(1), px(4), 1:3) = c;
        imgMat{4}(px(2), px(3), 1:3) = c;
        imgMat{5}(px(2), px(4), 1:3) = c;
        imgMat{6}(px(3), px(4), 1:3) = c;
        
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

