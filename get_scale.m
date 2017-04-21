function [outscale] = get_scale(img, realdist)
% gets the scale

% read image
img1 = imread(img);
face = rgb2gray(img1);

% hough transform
e1 = edge(face, 'Canny', 0.5);

radii = [5:1:50];
h1 = circle_hough(e1, radii, 'same', 'normalise');
peaks1 = circle_houghpeaks(h1, radii, 'nhoodxy', 15, 'nhoodr', 21, 'npeaks', 2, 'Threshold', 0.5*max(max(max(h1))));

assignin('base','e1',e1);

fig = figure(1);
imshow(img1);
hold on
for peak1 = peaks1
    [x, y] = circlepoints(peak1(3));
    plot(x+peak1(1), y+peak1(2), 'g-');
end

% calculate scale
imgdist_x = peaks1(1,1) - peaks1(1,2);
imgdist_y = peaks1(2,1) - peaks1(2,2);
imgdist = sqrt(imgdist_x^2 + imgdist_y^2);

outscale = realdist / imgdist;

end