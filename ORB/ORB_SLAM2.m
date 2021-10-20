%% Function ORBSLAM2
% This function is a main function for ORBSLAM2 
% images Left and Right.
% Author:   MichaelBeechan
% Email:    1252242281@qq.com
% Addr:     MianYang
% Created:  2021.8.25
% Modified: 2021.9.8 
%% 1、Read Images
imdsLeft       = imageDatastore('.\images\left');
imdsRight      = imageDatastore('.\images\right');

% Inspect the first pair of images
currFrameIdx   = 1;     % 第一帧图像
currILeft      = readimage(imdsLeft, currFrameIdx);
currIRight     = readimage(imdsRight, currFrameIdx);
% 显示一个图像对
figure(1);
imshowpair(currILeft, currIRight, 'montage');

% Set random seed for reproducibility
%rng(0);

%% 初始化相机位姿
% Load the initial camera pose. The initial camera pose is derived based 
% on the transformation between the camera and the vehicle:
% http://asrl.utias.utoronto.ca/datasets/2020-vtr-dataset/text_files/transform_camera_vehicle.tx 
initialPoseData = load('initialPose.mat');
initialPose     = initialPoseData.initialPose;

%% 相机内外参数
% Create a stereoParameters object to store the stereo camera parameters.
% The intrinsics for the dataset can be found at the following page:
% http://asrl.utias.utoronto.ca/datasets/2020-vtr-dataset/text_files/camera_parameters.txt
focalLength     = [387.777 387.777];     % specified in pixels
principalPoint  = [257.446 197.718];     % specified in pixels [x, y]

% 左右相机基线长度
baseline        = 0.239965;              % specified in meters
intrinsicMatrix = [focalLength(1), 0, 0; ...
    0, focalLength(2), 0; ...
    principalPoint(1), principalPoint(2), 1];
imageSize       = size(currILeft,[1,2]); % in pixels [mrows, ncols]
% 获取cameraParameters返回相机的内参、外参及畸变参数
cameraParam     = cameraParameters('IntrinsicMatrix', intrinsicMatrix, 'ImageSize', imageSize);
intrinsics      = cameraParam.Intrinsics;
stereoParams    = stereoParameters(cameraParam, cameraParam, eye(3), [-baseline, 0 0]);

% In this example, the images are already undistorted. In a general
% workflow, uncomment the following code to undistort the images.
% currILeft  = undistortImage(currILeft, intrinsics);
% currIRight = undistortImage(currIRight, intrinsics);

% Rectify the stereo images 校正
[currILeft, currIRight] = rectifyStereoImages(currILeft, currIRight, stereoParams, 'OutputView','full');

% Detect and extract ORB features from the rectified stereo images
scaleFactor = 1.2;
numLevels   = 8;
[currFeaturesLeft,  currPointsLeft]   = helperDetectAndExtractFeatures(currILeft, scaleFactor, numLevels); 
[currFeaturesRight, currPointsRight]  = helperDetectAndExtractFeatures(currIRight, scaleFactor, numLevels);

%% Optical Tracking  opticalflowLK
%% Feature points detect  20210906
%  pointImage = insertMarker(currILeft,currPointsLeft.Location,'+','Color','white');
%  figure(2);
%  imshow(pointImage);
%  title('Detected interest points');
%   See also detectBRISKFeatures, detectHarrisFeatures, detectFASTFeatures,
%          detectMinEigenFeatures, detectSURFFeatures, detectMSERFeatures,
%          detectKAZEFeatures, detectORBFeatures,

% I = im2gray(currILeft);
% corners = detectSURFFeatures(I);
% corners2 = detectHarrisFeatures(I);
% corners3 = detectFASTFeatures(I);
% corners4 = detectBRISKFeatures(I);
% corners5 = detectORBFeatures(I);
% corners6 = detectKAZEFeatures(I);
% corners7 = detectMinEigenFeatures(I);
% figure(1)
% imshow(I); hold on;
% plot(corners.selectStrongest(50));
% figure(2)
% imshow(I); hold on;
% plot(corners2.selectStrongest(50));
% figure(3)
% imshow(I); hold on;
% plot(corners3.selectStrongest(50));
% figure(4)
% imshow(I); hold on;
% plot(corners4.selectStrongest(50));
% figure(5)
% imshow(I); hold on;
% plot(corners5.selectStrongest(50));
% figure(6)
% imshow(I); hold on;
% plot(corners6.selectStrongest(50));
% figure(7)
% imshow(I); hold on;
% plot(corners7.selectStrongest(50));


% Match feature points between the stereo images and get the 3-D world positions 
maxDisparity = 48;  % specified in pixels
[xyzPoints, matchedPairs] = helperReconstructFromStereo(currILeft, currIRight, ...
    currFeaturesLeft, currFeaturesRight, currPointsLeft, currPointsRight, stereoParams, initialPose, maxDisparity);
% Create an empty imageviewset object to store key frames
vSetKeyFrames = imageviewset;

% Create an empty worldpointset object to store 3-D map points
mapPointSet   = worldpointset;

% Create a helperViewDirectionAndDepth object to store view direction and depth 
directionAndDepth = helperViewDirectionAndDepth(size(xyzPoints, 1));

% Add the first key frame
currKeyFrameId = 1;
vSetKeyFrames = addView(vSetKeyFrames, currKeyFrameId, initialPose, 'Points', currPointsLeft,...
    'Features', currFeaturesLeft.Features);

% Add 3-D map points
[mapPointSet, stereoMapPointsIdx] = addWorldPoints(mapPointSet, xyzPoints);

% Add observations of the map points
mapPointSet = addCorrespondences(mapPointSet, currKeyFrameId, stereoMapPointsIdx, matchedPairs(:, 1));

% Visualize matched features in the first key frame
featurePlot = helperVisualizeMatchedFeaturesStereo(currILeft, currIRight, currPointsLeft, ...
    currPointsRight, matchedPairs);

% Visualize initial map points and camera trajectory
mapPlot     = helperVisualizeMotionAndStructureStereo(vSetKeyFrames, mapPointSet);

% Show legend
showLegend(mapPlot);

%% Tracking
% ViewId of the last key frame
lastKeyFrameId    = currKeyFrameId;

% ViewId of the reference key frame that has the most co-visible 
% map points with the current key frame
refKeyFrameId     = currKeyFrameId;

% Index of the last key frame in the input image sequence
lastKeyFrameIdx   = currFrameIdx; 

% Indices of all the key frames in the input image sequence
addedFramesIdx    = lastKeyFrameIdx;

currFrameIdx      = 2;
isLoopClosed      = false;

%% 20210906
% tracker = vision.PointTracker('NumPyramidLevels',3);
% initialize(tracker,currPointsLeft.Location,currILeft);

% Main loop
while currFrameIdx < numel(imdsLeft.Files)

    currILeft  = readimage(imdsLeft, currFrameIdx);
    currIRight = readimage(imdsRight, currFrameIdx);
    [currILeft, currIRight] = rectifyStereoImages(currILeft, currIRight, stereoParams, 'OutputView','full');

    [currFeaturesLeft, currPointsLeft]    = helperDetectAndExtractFeatures(currILeft, scaleFactor, numLevels);
    [currFeaturesRight, currPointsRight]  = helperDetectAndExtractFeatures(currIRight, scaleFactor, numLevels);

        
    %% 20210906
%     [points,validity] = step(tracker, currILeft); 
%     [points,validity] = tracker(currILeft);
%     out = insertMarker(currILeft, points(validity, :),'+');
%     figure(8);
%     imshow(out);
    
    % Track the last key frame
    % trackedMapPointsIdx:  Indices of the map points observed in the current left frame 
    % trackedFeatureIdx:    Indices of the corresponding feature points in the current left frame
    [currPose, trackedMapPointsIdx, trackedFeatureIdx] = helperTrackLastKeyFrame(mapPointSet, ...
        vSetKeyFrames.Views, currFeaturesLeft, currPointsLeft, lastKeyFrameId, intrinsics, scaleFactor);
    
    if isempty(currPose) || numel(trackedMapPointsIdx) < 30
        currFrameIdx = currFrameIdx + 1;
        continue
    end
    
    % Track the local map
    % refKeyFrameId:      ViewId of the reference key frame that has the most 
    %                     co-visible map points with the current frame
    % localKeyFrameIds:   ViewId of the connected key frames of the current frame
    if currKeyFrameId == 1
        refKeyFrameId    = 1;
        localKeyFrameIds = 1;
    else
        [refKeyFrameId, localKeyFrameIds, currPose, trackedMapPointsIdx, trackedFeatureIdx] = ...
            helperTrackLocalMap(mapPointSet, directionAndDepth, vSetKeyFrames, trackedMapPointsIdx, ...
            trackedFeatureIdx, currPose, currFeaturesLeft, currPointsLeft, intrinsics, scaleFactor, numLevels);
    end
    
    % Match feature points between the stereo images and get the 3-D world positions
    [xyzPoints, matchedPairs] = helperReconstructFromStereo(currILeft, currIRight, currFeaturesLeft, ...
        currFeaturesRight, currPointsLeft, currPointsRight, stereoParams, currPose, maxDisparity);
        
    [untrackedFeatureIdx, ia] = setdiff(matchedPairs(:, 1), trackedFeatureIdx);
    xyzPoints = xyzPoints(ia, :);
    
    % Check if the current frame is a key frame
    isKeyFrame = helperIsKeyFrame(mapPointSet, refKeyFrameId, lastKeyFrameIdx, ...
        currFrameIdx, trackedMapPointsIdx);

    % Visualize matched features in the stereo image
    updatePlot(featurePlot, currILeft, currIRight, currPointsLeft, currPointsRight, trackedFeatureIdx, matchedPairs);
    
    if ~isKeyFrame
        currFrameIdx = currFrameIdx + 1;
        continue
    end
    
    % Update current key frame ID
    currKeyFrameId  = currKeyFrameId + 1;
    
    %% Local Mapping
    % Add the new key frame    
    [mapPointSet, vSetKeyFrames] = helperAddNewKeyFrame(mapPointSet, vSetKeyFrames, ...
        currPose, currFeaturesLeft, currPointsLeft, trackedMapPointsIdx, trackedFeatureIdx, localKeyFrameIds);
        
    % Remove outlier map points that are observed in fewer than 3 key frames
    if currKeyFrameId == 2
        triangulatedMapPointsIdx = [];
    end
    
    [mapPointSet, directionAndDepth, trackedMapPointsIdx] = ...
        helperCullRecentMapPoints(mapPointSet, directionAndDepth, trackedMapPointsIdx, triangulatedMapPointsIdx, ...
        stereoMapPointsIdx);
    
    % Add new map points computed from disparity 
    [mapPointSet, stereoMapPointsIdx] = addWorldPoints(mapPointSet, xyzPoints);
    mapPointSet = addCorrespondences(mapPointSet, currKeyFrameId, stereoMapPointsIdx, ...
        untrackedFeatureIdx);
    
    % Create new map points by triangulation
    minNumMatches = 10;
    minParallax   = 0.35;
    [mapPointSet, vSetKeyFrames, triangulatedMapPointsIdx, stereoMapPointsIdx] = helperCreateNewMapPointsStereo( ...
        mapPointSet, vSetKeyFrames, currKeyFrameId, intrinsics, scaleFactor, minNumMatches, minParallax, ...
        untrackedFeatureIdx, stereoMapPointsIdx);
    
    % Update view direction and depth
    directionAndDepth = update(directionAndDepth, mapPointSet, vSetKeyFrames.Views, ...
        [trackedMapPointsIdx; triangulatedMapPointsIdx; stereoMapPointsIdx], true);
    
    % Local bundle adjustment
    [mapPointSet, directionAndDepth, vSetKeyFrames, triangulatedMapPointsIdx, stereoMapPointsIdx] = ...
        helperLocalBundleAdjustmentStereo(mapPointSet, directionAndDepth, vSetKeyFrames, ...
        currKeyFrameId, intrinsics, triangulatedMapPointsIdx, stereoMapPointsIdx); 
    
    % Visualize 3-D world points and camera trajectory
    updatePlot(mapPlot, vSetKeyFrames, mapPointSet);
    
    %% Loop Closure
    % Initialize the loop closure database
    if currKeyFrameId == 2
        % Load the bag of features data created offline
        bofData         = load('bagOfFeaturesUTIAS.mat');
        
        % Initialize the place recognition database
        loopCandidates  = [1; 2];
        loopDatabase    = indexImages(subset(imdsLeft, loopCandidates), bofData.bof);
       
    % Check loop closure after some key frames have been created    
    elseif currKeyFrameId > 50
        
        % Minimum number of feature matches of loop edges
        loopEdgeNumMatches = 40;
        
        % Detect possible loop closure key frame candidates
        [isDetected, validLoopCandidates] = helperCheckLoopClosure(vSetKeyFrames, currKeyFrameId, ...
            loopDatabase, currILeft, loopCandidates, loopEdgeNumMatches);
        
        isTooCloseView = currKeyFrameId - max(validLoopCandidates) < 20;
        if isDetected && ~isTooCloseView
            % Add loop closure connections
            [isLoopClosed, mapPointSet, vSetKeyFrames] = helperAddLoopConnectionsStereo(...
                mapPointSet, vSetKeyFrames, validLoopCandidates, currKeyFrameId, ...
                currFeaturesLeft, currPointsLeft, loopEdgeNumMatches);
        end
    end
    
    % If no loop closure is detected, add the image into the database
    if ~isLoopClosed
        addImages(loopDatabase, subset(imdsLeft, currFrameIdx), 'Verbose', false);
        loopCandidates= [loopCandidates; currKeyFrameId];  
    end
    
    % Update IDs and indices
    lastKeyFrameId  = currKeyFrameId;
    lastKeyFrameIdx = currFrameIdx;
    addedFramesIdx  = [addedFramesIdx; currFrameIdx]; 
    currFrameIdx    = currFrameIdx + 1;
end % End of main loop

%% 
% Optimize the poses
minNumMatches = 10;
vSetKeyFramesOptim = optimizePoses(vSetKeyFrames, minNumMatches, 'Tolerance', 1e-16);

% Update map points after optimizing the poses
mapPointSet = helperUpdateGlobalMap(mapPointSet, directionAndDepth, vSetKeyFrames, vSetKeyFramesOptim);

updatePlot(mapPlot, vSetKeyFrames, mapPointSet);

% Plot the optimized camera trajectory
optimizedPoses  = poses(vSetKeyFramesOptim);
plotOptimizedTrajectory(mapPlot, optimizedPoses)

% Update legend
showLegend(mapPlot);

%% Compare with the Ground Truth
% Load GPS data
gpsData     = load('gpsLocation.mat');
gpsLocation = gpsData.gpsLocation;

% Transform GPS locations to the reference coordinate system
gTruth = helperTransformGPSLocations(gpsLocation, optimizedPoses);

% Plot the GPS locations
plotActualTrajectory(mapPlot, gTruth(addedFramesIdx, :));

% Show legend
showLegend(mapPlot);


