function u = arteSpikeViewer(varargin)
% ARTE2MATLAB - matlab function that is designed to be called from a mex file for plotting data recorded using arte
%   arte2matlab(buffer, nSamp, nChan, plotType)
%       buffer  - a 1d vector with data from arte
%       nChan   - the number of different channels contained in the buffer
%       nSamp   - the number of samples per channel in the buffer

% parser = inputParser;
% 
% parser.addOptional('txPort', 15000);
% parser.addOptional('rxPort', 5000);
%args.addOptional('plotWaveforms', 1);
%args.addOptional('plotProjections', 1);
%args.addOptional('waveformFigureHandle', []);
%args.addOptional('projectionFigureHandle', []);
% parser.parse(varargin);
% args = parser.Results()

args.host = '10.121.43.56';
args.rxPort = 4000;
args.txPort = 10000;
args.udpObjBufferSize = 1024*8;

args.samplesPerChannel = 32;
args.channels = 4;

args.linestyle = '-';
args.packetWordSize = 'uint16';

if strcmp(args.packetWordSize, 'uint16')
    args.bufferTrimTop = 7 + 2;
    args.sampleSize = 1;
%     args.bufferTrimBottom = 4*32 + args.bufferTrimTop;
else
    args.bufferTrimTop = (7*2)+1;
    args.sampleSize = 2;
%     args.bufferTrimBottom = (4*32 + args.bufferTrimTop) * 2;    
end

args.dataSamplesPerBuffer = args.channels * args.samplesPerChannel * args.sampleSize;
disp(['Predicted number of samples per buffer: ',  num2str(args.dataSamplesPerBuffer)]);



% -------------------------------------------
%   UI Elements
% -------------------------------------------

fig = [];
waveAxes = [];
projAxes = [];

waveLine = [];
projImage = [];

% -------------------------------------------
%   Networking variables
% -------------------------------------------

u = [];
nPacket = 0;
udpRxConnectionOpen = false;

% -------------------------------------------
%  Start Everything up
% -------------------------------------------

initFigures;
    
initWaveformAxes();
initProjectionAxes();
    
initWaveformLines();
initProjectionImages();
drawnow();

initNetwork();
%startNetworkRx();



% -------------------------------------------
%   Network and Buffer Related Function
% -------------------------------------------
%     function initNetwork()
%         u = udp(args.host, args.txPort, 'LocalPort', args.rxPort)
%         set(u,'DatagramReceivedFcn', @udpPacketRxCallback);
%     end
% 
%     function startNetworkRx()
%         fopen(u);
%         udpRxConnectionOpen = true;
%     end
% 
%     function udpPacketRxCallback(obj, event)
%         if udpRxConnectionOpen
%             nPacket = nPacket + 1;
%             %data = fread(obj, 512, args.packetWordSize);
%             data = bufferToSpike([]);
%             %plotBuffer(data);
%        
%             %drawnow();
%             
%             disp(['Packets recieved:', num2str(nPacket)]);
%             
%         end
%         
%     end
    function initNetwork()
        u = udp(args.host, args.txPort, 'LocalPort', args.rxPort, ...
            'InputBufferSize', 2048);
        set(u,'DatagramReceivedFcn', @udpPacketRxCallback);
        fopen(u);
        udpRxConnectionOpen = true;
    end

    function udpPacketRxCallback(obj, event)

        data = fread(obj, 290, 'uint16');
        nPacket = nPacket + 1;
        if numel(data)<145
            disp('truncated buffer');
            return
        end
        
        plotBuffer(bufferToSpike(data));
        drawnow();
    end

    function data = bufferToSpike(data)
       data = handParseBuffer(data);
       %data = mxParseSpikeBuffer(data);
    end
       
    function data = handParseBuffer(data)
         data = reshape( data( ...
             args.bufferTrimTop : args.bufferTrimTop + args.dataSamplesPerBuffer-1 ) ...
                         , args.samplesPerChannel, args.channels );
%        data = rand(args.samplesPerChannel, args.channels);
    end
    %function parseNetworkBuffer(buffer)
    %end
    

% -------------------------------------------
%   GUI Related Functions
% -------------------------------------------
   function initFigures()
        disp('initial run of plotArteSpikes, creating a figure');
        fig(1) = figure('Position', [50 300 600 600], 'toolbar', 'none', 'NumberTitle', 'off', 'Name', 'Waveforms', 'color' ,'k');
        fig(2) = figure('Position', [660 300 900 600], 'toolbar', 'none', 'NumberTitle', 'off', 'Name', 'Projections', 'color' ,'k');
%         fig(3) = figure('Position', [545 950 150 50], 'toolbar', 'none', 'NumberTitle', 'off', 'Name', 'Control');
%         initControlFig;

%         for figN = 1:numel(fig)
%             listerHandler(figN) = addlistener(fig(figN), 'BeingDeleted','PostSet', @shutItDownFcn);
%         end
        set(fig,'DeleteFcn', @shutItDownFcn);
    end

%     function initControlFig()
%         closeBtn = uicontrol('Style', 'pushbutton', 'Parent', fig(3), ...
%         'units', 'normalized', 'position', [0 0 1 1], 'String', 'Shut it down!', ...
%         'callback', @shutItDownFcn);
%     end
            
    function initWaveformAxes()
        waveAxes(1) = axes('Parent', fig(1), 'Box', 'on',...
            'Position', [.01 .01 .48 .48], 'Xtick', [], 'YTick', [],...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
        waveAxes(2) = axes('Parent', fig(1), 'Box', 'on',...
            'Position', [.51 .01 .48 .48], 'Xtick', [], 'YTick', [],...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
        waveAxes(3) = axes('Parent', fig(1), 'Box', 'on',...
            'Position', [.01 .51 .48 .48], 'Xtick', [], 'YTick', [],...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
        waveAxes(4) = axes('Parent', fig(1), 'Box', 'on',...
            'Position', [.51 .51 .48 .48], 'Xtick', [], 'YTick', [],...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
        
        set(waveAxes, 'Xlim', [1, 32]);
    end

    function initProjectionAxes()
        projAxes(1,2) = axes('Parent', fig(2), 'Box', 'on',...
            'Position', [.01 .01 .32 .48], 'Xtick', [], 'YTick', [],...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
        projAxes(1,3) = axes('Parent', fig(2), 'Box', 'on',...
            'Position', [.34 .01 .32 .48], 'Xtick', [], 'YTick', [],...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
        projAxes(1,4) = axes('Parent', fig(2), 'Box', 'on',...
            'Position', [.67 .01 .32 .48], 'Xtick', [], 'YTick', [],...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
        projAxes(2,3) = axes('Parent', fig(2), 'Box', 'on',...
            'Position', [.01 .51 .32 .48], 'Xtick', [], 'YTick', [],...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w'); 
        projAxes(2,4) = axes('Parent', fig(2), 'Box', 'on',...
            'Position', [.34 .51 .32 .48], 'Xtick', [], 'YTick', [],...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w'); 
        projAxes(3,4) = axes('Parent', fig(2), 'Box', 'on',...
            'Position', [.67 .51 .32 .48], 'Xtick', [], 'YTick', [],...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w'); 
        
    end
    

    function initWaveformLines()
        disp('Initializing the waveform plotting lines');
        waveLine(1) = line([0 0], [0 0], 'parent', waveAxes(1), 'Color', 'w', 'linestyle', args.linestyle);
        waveLine(2) = line([0 0], [0 0], 'parent', waveAxes(2), 'Color', 'w', 'linestyle', args.linestyle);
        waveLine(3) = line([0 0], [0 0], 'parent', waveAxes(3), 'Color', 'w', 'linestyle', args.linestyle);
        waveLine(4) = line([0 0], [0 0], 'parent', waveAxes(4), 'Color', 'w', 'linestyle', args.linestyle);
    end
    
    function initProjectionImages()
        disp('Initializing the projection images');
        projImage(1,2) = image(0,0,0, 'parent', projAxes(1,2));
        projImage(1,3) = image(0,0,0, 'parent', projAxes(1,3));
        projImage(1,4) = image(0,0,0, 'parent', projAxes(1,4));
        projImage(2,3) = image(0,0,0, 'parent', projAxes(2,3));
        projImage(2,4) = image(0,0,0, 'parent', projAxes(2,4));
        projImage(3,4) = image(0,0,0, 'parent', projAxes(3,4));
    end

    function plotBuffer(buffer)
        plotWaveforms(buffer);
        plotProjections(buffer);
    end

    function plotWaveforms(waveform)
                disp(['Packets recieved:', num2str(nPacket)]);

        if (~exist('waveLine', 'var'))
            initWaveformLines;
        end
        set(waveLine(1), 'XData', 1:args.samplesPerChannel, 'YData', waveform(:,1));
        set(waveLine(2), 'XData', 1:args.samplesPerChannel, 'YData', waveform(:,2));
        set(waveLine(3), 'XData', 1:args.samplesPerChannel, 'YData', waveform(:,3));
        set(waveLine(4), 'XData', 1:args.samplesPerChannel, 'YData', waveform(:,4));
    end
    function plotProjections(waveform)
        
        
        
    end

% -------------------------------------------
%  MISC Functions
% -------------------------------------------

    function shutItDownFcn(obj, event)
        set(fig,'DeleteFcn', []);
        
        disp('Shut it down');
        if udpRxConnectionOpen
            udpRxConnectionOpen = false;
            fclose(u);
        end
        close(fig);
    end

   
end

