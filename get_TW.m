function [temple, xloc1, xloc2, avgheight] = get_TW(img, realdist)
% gets IPD
%	img: frontal face input image
%	temple: temple width in units of pixels

[a, IPD, peaks1, peaks2, peaks3, boxes1, boxes2] = get_IPD1(img, realdist);
imgscale = get_scale(img, realdist)

avgheight = round(((peaks1(2,1)+boxes1(1,2)) + (peaks2(2,1)+boxes2(1,2)))/2);

% facial edge detection
img1 = imread(img);
img1a = img1;
img1 = rgb2gray(img1);
bw = edge(img1,'Canny',0.1);

assignin('base','bw',bw);
assignin('base','img1',img1);

% starting from edges, move inwards towards pupils

Pline = mean(bw(avgheight-2:avgheight+2,:));

count = 0;
xloc1 = size(img1,2);
while count<=1
	xloc1 = xloc1 - 1;
	if Pline(xloc1)>0.3
		count = count + 1;
	end
end

count = 0;
xloc2 = 1;
while count<=1
	xloc2 = xloc2 + 1;
	if Pline(xloc2)>0.3
		count = count + 1;
	end
end

xloc1 = xloc1 - 15;
xloc2 = xloc2 + 15;

% count = 0;
% xloc = peaks1(1,1)+boxes1(1,1)+0.0*IPD;
% while count<=3
% 	disp(xloc);
% 	xloc = xloc + 1;
% 	if Pline(xloc)>0.3
% 		count = count+1;
% 	end
% end
% xloc1 = xloc;

% count = 0;
% xloc = peaks2(1,1)+boxes2(1,1)-0.0*IPD;
% while count<=3
% 	disp(xloc);
% 	xloc = xloc - 1;
% 	if Pline(xloc)>0.3
% 		count = count+1;
% 	end
% end
% xloc2 = xloc;

temple = imgscale*(xloc1 - xloc2);

% % isolate line of pixels at pupil height
% xloc1 = peaks1(1,1)+boxes1(1,1);
% xloc2 = peaks2(1,1)+boxes2(1,1);


assignin('base','Pline',Pline);
% assignin('base','xloc1',xloc1);
% assignin('base','xloc2',xloc2);
% assignin('base','IPD',IPD);

% assignin('base','Plineseg1',Plineseg1);
% assignin('base','Plineseg2',Plineseg2);

% figure(2);
% imshow(img1a,'Border','tight'); zoom(0.5);
% hold on
% [x, y] = circlepoints(peaks1(3));
% plot(x+peaks1(1)+boxes1(1,1), y+peaks1(2)+boxes1(1,2), 'g-');
% [x, y] = circlepoints(peaks2(3));
% plot(x+peaks2(1)+boxes2(1,1), y+peaks2(2)+boxes2(1,2), 'g-');
% [x, y] = circlepoints(peaks3(3));
% plot(x+peaks3(1,1), y+peaks3(2,1), 'g-');
% plot(x+peaks3(1,2), y+peaks3(2,2), 'g-');
% scatter([xloc1 xloc2], avgheight.*[1 1], 20);
% set(gca,'position',[0 0 1 1],'units','normalized')


end