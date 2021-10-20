classdef helperVisualizeMatchedFeaturesStereo < handle
%helperVisualizeMatchedFeaturesStereo show the matched features in a frame
%
%   This is an example helper class that is subject to change or removal 
%   in future releases.

%   Copyright 2019-2020 The MathWorks, Inc.

    properties (Access = private)
        Image
        
        Feature
    end
    
    methods (Access = public)
        
        function obj = helperVisualizeMatchedFeaturesStereo(I1, I2, points1, points2, matchedPairs)
            
            % Plot image
            hFig  = figure;
            hAxes = newplot(hFig); 
            
            % Set figure visibility and position
            hFig.Visible = 'on';
            movegui(hFig, [200 200]);
            
            % Show the image and features
            obj.Image = showMatchedFeatures(I1, I2, points1(matchedPairs(:, 1)), ...
                points2(matchedPairs(:, 2)), 'montage', 'Parent', hAxes, ...
                'PlotOptions', {'ro','g+',''});
            title(hAxes, 'Matched Features in Current Frame');
            hold(hAxes, 'on');
            
            obj.Feature = findobj(hAxes.Parent,'Type','Line'); 
        end 
        
        function updatePlot(obj, I1, I2, points1, points2, trackedFeatureIdx, matchedPairs)
            [~, ia, ib]    = intersect(trackedFeatureIdx, matchedPairs(:, 1), 'stable');
            
            points1 = points1(trackedFeatureIdx(ia));
            points2 = points2(matchedPairs(ib, 2));
            
            % Stereo image
            obj.Image.CData   = [I1, I2];
            
            % Connecting lines
            obj.Feature(1).XData = NaN;
            obj.Feature(1).YData = NaN;
            
            % Right image
            obj.Feature(2).XData = points2.Location(:,1) + size(I1, 2);
            obj.Feature(2).YData = points2.Location(:,2);
            
            % Left image
            obj.Feature(3).XData = points1.Location(:,1);
            obj.Feature(3).YData = points1.Location(:,2);
            drawnow limitrate
        end
    end
end



