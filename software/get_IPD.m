function [IPD] = get_IPD(img)
% gets IPD
%	img: frontal face input image
%	IPD: inter-pupilary distance in units of pixels

% initialize Viola-Jones steps
detect_eye1 = vision.CascadeObjectDetector('LeftEye');
detect_eye1.MinSize = [100 100];
detect_eye1.MergeThreshold = 100;

detect_eye2 = vision.CascadeObjectDetector('RightEye');
detect_eye2.MinSize = [100 100];
detect_eye2.MergeThreshold = 100;

detect_nose = vision.CascadeObjectDetector('Nose');
detect_nose.MinSize = [100 100];
detect_nose.MergeThreshold = 100;

% read image
img1 = imread(img);
img1 = imresize(img1, 0.5);
boxes1 = step(detect_eye1, img1);
boxes2 = step(detect_eye2, img1);
boxes3 = step(detect_nose, img1);

% create images with marked features
IFaces1 = insertObjectAnnotation(img1, 'rectangle', boxes1, 'Eye');
IFaces2 = insertObjectAnnotation(IFaces1, 'rectangle', boxes2, 'Eye');
IFaces3 = insertObjectAnnotation(IFaces2, 'rectangle', boxes3, 'Nose');

% figure(1); clf;
% imshow(IFaces3);
% title('Detected features');

%----------------------------------------------------------------------------------%

% detect eyes
eye1 = img1(boxes1(1,2):boxes1(1,2)+boxes1(1,4), boxes1(1,1):boxes1(1,1)+boxes1(1,3), :);
eye2 = img1(boxes2(1,2):boxes2(1,2)+boxes2(1,4), boxes2(1,1):boxes2(1,1)+boxes2(1,3), :);

% convert to grayscale
eye1a = eye1; eye1 = eye1;
eye1 = rgb2gray(eye1);

eye2a = eye2; eye2 = eye2;
eye2 = rgb2gray(eye2);

% use a median filter to filter out noise
eye1 = medfilt2(eye1, [3 3]);
eye2 = medfilt2(eye2, [3 3]);

% convert the resulting grayscale image into a binary image.
eye1 = im2bw(eye1,0.17);
eye2 = im2bw(eye2,0.17);

% hough transform
e1 = edge(eye1, 'canny');
e2 = edge(eye2, 'canny');

radii = [5:1:40];
h1 = circle_hough(e1, radii, 'same', 'normalise');
peaks1 = circle_houghpeaks(h1, radii, 'nhoodxy', 15, 'nhoodr', 21, 'npeaks', 10);
peaks1 = peaks1(:,1);

radii = [8:1:40];
h2 = circle_hough(e2, radii, 'same', 'normalise');
peaks2 = circle_houghpeaks(h2, radii, 'nhoodxy', 15, 'nhoodr', 21, 'npeaks', 10);
peaks2 = peaks2(:,1);

% figure(2); clf;
% imshow(eye1a);
% hold on;
% for peak = peaks
%     [x, y] = circlepoints(peak(3));
%     plot(x+peak(1), y+peak(2), 'g-');
% end
% hold off

figure(3); clf;
imshow(IFaces3);
hold on;
for peak1 = peaks1
    [x, y] = circlepoints(peak1(3));
    plot(x+peak1(1)+boxes1(1,1), y+peak1(2)+boxes1(1,2), 'g-');
end
for peak2 = peaks2
    [x, y] = circlepoints(peak2(3));
    plot(x+peak2(1)+boxes2(1,1), y+peak2(2)+boxes2(1,2), 'g-');
end

IPD_x = (peak1(1)+boxes1(1,1)) - (peak2(1)+boxes2(1,1));
IPD_y = (peak1(2)+boxes1(1,2)) - (peak2(2)+boxes2(1,2));
IPD = sqrt(IPD_x^2 + IPD_y^2);

end