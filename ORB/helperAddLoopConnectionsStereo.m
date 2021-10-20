function [isLoopClosed, mapPoints, vSetKeyFrames] = helperAddLoopConnectionsStereo(...
    mapPoints, vSetKeyFrames, loopCandidates, currKeyFrameId, currFeatures, ...
    currPoints, loopEdgeNumMatches)
%helperAddLoopConnectionsStereo add connections between the current key frame and
%   the valid loop candidate key frames. A loop candidate is valid if it has
%   enough covisible map points with the current key frame.

%   This is an example helper function that is subject to change or removal
%   in future releases.

%   Copyright 2019-2020 The MathWorks, Inc.

numCandidates   = size(loopCandidates,1);
loopConnections = [];
[index3d1, index2d1] = findWorldPointsInView(mapPoints, currKeyFrameId);
validFeatures1  = currFeatures.Features(index2d1, :);
validPoints1    = currPoints(index2d1).Location;

for k = 1 : numCandidates
    [index3d2, index2d2] = findWorldPointsInView(mapPoints, loopCandidates(k));
    allFeatures2   = vSetKeyFrames.Views.Features{loopCandidates(k)};
    validFeatures2 = allFeatures2(index2d2, :);
    allPoints2     = vSetKeyFrames.Views.Points{loopCandidates(k)};
    validPoints2   = allPoints2(index2d2);
    
    indexPairs = matchFeatures(binaryFeatures(validFeatures1), binaryFeatures(validFeatures2), ...
        'Unique', true, 'MaxRatio', 0.9, 'MatchThreshold', 40);
    
    % Check if all the candidate key frames have strong connection with the
    % current keyframe
    if size(indexPairs, 1) < loopEdgeNumMatches
        isLoopClosed = false;
        return
    end
    
    % Estimate the relative pose of the current key frame with respect to the
    % loop candidate keyframe with the highest similarity score
    
    worldPoints1 = mapPoints.WorldPoints(index3d1(indexPairs(:, 1)), :);
    worldPoints2 = mapPoints.WorldPoints(index3d2(indexPairs(:, 2)), :);
    
    pose1 = vSetKeyFrames.Views.AbsolutePose(end);
    pose2 = vSetKeyFrames.Views.AbsolutePose(loopCandidates(k));
    [rotation1, translation1] = cameraPoseToExtrinsics(pose1.Rotation, pose1.Translation);
    [rotation2, translation2] = cameraPoseToExtrinsics(pose2.Rotation, pose2.Translation);
    
    worldPoints1InCamera1 = worldPoints1 * rotation1 + translation1;
    worldPoints2InCamera2 = worldPoints2 * rotation2 + translation2;
    
    tform = estimateGeometricTransform3D(worldPoints1InCamera1, ...
        worldPoints2InCamera2, 'rigid');
    
    % Add connection between the current key frame and the loop key frame
    matches = uint32([index2d2(indexPairs(:, 2)), index2d1(indexPairs(:, 1))]);
    vSetKeyFrames = addConnection(vSetKeyFrames, loopCandidates(k), currKeyFrameId, tform, 'Matches', matches);
    disp(['Loop edge added between keyframe: ', num2str(loopCandidates(k)), ' and ', num2str(currKeyFrameId)]);
    
    % Fuse co-visible map points
    matchedIndex3d1 = index3d1(indexPairs(:, 1));
    matchedIndex3d2 = index3d2(indexPairs(:, 2));
    mapPoints = updateWorldPoints(mapPoints, matchedIndex3d1, mapPoints.WorldPoints(matchedIndex3d2, :));
    
    isLoopClosed = true;
end
end