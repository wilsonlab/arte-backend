function [ v ] = plotArteSpikes(v, waveform, u)

%-------------------------
%       Data Vars
%-------------------------
args.nSamp = 32;
args.nChan = 4;

%-------------------------
%       Plotting Vars
%-------------------------
args.linestyle = '-';
args.dispPacketNumber = false;
args.plotFrameNumber = true;
args.colors = 'rygc';


%-------------------------
%       Misc Vars
%-------------------------
args.VOLT_MAX = (2^16);
args.WAVE_MAX = 2^15*1.4;
args.WAVE_MIN = 2^14 * 1.5;
args.D_WAVE = args.WAVE_MAX - args.WAVE_MIN;

%-------------------------
%       UI Vars
%-------------------------
args.name = 'TT 14';
args.orientation = 'vertical';
args.showcontrols = true;

miny = .01;
dY = [];
vert_position = [04 270 395 850];
position  = [];
if strcmp(args.orientation, 'vertical')
    position = vert_position;
else
    disp('Horizontal Position not implemented');
    position = horz_position;
end


if isempty(v)
    v.fig = [];
    v.waveAx = [];
    v.projAx = [];
    v.dpix = [];

    v.waveLine = [];
    v.projImage = [];
    v.imgMat = [];
    v.nPacketText = [];
    v.nSpike = 0;
    
    v.netCon = [];
    
    initFigure();
    
    initWaveformAxes();
    initWaveformLines();
    
    initProjectionAxes();
    initProjectionImages();
end

if ~isempty(waveform)
    if isempty(v.netCon)
        v.netCon = u;
    end
    v.nSpike = v.nSpike +1;
    plotProjections(waveform);
    plotWaveforms(waveform);
end

% -------------------------------------------
%   GUI Related Functions
% -------------------------------------------
   function initFigure()
        disp('Initializing figures');
        v.fig(1) = figure('Position', position, 'toolbar', 'none', 'menubar', 'none',...
            'NumberTitle', 'off', 'Name', [args.name, ' - Arte Matlab Tetrode Viewer '], 'color' ,'k');
        set(v.fig,'DeleteFcn', @shutItDownFcn);        
    end

            
    function initWaveformAxes()
        disp('Initializing waveform plotting axes');
        v.waveAx(1) = axes('Parent', v.fig(1), 'Position', [.01 .75 .24 .24]);
        v.waveAx(2) = axes('Parent', v.fig(1), 'Position', [.26 .75 .24 .24]);
        v.waveAx(3) = axes('Parent', v.fig(1), 'Position', [.51 .75 .24 .24]);
        v.waveAx(4) = axes('Parent', v.fig(1), 'Position', [.76 .75 .23 .24]);
        
        set(v.waveAx, 'XLim', [1 args.nSamp], 'YLim', [args.WAVE_MIN args.WAVE_MAX], ...
            'Xtick', [], 'YTick', [], 'box', 'on', ...
            'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
    end

    function initProjectionAxes()
        disp('Initializing projection plotting axes');
        v.projAx(1) = axes('Parent', v.fig(1), 'Position', [.01 .50 .49 .24]);
        v.projAx(2) = axes('Parent', v.fig(1), 'Position', [.51 .50 .48 .24]);
        v.projAx(3) = axes('Parent', v.fig(1), 'Position', [.01 .25 .49 .24]);
        v.projAx(4) = axes('Parent', v.fig(1), 'Position', [.51 .25 .48 .24]); 
        v.projAx(5) = axes('Parent', v.fig(1), 'Position', [.01 .01 .49 .23]); 
        v.projAx(6) = axes('Parent', v.fig(1), 'Position', [.51 .01 .48 .23]); 
        
        set(v.projAx, 'Box', 'on', 'Color', 'k', 'xcolor', 'w', 'ycolor', 'w',...
            'Xlim', [0, args.D_WAVE], 'YLim', [0, args.D_WAVE]);
        linkaxes(v.projAx);
     end
    

    function initWaveformLines()
        disp('Initializing the waveform lines');
        
        v.waveLine(1) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', v.waveAx(1), 'Color', args.colors(1), 'linestyle', args.linestyle);
        v.waveLine(2) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', v.waveAx(2), 'Color', args.colors(2), 'linestyle', args.linestyle);
        v.waveLine(3) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', v.waveAx(3), 'Color', args.colors(3), 'linestyle', args.linestyle);
        v.waveLine(4) = line(1:args.nSamp, repmat(100, 1,args.nSamp), 'parent', v.waveAx(4), 'Color', args.colors(4), 'linestyle', args.linestyle);
        v.nPacketText = text(2,args.WAVE_MAX - args.WAVE_MAX * .025, 'Spike:0', 'color', 'w', 'parent', v.waveAx(1), 'fontsize', 10);
    end
    
    function initProjectionImages()
        
        disp('Initializing the projection images');
        
       
        v.dpix = getProjectionImageResolution(v.projAx(1));
        
        v.imgMat{1} = zeros(v.dpix, v.dpix,3);
        v.imgMat{2} = zeros(v.dpix, v.dpix,3);
        v.imgMat{3} = zeros(v.dpix, v.dpix,3);
        v.imgMat{4} = zeros(v.dpix, v.dpix,3);
        v.imgMat{5} = zeros(v.dpix, v.dpix,3);
        v.imgMat{6} = zeros(v.dpix, v.dpix,3);
        
        v.projImage(1) = image([],[],v.imgMat{1}, 'parent', v.projAx(1));
        v.projImage(2) = image([],[],v.imgMat{2}, 'parent', v.projAx(2));
        v.projImage(3) = image([],[],v.imgMat{3}, 'parent', v.projAx(3));
        v.projImage(4) = image([],[],v.imgMat{4}, 'parent', v.projAx(4));
        v.projImage(5) = image([],[],v.imgMat{5}, 'parent', v.projAx(5));
        v.projImage(6) = image([],[],v.imgMat{6}, 'parent', v.projAx(6));
        set(v.projAx,'xcolor', 'w', 'ycolor', 'w','Xtick', [],'YTick', [], 'Box', 'on',...
            'YDir', 'normal');       
    end
    function res = getProjectionImageResolution(a)

        units = get(a,'Units');
        set(a,'Units', 'pixels');
        dp = get(a,'Position');
        set(a,'Units',units);
        
        dpx = dp(3);
        dpy = dp(4);
        
        res = min([dpx, dpy]);
    
    end       

    function plotWaveforms(waveform)

%         
%         if (~exist('v.waveLine', 'var'))
%             initWaveformLines;
%         end
        if args.plotFrameNumber
            %waveform(1:2,1) = args.VOLT_MAX * ( mod(nPacket, 50)/50);
            set(v.nPacketText,'String', strcat('Spike:',num2str(v.nSpike)) );    
        end
        
        
        set(v.waveLine(1), 'YData', waveform(:,1));
        set(v.waveLine(2), 'YData', waveform(:,2));
        set(v.waveLine(3), 'YData', waveform(:,3));
        set(v.waveLine(4), 'YData', waveform(:,4));
   
    end

    function plotProjections(waveform)
        spikeAmplitude = max(waveform) - min(waveform);
        px = floor(voltToPixel(spikeAmplitude));

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
        v.imgMat{1}(px(1), px(2), 1:3) = c;
        v.imgMat{2}(px(1), px(3), 1:3) = c;
        v.imgMat{3}(px(1), px(4), 1:3) = c;
        v.imgMat{4}(px(2), px(3), 1:3) = c;
        v.imgMat{5}(px(2), px(4), 1:3) = c;
        v.imgMat{6}(px(3), px(4), 1:3) = c;
        
        set(v.projImage(1), 'CData', v.imgMat{1});
        set(v.projImage(2), 'CData', v.imgMat{2});
        set(v.projImage(3), 'CData', v.imgMat{3});
        set(v.projImage(4), 'CData', v.imgMat{4});
        set(v.projImage(5), 'CData', v.imgMat{5});
        set(v.projImage(6), 'CData', v.imgMat{6});
              
    end

    function pixels = voltToPixel(maxes)

        %pixels = floor( ((maxes - args.WAVE_MIN)./args.WAVE_MAX) .* v.dpix);
        pixels = floor( maxes./args.D_WAVE .* v.dpix);
        pixels(pixels<1) =1;
        pixels(pixels>v.dpix) = v.dpix;
    end

% -------------------------------------------
%  MISC Functions
% -------------------------------------------

    function shutItDownFcn(obj, event)
        disp('Closing Spike Viewer');
        set(v.fig,'DeleteFcn', []);
             
        disp(' ');
        class(v.netCon)
        get(v.netCon, 'status')
        if strcmp(class(v.netCon), 'udp') && ~strcmp(get(v.netCon,'Status') ,'closed')
            disp('Closing the network connection');
            fclose(u);
        end
        delete(v.fig);
    end

end

