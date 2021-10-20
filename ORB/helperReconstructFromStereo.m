function [xyzPoints, indexPairs] = helperReconstructFromStereo(I1, I2, ...
    features1, features2, points1, points2, stereoParams, currPose, maxDisparity)

indexPairs     = helperFindValidFeaturePairs(features1, features2, points1, points2, maxDisparity);

%% 视差
% Compute disparity for all pixels in the left image. In practice, it is more 
% common to compute disparity just for the matched feature points.
disparityMap   = disparitySGM(rgb2gray(I1), rgb2gray(I2), 'DisparityRange', [0, maxDisparity]);
xyzPointsAll   = reconstructScene(disparityMap, stereoParams);

% Find the corresponding world point of the matched feature points 
locations      = floor(points1.Location(indexPairs(:, 1), [2 1]));
xyzPoints      = [];
isPointFound   = false(size(points1));

for i = 1:size(locations, 1)
    point3d = squeeze(xyzPointsAll(locations(i,1), locations(i, 2), :))';
    isPointValid   = all(~isnan(point3d)) && all(isfinite(point3d)) &&  point3d(3) > 0;
    isDepthInRange = point3d(3) < 200*norm(stereoParams.TranslationOfCamera2);
    if isPointValid  && isDepthInRange
        xyzPoints       = [xyzPoints; point3d]; %#ok<*AGROW> 
        isPointFound(i) = true;
    end
end
indexPairs = indexPairs(isPointFound, :);
xyzPoints  = xyzPoints * currPose.Rotation + currPose.Translation;
end