classdef helperVisualizeMotionAndStructureStereo < handle
%helperVisualizeMotionAndStructureStereo show map points and camera trajectory
%
%   This is an example helper class that is subject to change or removal 
%   in future releases.

%   Copyright 2020 The MathWorks, Inc.

    properties
        XLim = [-20 100]
        
        YLim = [-100 40]
        
        ZLim = [-5 15]
        
        Axes
    end
    
    properties (Access = private)
        MapPointsPlot
 
        EstimatedTrajectory
        
        OptimizedTrajectory

        CameraPlot
    end
    
    methods (Access = public)
        function obj = helperVisualizeMotionAndStructureStereo(vSetKeyFrames, mapPoints)
        
            [xyzPoints, currPose, trajectory]  = retrievePlottedData(obj, vSetKeyFrames, mapPoints);
             
            obj.MapPointsPlot = pcplayer(obj.XLim, obj.YLim, obj.ZLim, ...
                'VerticalAxis', 'y', 'VerticalAxisDir', 'down');
            
            obj.Axes  = obj.MapPointsPlot.Axes;
            
            color = xyzPoints(:, 3);
            color = min(2, max(-1, color));
            obj.MapPointsPlot.view(xyzPoints, color);
            obj.Axes.Children.DisplayName = 'Map points';
            
            hold(obj.Axes, 'on');
            
            % Set figure position on the screen
            movegui(obj.Axes.Parent, [1200 200]);
            
            % Plot camera trajectory
            obj.EstimatedTrajectory = plot3(obj.Axes, trajectory(:,1), trajectory(:,2), ...
                trajectory(:,3), 'r', 'LineWidth', 2 , 'DisplayName', 'Estimated trajectory');
            
            % Plot the current cameras
            obj.CameraPlot = plotCamera(currPose, 'Parent', obj.Axes, 'Size', 1);
            
            view(obj.Axes, [0 0 1]);
            camroll(obj.Axes, 90);
        end
        
        function updatePlot(obj, vSetKeyFrames, mapPoints)
            
            [xyzPoints, currPose, trajectory]  = retrievePlottedData(obj, vSetKeyFrames, mapPoints);
            
            % Update the point cloud
            color = xyzPoints(:, 3);
            color = min(10, max(-2, color));
            obj.MapPointsPlot.view(xyzPoints, color);
            
            % Update the camera trajectory
            set(obj.EstimatedTrajectory, 'XData', trajectory(:,1), 'YData', ...
                trajectory(:,2), 'ZData', trajectory(:,3));
            
            % Update the current camera pose since the first camera is fixed
            obj.CameraPlot.AbsolutePose = currPose.AbsolutePose;
            obj.CameraPlot.Label        = num2str(currPose.ViewId);
            
            drawnow limitrate
        end
        
        function plotOptimizedTrajectory(obj, poses)
            
            % Delete the camera plot
            delete(obj.CameraPlot);
            
            % Plot the optimized trajectory
            trans = vertcat(poses.AbsolutePose.Translation);
            obj.OptimizedTrajectory = plot3(obj.Axes, trans(:, 1), trans(:, 2), trans(:, 3), 'm', ...
                'LineWidth', 2, 'DisplayName', 'Optimized trajectory');
        end
        
        function plotActualTrajectory(obj, gTruth)
            
            % Plot the ground truth
            plot3(obj.Axes, gTruth(:,1), gTruth(:,2), gTruth(:,3), ...
                'g','LineWidth',2, 'DisplayName', 'GPS trajectory');
            drawnow limitrate
        end
        
        function showLegend(obj)
            % Add a legend to the axes
            hLegend = legend(obj.Axes, 'Location',  'southeast', ...
                'TextColor', [1 1 1], 'FontWeight', 'bold');
        end
    end
    
    methods (Access = private)
        function [xyzPoints, currPose, trajectory]  = retrievePlottedData(obj, vSetKeyFrames, mapPoints)
            camPoses    = poses(vSetKeyFrames);
            currPose    = camPoses(end,:); % Contains both ViewId and Pose
            trajectory  = vertcat(camPoses.AbsolutePose.Translation);
            xyzPoints   = mapPoints.WorldPoints;%(mapPoints.UserData.Validity,:);
            
            % Only plot the points within the limit
            inPlotRange = xyzPoints(:, 1) > obj.XLim(1) & ...
                xyzPoints(:, 1) < obj.XLim(2) & xyzPoints(:, 2) > obj.YLim(1) & ...
                xyzPoints(:, 2) < obj.YLim(2) & xyzPoints(:, 3) > obj.ZLim(1) & ...
                xyzPoints(:, 3) < obj.ZLim(2);
            xyzPoints   = xyzPoints(inPlotRange, :);
        end
    end
end

