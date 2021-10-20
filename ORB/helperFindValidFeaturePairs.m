function indexPairs = helperFindValidFeaturePairs(features1, features2, points1, points2, maxDisparity)
indexPairs  = matchFeatures(features1, features2,...
    'Unique', true, 'MaxRatio', 1, 'MatchThreshold', 40);

matchedPoints1 = points1.Location(indexPairs(:,1), :);
matchedPoints2 = points2.Location(indexPairs(:,2), :);
scales1        = points1.Scale(indexPairs(:,1), :);
scales2        = points2.Scale(indexPairs(:,2), :);

dist2EpipolarLine = abs(matchedPoints2(:, 2) - matchedPoints1(:, 2));
shiftDist = matchedPoints1(:, 1) - matchedPoints2(:, 1);

isCloseToEpipolarline = dist2EpipolarLine < 2*scales2;
isDisparityValid      = shiftDist > 0 & shiftDist < maxDisparity;
isScaleIdentical      = scales1 == scales2;
indexPairs = indexPairs(isCloseToEpipolarline & isDisparityValid & isScaleIdentical, :);
end