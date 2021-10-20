% helperViewDirectionAndDepth Object for storing view direction and depth
%   Use this object to store map points attributes, such as view direction,
%   predicted depth range, and the ID of the major view that contains the
%   representative feature descriptor.
%
%   This is an example helper class that is subject to change or removal 
%   in future releases.
%
%   viewAndDepth = helperViewDirectionAndDepth(numPoints) returns a 
%   helperViewDirectionAndDepth object. 
%
%   helperViewDirectionAndDepth properties:
%   ViewDirection       - An M-by-3 matrix representing the view direction
%   MaxDistance         - An M-by-1 vector representing the maximum scale 
%                         invariant distance 
%   MinDistance         - An M-by-1 vector representing the minimum scale 
%                         invariant distance 
%   MajorViewId         - An M-by-1 vector containing the Id of the major view
%                         that contains the representative feature descriptor 
%   MajorFeatureIndex   - An M-by-1 vector containing the index of the 
%                         representative feature in the major view 
%
%   helperViewDirectionAndDepth methods:
%
%   update             - Add view direction and depth
%   remove             - Remove view direction and depth

%   Copyright 2020 The MathWorks, Inc.

classdef helperViewDirectionAndDepth
    properties
        %ViewDirection  An M-by-3 matrix representing the view direction of
        %   each map point
        ViewDirection
        
        %MaxDistance An M-by-1 vector representing the maximum scale 
        % 	invariant distance for each map point
        MaxDistance
        
        %MinDistance An M-by-1 vector representing the minimum scale 
        % 	invariant distance for each map point
        MinDistance
        
        %MajorViewId An M-by-1 vector containing the Id of the major view
        %   that contains the representative feature descriptor for each
        %   map point
        MajorViewId
        
        %MajorFeatureIndex An M-by-1 vector containing the index of the 
        %	representative feature in the major view for each map point
        MajorFeatureIndex
    end
    
    methods (Access = public)
        function this = helperViewDirectionAndDepth(numPoints)
            this.ViewDirection     = zeros(numPoints, 3);
            this.MaxDistance       = zeros(numPoints, 1);
            this.MinDistance       = zeros(numPoints, 1);
            this.MajorFeatureIndex = ones(numPoints, 1);
            this.MajorViewId       = ones(numPoints, 1, 'uint32');
        end
        
        function this = initialize(this, numPoints)
            numOldPoints = numel(this.MaxDistance);
            if numPoints > numOldPoints
                idx = numOldPoints+1:numPoints;
                this.ViewDirection(idx, :) = zeros(numel(idx),3);
                this.MaxDistance(idx)       = 0;
                this.MinDistance(idx)       = 0;
                this.MajorFeatureIndex(idx) = 1;
                this.MajorViewId(idx, :)    = uint32(1);
            end
        end

        function this = remove(this, idx)
            this.ViewDirection(idx, :)  = [];
            this.MaxDistance(idx)       = [];
            this.MinDistance(idx)       = [];
            this.MajorFeatureIndex(idx) = [];
            this.MajorViewId(idx, :)    = [];
        end
        
        function this = update(this, mapPointSet, views, mapPointsIndices, updateMajorFeatureIndex)
            
            this = initialize(this, mapPointSet.Count);

            % Extract the columns for faster query
            viewsLocations= vertcat(views.AbsolutePose.Translation);
            viewsFeatures = views.Features;
            viewsPoints   = views.Points;
            viewsScales   = cellfun(@(x) x.Scale, viewsPoints, 'UniformOutput', false);
            
            [keyFrameIds, allFeatureIdx] = findViewsOfWorldPoint(mapPointSet, mapPointsIndices);
            
            % Update for each map point
            for j = 1: numel(mapPointsIndices)
                pointIdx      = mapPointsIndices(j);
                keyFrameIdsOfPoint = keyFrameIds{j};
                featureIdxOfPoints = allFeatureIdx{j};
                numCameras    = numel(keyFrameIdsOfPoint);
                
                % Update mean viewing direction
                allFeatures   = zeros(numCameras, 32, 'uint32');
                allScales     = zeros(numCameras, 1);
                
                for k = 1:numCameras
                    tempId            = keyFrameIdsOfPoint(k);
                    features          = viewsFeatures{tempId};
                    scales            = viewsScales{tempId};
                    featureIndex      = featureIdxOfPoints(k);
                    allFeatures(k, :) = features(featureIndex, :);
                    allScales(k)      = scales(featureIndex);
                end
                
                directionVec = mapPointSet.WorldPoints(pointIdx,:) - viewsLocations(keyFrameIdsOfPoint,:);
                
                % Update view direction
                meanViewVec = mean(directionVec ./ (vecnorm(directionVec, 2, 2)), 1);
                this.ViewDirection(pointIdx, :) = meanViewVec/norm(meanViewVec);
                
                if updateMajorFeatureIndex
                    
                    % Identify the distinctive descriptor and the associated key frame
                    distIndex = computeDistinctiveDescriptors(allFeatures);
                    
                    % Update the distinctive key frame index
                    this.MajorViewId(pointIdx)       = keyFrameIdsOfPoint(distIndex);
                    this.MajorFeatureIndex(pointIdx) = featureIdxOfPoints(distIndex);
                else
                    majorViewId = this.MajorViewId(pointIdx);
                    distIndex   = find(keyFrameIdsOfPoint == majorViewId);
                end
                
                % Update depth range
                distDirectionVec = directionVec(distIndex,:);
                distKeyFrameId   = keyFrameIdsOfPoint(distIndex);
                maxDist          = norm(distDirectionVec)* allScales(distIndex);
                
                minDist          = maxDist/max(viewsScales{distKeyFrameId});
                
                this.MaxDistance(pointIdx, 1) = maxDist;
                this.MinDistance(pointIdx, 1) = minDist;
            end
            
        end
    end
end

%------------------------------------------------------------------
function index = computeDistinctiveDescriptors(features)
%computeDistinctiveDescriptors Find the distinctive discriptor

if size(features, 1) < 3
    index       = size(features, 1);
else
    scores      = helperHammingDistance(features, features);
    [~, index]  = min(sum(scores, 2));
end
end

function scores = helperHammingDistance(features1, features2)
%helperHammingDistance compute hamming distance between two groups of
%   binary feature vectors.
%
%   This is an example helper function that is subject to change or removal 
%   in future releases.

persistent lookupTable; % lookup table for counting bits

N1 = size(features1, 1);
N2 = size(features2, 1);

scores = zeros(N1, N2);

if isempty(lookupTable)
    lookupTable = zeros(256, 1);
    for i = 0:255
        lookupTable(i+1) = sum(dec2bin(i)-'0');
    end
end

for r = 1:N1
    for c = 1:N2
        temp = bitxor(features1(r, :), features2(c, :));
        idx = double(temp) + 1; % cast needed to avoid integer math
        scores(r,c) = sum(lookupTable(idx));
    end
end

end