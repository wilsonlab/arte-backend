function u = arteSpikeViewer(u)
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

args.host = '1.1.2.54';
args.rxPort = 6303;
args.txPort = 10000;
args.udpObjBufferSize = 1024*8;

args.VOLT_MAX = 2000;

args.samplesPerChannel = 32;
args.channels = 4;

args.linestyle = '.';
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

%u = [];
nPacket = 0;
udpRxConnectionOpen = false;

% -------------------------------------------
%  Start Everything up
% -------------------------------------------

%initFigures;
    
initWaveformFigure();
initWaveformLine();

initNetwork();
startNetworkRx();
     
    function initWaveformFigure()
	fig(1) = figure('Position', [50 300 750 350],...
		 'toolbar', 'none', 'NumberTitle', 'off', 'Name', 'Waveforms', ...
		'color', 'k', 'DeleteFcn', @shutItDownFcn);

        waveAxes(1) = axes('Parent', fig(1), 'Box', 'on', 'Units', 'Normalized',...
            'Position', [.01 .02 .98 .96], 'Xtick', [], 'YTick', [],...
	    'XLim', [0.5 128.5], 'YLim', [0 args.VOLT_MAX], ...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');

    end    

    function initWaveformLine()
        disp('Initializing the waveform plotting line');
	temp = 1:128;
        waveLine(1) = line(temp,repmat(25,128,1), 'parent', waveAxes(1),...
			'color', 'w',  'linestyle', 'none', ...
			 'marker', '.', 'markerfacecolor', 'w','MarkerSize', 10);
	line([32.5 32.5], [0 args.VOLT_MAX], 'color', 'w');
	line([64.5 64.5], [0 args.VOLT_MAX], 'color', 'w');
	line([96.5 96.5], [0 args.VOLT_MAX], 'color', 'w');
%	get(waveLine,'Xdata')
%	get(waveLine,'YData')
%        waveLine(2) = line([0 0], [0 0], 'parent', waveAxes(2), 'Color', 'w', 'linestyle', args.linestyle);
%        waveLine(3) = line([0 0], [0 0], 'parent', waveAxes(3), 'Color', 'w', 'linestyle', args.linestyle);
%        waveLine(4) = line([0 0], [0 0], 'parent', waveAxes(4), 'Color', 'w', 'linestyle', args.linestyle);
    end
    
   

% -------------------------------------------
%   Network and Buffer Related Function
% -------------------------------------------
    function initNetwork()
        disp(['Initializing network to listen to host: ' args.host, ' port:', num2str(args.rxPort)]);
        u = udp(args.host, args.txPort, 'LocalPort', args.rxPort, ...
            'InputBufferSize', 2048);

        set(u,'DatagramReceivedFcn', @udpPacketRxCallback);
    end
    function startNetworkRx()
        disp('Starting the network');
        fopen(u);
        udpRxConnectionOpen = true;
    end

    function udpPacketRxCallback(obj, event)

        data = fread(obj, 145, 'uint16');
        nPacket = nPacket + 1;
        disp(['Packets recieved:', num2str(nPacket)]);

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
         data = data( args.bufferTrimTop : args.bufferTrimTop + args.dataSamplesPerBuffer-1 );
	
%        data = rand(args.samplesPerChannel, args.channels);
    end
    %function parseNetworkBuffer(buffer)
    %end
  
    function plotBuffer(buffer)
        plotWaveforms(buffer);
       % plotProjections(buffer);
    end

    function plotWaveforms(waveform)
                
        if (~exist('waveLine', 'var'))
            initWaveformLines;
        end
	
        waveform(1) = args.VOLT_MAX * ( mod(nPacket, 50)/50);
        set(waveLine(1), 'YData', waveform);
        
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
        delete(fig);
    end

   
end

