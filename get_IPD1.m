function [imgout, IPD, peaks1, peaks2, peaks3, boxes1, boxes2] = get_IPD1(img, realdist)
% gets IPD
%	img: frontal face input image
%	IPD: inter-pupilary distance in units of pixels

% get scale
imgscale = get_scale(img, realdist)

% initialize Viola-Jones steps
detect_eye1 = vision.CascadeObjectDetector('LeftEye');
detect_eye1.MinSize = [100 100];
detect_eye1.MergeThreshold = 150;

detect_eye2 = vision.CascadeObjectDetector('RightEye');
detect_eye2.MinSize = [100 100];
detect_eye2.MergeThreshold = 150;

% read image
img1 = imread(img);
boxes1 = step(detect_eye1, img1);
boxes2 = step(detect_eye2, img1);

% if can't detect eyes
if size(boxes1,1)<1
	boxes1 = [500 500 100 100];
end
if size(boxes2,1)<1
	boxes2 = [500 500 100 100];
end

% create images with marked features
IFaces1 = insertObjectAnnotation(img1, 'rectangle', boxes1(1,:), 'Left Eye');
IFaces2 = insertObjectAnnotation(IFaces1, 'rectangle', boxes2(1,:), 'Right Eye');

%----------------------------------------------------------------------------------%

% detect eyes
eye1r = [boxes1(1,4), boxes1(1,3)];
eye2r = [boxes2(1,4), boxes2(1,3)];
eye1 = img1(boxes1(1,2):boxes1(1,2)+boxes1(1,4), boxes1(1,1):boxes1(1,1)+boxes1(1,3), :);
eye2 = img1(boxes2(1,2):boxes2(1,2)+boxes2(1,4), boxes2(1,1):boxes2(1,1)+boxes2(1,3), :);
% eye1 = eye1(round((0.2*eye1r(1)):(0.8*eye1r(1))), round((0.2*eye1r(2)):(0.8*eye1r(2))), :);
% eye2 = eye2(round((0.2*eye2r(1)):(0.8*eye2r(1))), round((0.2*eye2r(2)):(0.8*eye2r(2))), :);

% convert to grayscale
eye1a = eye1; eye1 = eye1;
eye1 = rgb2gray(eye1);

eye2a = eye2; eye2 = eye2;
eye2 = rgb2gray(eye2);

face = rgb2gray(img1);

% use a median filter to filter out noise
eye1 = medfilt2(eye1, [2 2]);
eye2 = medfilt2(eye2, [2 2]);

% convert the resulting grayscale image into a binary image.
eye1 = im2bw(eye1,0.08);
eye2 = im2bw(eye2,0.08);

% hough transform
e1 = edge(eye1, 'Canny', 0.5);
e2 = edge(eye2, 'Canny', 0.5);
e3 = edge(face, 'Canny', 0.5);

% where we get schwifty
radii = [10:1:20];
h1 = circle_hough(e1, radii, 'same', 'normalise');
peaks1 = circle_houghpeaks(h1, radii, 'nhoodxy', 15, 'nhoodr', 21, 'npeaks', 1, 'Threshold', 0.5*max(max(max(h1))));

radii = [10:1:20];
h2 = circle_hough(e2, radii, 'same', 'normalise');
peaks2 = circle_houghpeaks(h2, radii, 'nhoodxy', 15, 'nhoodr', 21, 'npeaks', 1, 'Threshold', 0.5*max(max(max(h2))));

radii = [10:1:40];
h3 = circle_hough(e3, radii, 'same', 'normalise');
peaks3 = circle_houghpeaks(h3, radii, 'nhoodxy', 15, 'nhoodr', 21, 'npeaks', 2, 'Threshold', 0.5*max(max(max(h3))));

% if can't detect pupils
if size(peaks1,2)<1
	peaks1 = [100; 100; 10];
end
if size(peaks2,2)<1
	peaks2 = [100; 100; 10];
end
if size(peaks3,2)<1
	peaks3 = [100; 100; 10];
end
assignin('base','IFaces2',IFaces2);

% figure(1);
% imshow(IFaces2,'Border','tight'); zoom(0.5);
% hold on
% [x, y] = circlepoints(peaks1(3));
% plot(x+peaks1(1)+boxes1(1,1), y+peaks1(2)+boxes1(1,2), 'g-');
% [x, y] = circlepoints(peaks2(3));
% plot(x+peaks2(1)+boxes2(1,1), y+peaks2(2)+boxes2(1,2), 'g-');
% [x, y] = circlepoints(peaks3(3));
% plot(x+peaks3(1,1), y+peaks3(2,1), 'g-');
% plot(x+peaks3(1,2), y+peaks3(2,2), 'g-');
% set(gca,'position',[0 0 1 1],'units','normalized')
imgout = 'test';

% calculate IPD value
assignin('base','peaks1',peaks1);
assignin('base','peaks2',peaks2);
assignin('base','boxes1',boxes1);
assignin('base','boxes2',boxes2);

IPD_x = (peaks1(1)+boxes1(1,1)) - (peaks2(1)+boxes2(1,1));
IPD_y = (peaks1(2)+boxes1(1,2)) - (peaks2(2)+boxes2(1,2));
IPD = imgscale*sqrt(IPD_x^2 + IPD_y^2);



end