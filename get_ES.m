function [imgout, IPD] = get_ES(img, realdist)
% gets IPD
%	img: frontal face input image
%	IPD: inter-pupilary distance in units of pixels

% get scale
imgscale = get_scale(img, realdist)

% initialize Viola-Jones steps
detect_eye1 = vision.CascadeObjectDetector('LeftEye');
detect_eye1.MinSize = [100 100];
detect_eye1.MergeThreshold = 100;

detect_eye2 = vision.CascadeObjectDetector('RightEye');
detect_eye2.MinSize = [100 100];
detect_eye2.MergeThreshold = 100;

% read image
img1 = imread(img);
img1 = imresize(img1, 0.5);
boxes1 = step(detect_eye1, img1);
boxes2 = step(detect_eye2, img1);

% create images with marked features
IFaces1 = insertObjectAnnotation(img1, 'rectangle', boxes1(1,:), 'Eye');
IFaces2 = insertObjectAnnotation(IFaces1, 'rectangle', boxes2(1,:), 'Eye');

%----------------------------------------------------------------------------------%

% detect eyes
eye1r = [boxes1(1,4), boxes1(1,3)];
eye2r = [boxes2(1,4), boxes2(1,3)];
eye1 = img1(boxes1(1,2):boxes1(1,2)+boxes1(1,4), boxes1(1,1):boxes1(1,1)+boxes1(1,3), :);
eye2 = img1(boxes2(1,2):boxes2(1,2)+boxes2(1,4), boxes2(1,1):boxes2(1,1)+boxes2(1,3), :);
eye1 = eye1(round((0.2*eye1r(1)):(0.8*eye1r(1))), round((0.2*eye1r(2)):(0.8*eye1r(2))), :);
eye2 = eye2(round((0.2*eye2r(1)):(0.8*eye2r(1))), round((0.2*eye2r(2)):(0.8*eye2r(2))), :);

% convert to grayscale
eye1a = eye1; eye1 = eye1;
eye1 = rgb2gray(eye1);
eye1b = imadjust(eye1);

eye2a = eye2; eye2 = eye2;
eye2 = rgb2gray(eye2);

face = rgb2gray(img1);

% % use a median filter to filter out noise
% eye1 = medfilt2(eye1, [2 2]);
% eye2 = medfilt2(eye2, [2 2]);

% % convert the resulting grayscale image into a binary image.
% eye1 = im2bw(eye1,0.3);
% eye2 = im2bw(eye2,0.3);

% hough transform
e1 = edge(eye1, 'Canny', 0.1);
e2 = edge(eye2, 'Canny', 0.1);
e3 = edge(face, 'Canny', 0.1);

assignin('base','eye1',eye1);
assignin('base','eye1b',eye1b);
assignin('base','e1',e1);

% where we get schwifty
radii = [7:0.5:10];
h1 = circle_hough(e1, radii, 'same', 'normalise');
peaks1 = circle_houghpeaks(h1, radii, 'nhoodxy', 15, 'nhoodr', 21, 'npeaks', 1, 'Threshold', 0.5*max(max(max(h1))));

radii = [7:0.5:10];
h2 = circle_hough(e2, radii, 'same', 'normalise');
peaks2 = circle_houghpeaks(h2, radii, 'nhoodxy', 15, 'nhoodr', 21, 'npeaks', 1, 'Threshold', 0.5*max(max(max(h2))));

radii = [5:0.5:20]
h3 = circle_hough(e3, radii, 'same', 'normalise');
peaks3 = circle_houghpeaks(h3, radii, 'nhoodxy', 15, 'nhoodr', 21, 'npeaks', 2, 'Threshold', 0.5*max(max(max(h3))));


fig = figure(1);
imshow(IFaces2);
hold on
for peak1 = peaks1
    [x, y] = circlepoints(peak1(3));
    plot(x+peak1(1)+boxes1(1,1), y+peak1(2)+boxes1(1,2), 'g-');
end
for peak2 = peaks2
    [x, y] = circlepoints(peak2(3));
    plot(x+peak2(1)+boxes2(1,1), y+peak2(2)+boxes2(1,2), 'g-');
end
for peak3 = peaks3
    [x, y] = circlepoints(peak3(3));
    plot(x+peak3(1), y+peak3(2), 'g-');
end
imgout = print('-RGBImage');

% calculate IPD value

IPD_x = (peak1(1)+boxes1(1,1)) - (peak2(1)+boxes2(1,1));
IPD_y = (peak1(2)+boxes1(1,2)) - (peak2(2)+boxes2(1,2));
IPD = realdist * sqrt(IPD_x^2 + IPD_y^2);

end