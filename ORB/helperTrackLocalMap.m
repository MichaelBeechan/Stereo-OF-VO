function [refKeyFrameId, localKeyFrameIds, currPose, mapPointIdx, featureIdx] = ...
    helperTrackLocalMap(mapPoints, directionAndDepth, vSetKeyFrames, mapPointIdx, ...
    featureIdx, currPose, currFeatures, currPoints, intrinsics, scaleFactor, numLevels)
%helperTrackLocalMap Refine camera pose by tracking the local map
%
%   This is an example helper function that is subject to change or removal 
%   in future releases.
%
%   Inputs
%   ------
%   mapPoints         - A worldpointset object storing map points
%   directionAndDepth - A helperDirectionAndDepth object of map point attributes
%   vSetKeyFrames     - An imageviewset storing key frames
%   mapPointsIndices  - Indices of map points observed in the current frame
%   featureIndices    - Indices of features in the current frame 
%                       corresponding to map points denoted by mapPointsIndices                      
%   currPose          - Current camera pose
%   currFeatures      - ORB Features in the current frame 
%   currPoints        - Feature points in the current frame
%   intrinsics        - Camera intrinsics 
%   scaleFactor       - scale factor of features
%   numLevels         - number of levels in feature exatraction
%   
%   Outputs
%   -------
%   mapPoints         - A helperMapPointSet objects storing map points
%   localKeyFrameIds  - ViewIds of the local key frames 
%   currPose          - Refined camera pose of the current frame
%   mapPointIdx       - Indices of map points observed in the current frame
%   featureIdx        - Indices of features in the current frame corresponding
%                       to mapPointIdx   

%   Copyright 2019-2020 The MathWorks, Inc.

[refKeyFrameId, localPointsIndices, localKeyFrameIds] = ...
    updateRefKeyFrameAndLocalPoints(mapPoints, vSetKeyFrames, mapPointIdx);

% Project the map into the frame and search for more map point correspondences
newMapPointIdx = setdiff(localPointsIndices, mapPointIdx, 'stable');
localMapPoints = mapPoints.WorldPoints(newMapPointIdx, :);
localFeatures  = getFeatures(directionAndDepth, vSetKeyFrames.Views, newMapPointIdx); 
[projectedPoints, inlierIndex, predictedScales, viewAngles] = removeOutlierMapPoints(mapPoints, ...
    directionAndDepth, currPose, intrinsics, newMapPointIdx, scaleFactor, numLevels);

newMapPointIdx = newMapPointIdx(inlierIndex);
localMapPoints = localMapPoints(inlierIndex,:);
localFeatures  = localFeatures(inlierIndex,:);

unmatchedfeatureIdx = setdiff(cast((1:size( currFeatures.Features, 1)).', 'uint32'), ...
    featureIdx,'stable');
unmatchedFeatures   = currFeatures.Features(unmatchedfeatureIdx, :);
unmatchedValidPoints= currPoints(unmatchedfeatureIdx);

% Search radius depends on scale and view direction
searchRadius    = 4*ones(size(localFeatures, 1), 1);
searchRadius(viewAngles<3) = 2.5;
searchRadius    = searchRadius.*predictedScales;

indexPairs = matchFeaturesInRadius(binaryFeatures(localFeatures),...
    binaryFeatures(unmatchedFeatures), unmatchedValidPoints, projectedPoints, ...
    searchRadius, 'MatchThreshold', 40, 'MaxRatio', 0.9, 'Unique', true);

% Filter by scales
isGoodScale = currPoints.Scale(indexPairs(:, 2)) >= ...
    max(1, predictedScales(indexPairs(:, 1))/scaleFactor) & ...
    currPoints.Scale(indexPairs(:, 2)) <= predictedScales(indexPairs(:, 1));
indexPairs  = indexPairs(isGoodScale, :);

% Refine camera pose with more 3D-to-2D correspondences
mapPointIdx   = [newMapPointIdx(indexPairs(:,1)); mapPointIdx];
featureIdx     = [unmatchedfeatureIdx(indexPairs(:,2)); featureIdx];
matchedMapPoints   = mapPoints.WorldPoints(mapPointIdx,:);
matchedImagePoints = currPoints.Location(featureIdx,:);

% Refine camera pose only
currPose = bundleAdjustmentMotion(matchedMapPoints, matchedImagePoints, ...
    currPose, intrinsics, 'PointsUndistorted', true, ...
    'AbsoluteTolerance', 1e-7, 'RelativeTolerance', 1e-16,'MaxIteration', 20);
end

function [refKeyFrameId, localPointsIndices, localKeyFrameIds] = ...
    updateRefKeyFrameAndLocalPoints(mapPoints, vSetKeyFrames, pointIndices)

% Get key frames K1 that observe map points in the current key frame
viewIds = findViewsOfWorldPoint(mapPoints, pointIndices);
K1IDs = vertcat(viewIds{:});

% The reference key frame has the most covisible map points 
refKeyFrameId = mode(K1IDs);

% Retrieve key frames K2 that are connected to K1
K1IDs = unique(K1IDs);
localKeyFrameIds = K1IDs;

for i = 1:numel(K1IDs)
    views = connectedViews(vSetKeyFrames, K1IDs(i));
    K2IDs = setdiff(views.ViewId, localKeyFrameIds);
    localKeyFrameIds = [localKeyFrameIds; K2IDs]; %#ok<AGROW>
end

pointIdx = findWorldPointsInView(mapPoints, localKeyFrameIds);
localPointsIndices = sort(vertcat(pointIdx{:}));
end

function features = getFeatures(directionAndDepth, views, mapPointIdx)

% Efficiently retrieve features and image points corresponding to map points
% denoted by mapPointIdx
allIndices = zeros(1, numel(mapPointIdx));

% ViewId and offset pair
count = []; % (ViewId, NumFeatures)
viewsFeatures = views.Features;
majorViewIds  = directionAndDepth.MajorViewId;
majorFeatureindices = directionAndDepth.MajorFeatureIndex;

for i = 1:numel(mapPointIdx)
    index3d  = mapPointIdx(i);
    
    viewId   = double(majorViewIds(index3d));
    
    if isempty(count)
        count = [viewId, size(viewsFeatures{viewId},1)];
    elseif ~any(count(:,1) == viewId)
        count = [count; viewId, size(viewsFeatures{viewId},1)];
    end
    
    idx = find(count(:,1)==viewId);
    
    if idx > 1
        offset = sum(count(1:idx-1,2));
    else
        offset = 0;
    end
    allIndices(i) = majorFeatureindices(index3d) + offset;
end

uIds = count(:,1);

% Concatenating features and indexing once is faster than accessing via a for loop
allFeatures = vertcat(viewsFeatures{uIds});
features    = allFeatures(allIndices, :);
end

function [projectedPoints, inliers, predictedScales, viewAngles] = removeOutlierMapPoints(...
    mapPoints, directionAndDepth, pose, intrinsics, localPointsIndices, scaleFactor, ...
    numLevels)

% 1) Points within the image bounds
xyzPoints = mapPoints.WorldPoints(localPointsIndices, :);
[R, t]    = cameraPoseToExtrinsics(pose.Rotation, pose.Translation);
[projectedPoints, isInImage] = worldToImage(intrinsics, R, t, xyzPoints);

% 2) Parallax less than 60 degrees
cameraNormVector = [0 0 1] * pose.Rotation;
cameraToPoints   = xyzPoints - pose.Translation;
viewDirection    = directionAndDepth.ViewDirection(localPointsIndices, :);
validByView      = sum(viewDirection.*cameraToPoints, 2) > ...
    cosd(60)*(vecnorm(cameraToPoints, 2, 2));

% 3) Distance from map point to camera center is in the range of scale
% invariant depth
minDist          = directionAndDepth.MinDistance(localPointsIndices);
maxDist          = directionAndDepth.MaxDistance(localPointsIndices);
dist             = vecnorm(xyzPoints - pose.Translation, 2, 2);

validByDistance  = dist > minDist & dist < maxDist;

inliers          = isInImage & validByView & validByDistance;

% Predicted scales
level= ceil(log(maxDist ./ dist)./log(scaleFactor));
level(level<0)   = 0;
level(level>=numLevels-1) = numLevels-1;
predictedScales  = scaleFactor.^level;

% View angles
viewAngles       = acosd(sum(cameraNormVector.*cameraToPoints, 2) ./ ...
    vecnorm(cameraToPoints, 2, 2));

predictedScales  = predictedScales(inliers);
viewAngles       = viewAngles(inliers);

projectedPoints = projectedPoints(inliers, :);
end
