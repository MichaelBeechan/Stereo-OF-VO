function [mapPoints, vSetKeyFrames] = helperAddNewKeyFrame(mapPoints, vSetKeyFrames,...
    cameraPose, currFeatures, currPoints, mapPointsIndices, featureIndices, keyFramesIndices)
%helperAddNewKeyFrame add key frames to the key frame set
%
%   This is an example helper function that is subject to change or removal 
%   in future releases.

%   Copyright 2019-2020 The MathWorks, Inc.

viewId = vSetKeyFrames.Views.ViewId(end)+1;

vSetKeyFrames = addView(vSetKeyFrames, viewId, cameraPose,...
    'Features', currFeatures.Features, ...
    'Points', currPoints);

viewsAbsPoses = vSetKeyFrames.Views.AbsolutePose;

for i = 1:numel(keyFramesIndices)
    localKeyFrameId = keyFramesIndices(i);
    [index3d, index2d] = findWorldPointsInView(mapPoints, localKeyFrameId);
    [~, ia, ib] = intersect(index3d, mapPointsIndices, 'stable');
    
    prePose   = viewsAbsPoses(localKeyFrameId);
    relPose = rigid3d(cameraPose.Rotation*prePose.Rotation', ...
        (cameraPose.Translation-prePose.Translation)*prePose.Rotation');
    
    if numel(ia) > 5
        vSetKeyFrames = addConnection(vSetKeyFrames, localKeyFrameId, viewId, relPose, ...
            'Matches', [index2d(ia),featureIndices(ib)]);
    end
end

mapPoints = addCorrespondences(mapPoints, viewId, mapPointsIndices, ...
    featureIndices);
end