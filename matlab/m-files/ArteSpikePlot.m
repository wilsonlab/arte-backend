
classdef ArteSpikePlot < handle
    % topo is a subclass of handle
    properties
 
        % UI Variables
        fig
        figPos
        waveAx
        waveLine
        projAx
        projImage
        imgMat
        imgMatBig
        imgSize
        imgSizeBig
        nSpikeLbl
        
        
        
        
        % Networking & Buffer Variables
        udpObj
        nSamp
        nChan
     
        %General Variables
        name
        nSpike
        
        %Display Variables
        volt_max
        wave_max
        wave_min
        delta_wave
        waveColors
        projColors
        waveLineStyle
        
        
        
        
        
    end % properties
    
    methods
        function obj = ArteSpikePlot(name)
            obj.name = name;
            
            obj.volt_max = 2^16;
            obj.wave_max = 2^15 * 1.9;
            obj.wave_min = 2^14 * 1.5;
            obj.delta_wave = obj.wave_max - obj.wave_min;
            
            obj.figPos = [04 270 395 550];
            
            obj.nChan = 4;
            obj.nSamp = 32;
            obj.waveColors = 'rygc';
            obj.waveLineStyle = '-';
            obj.nSpike = 0;

            %obj.projColors = [1 0 0; 1 1 0; 0 1 0; 0 1 1];
            obj.projColors = [1 1 1; 1 1 1; 1 1 1; 1 1 1];
            
            obj.initFigure();
            obj.initWaveformAxes();
            obj.initProjectionAxes();
            obj.initWaveformLines();
            obj.initProjectionImages();
            
        end % topo
        
        function initFigure(obj)
            disp('initializing figure');
            obj.fig = figure('position', obj.figPos, 'toolbar', 'none', 'menubar', 'none',...
                'numbertitle', 'off', 'name', [obj.name, ' - arte matlab tetrode viewer '], 'color' ,'k');
        end
        
        function initWaveformAxes(obj)
            disp('initializing waveform plotting axes');
            obj.waveAx(1) = axes('parent', obj.fig(1), 'position', [.01 .75 .24 .24]);
            obj.waveAx(2) = axes('parent', obj.fig(1), 'position', [.26 .75 .24 .24]);
            obj.waveAx(3) = axes('parent', obj.fig(1), 'position', [.51 .75 .24 .24]);
            obj.waveAx(4) = axes('parent', obj.fig(1), 'position', [.76 .75 .23 .24]);
            
            set(obj.waveAx, 'xlim', [1 obj.nSamp], 'ylim', [obj.wave_min obj.wave_max], ...
                'xtick', [], 'ytick', [], 'box', 'on', ...
                'color' ,'k', 'xcolor', 'w', 'ycolor', 'w');
        end
        
        function initProjectionAxes(obj)
            disp('initializing projection plotting axes');
            obj.projAx(1) = axes('parent', obj.fig(1), 'position', [.01 .50 .49 .24]);
            obj.projAx(2) = axes('parent', obj.fig(1), 'position', [.51 .50 .48 .24]);
            obj.projAx(3) = axes('parent', obj.fig(1), 'position', [.01 .25 .49 .24]);
            obj.projAx(4) = axes('parent', obj.fig(1), 'position', [.51 .25 .48 .24]);
            obj.projAx(5) = axes('parent', obj.fig(1), 'position', [.01 .01 .49 .23]);
            obj.projAx(6) = axes('parent', obj.fig(1), 'position', [.51 .01 .48 .23]);
            
            set(obj.projAx, 'box', 'on', 'color', 'k', 'xcolor', 'w', 'ycolor', 'w',...
                'xlim', [0, obj.delta_wave], 'ylim', [0, obj.delta_wave]);
            linkaxes(obj.projAx);
        end
        function initWaveformLines(obj)
            disp('initializing the waveform lines');
            size(obj.waveLine);
            size(obj.waveAx);
            obj.waveLine(1) = line(1:obj.nSamp, repmat(100, 1,obj.nSamp), 'parent', obj.waveAx(1), 'color', obj.waveColors(1), 'linestyle', obj.waveLineStyle);
            obj.waveLine(2) = line(1:obj.nSamp, repmat(100, 1,obj.nSamp), 'parent', obj.waveAx(2), 'color', obj.waveColors(2), 'linestyle', obj.waveLineStyle);
            obj.waveLine(3) = line(1:obj.nSamp, repmat(100, 1,obj.nSamp), 'parent', obj.waveAx(3), 'color', obj.waveColors(3), 'linestyle', obj.waveLineStyle);
            obj.waveLine(4) = line(1:obj.nSamp, repmat(100, 1,obj.nSamp), 'parent', obj.waveAx(4), 'color', obj.waveColors(4), 'linestyle', obj.waveLineStyle);
            obj.nSpikeLbl = text(2,obj.wave_max - obj.wave_max * .025, 'spike:0', 'color', 'w', 'parent', obj.waveAx(1), 'fontsize', 10);
        end

        
        function initProjectionImages(obj)
            
            disp('initializing the projection images');
%            obj.getProjImgRes();
            obj.imgSize = 300;
            
            obj.imgMat{1} = zeros(obj.imgSize, obj.imgSize,3);
            obj.imgMat{2} = zeros(obj.imgSize, obj.imgSize,3);
            obj.imgMat{3} = zeros(obj.imgSize, obj.imgSize,3);
            obj.imgMat{4} = zeros(obj.imgSize, obj.imgSize,3);
            obj.imgMat{5} = zeros(obj.imgSize, obj.imgSize,3);
            obj.imgMat{6} = zeros(obj.imgSize, obj.imgSize,3);
            
            obj.projImage(1) = image([],[],obj.imgMat{1}, 'parent', obj.projAx(1));
            obj.projImage(2) = image([],[],obj.imgMat{2}, 'parent', obj.projAx(2));
            obj.projImage(3) = image([],[],obj.imgMat{3}, 'parent', obj.projAx(3));
            obj.projImage(4) = image([],[],obj.imgMat{4}, 'parent', obj.projAx(4));
            obj.projImage(5) = image([],[],obj.imgMat{5}, 'parent', obj.projAx(5));
            obj.projImage(6) = image([],[],obj.imgMat{6}, 'parent', obj.projAx(6));
            set(obj.projAx,'xcolor', 'w', 'ycolor', 'w','xtick', [],'ytick', [], 'box', 'on',...
                'ydir', 'normal');
        end
       
        
        function getProjImgRes(obj)
            
            units = get(obj.projAx(1),'units');
            set(obj.projAx(1),'units', 'pixels');
            dp = get(obj.projAx(1),'position');
            set(obj.projAx(1),'units',units);
            
            dpx = dp(3);
            dpy = dp(4);
            
            obj.imgSize = min([dpx, dpy]);
            
        end
        
        function plotSpike(obj, waveform)
            if  ~ishandle(obj.fig)
                obj.delete();
                return
            end
         
            obj.nSpike = obj.nSpike + 1;
            set(obj.nSpikeLbl,'String', strcat('Spike:',num2str(obj.nSpike)) );
            
            obj.plotWaveforms(waveform);
            obj.plotProjections(waveform);
            drawnow();
        end
        function plotWaveforms(obj, waveform)
            set(obj.waveLine(1), 'YData', waveform(:,1));
            set(obj.waveLine(2), 'YData', waveform(:,2));
            set(obj.waveLine(3), 'YData', waveform(:,3));
            set(obj.waveLine(4), 'YData', waveform(:,4));
            
        end
        
        function plotProjections(obj, waveform)
            spikeAmplitude = max(waveform) - min(waveform);
            px = floor(ArteSpikePlot.voltToPixel(spikeAmplitude, obj.delta_wave, obj.imgSize));
            
            [~, i] = max(px);
            c = obj.projColors(i,:);
%             
%             switch i
%                 case 1
%                     c = [1 0 0];
%                 case 2
%                     c = [1 1 0];
%                 case 3
%                     c = [0 1 0];
%                 case 4
%                     c = [0 1 1];
%             end
            obj.imgMat{1}(px(1), px(2), 1:3) = c;
            obj.imgMat{2}(px(1), px(3), 1:3) = c;
            obj.imgMat{3}(px(1), px(4), 1:3) = c;
            obj.imgMat{4}(px(2), px(3), 1:3) = c;
            obj.imgMat{5}(px(2), px(4), 1:3) = c;
            obj.imgMat{6}(px(3), px(4), 1:3) = c;
            
            set(obj.projImage(1), 'CData', obj.imgMat{1});
            set(obj.projImage(2), 'CData', obj.imgMat{2});
            set(obj.projImage(3), 'CData', obj.imgMat{3});
            set(obj.projImage(4), 'CData', obj.imgMat{4});
            set(obj.projImage(5), 'CData', obj.imgMat{5});
            set(obj.projImage(6), 'CData', obj.imgMat{6});
            
        end
        
        
        
        function delete(obj)
            disp('Deleting');
            fclose(obj.udpObj);
            disp(['UDP Socket closed port:', num2str(get(obj.udpObj, 'localport'))]);
            delete(obj.fig);
        end
    end % end methods
    methods (Static = true)
        function pixels = voltToPixel(maxes, delta, dpix)
            
            %pixels = floor( ((maxes - args.WAVE_MIN)./args.WAVE_MAX) .* obj.dpix);
            pixels = floor( maxes./delta .* dpix);
            pixels(pixels<1) = 1;
            pixels(pixels>dpix) = dpix;
        end
    end
end % end class