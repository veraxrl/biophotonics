function [out] = get_manual(inmesh, front_img, dist_2d)

% determine automatic measurements
img1 = imread(front_img);
[temp, IPD, peaks1, peaks2, peaks3, boxes1, boxes2] = get_IPD1(front_img, dist_2d);
[TW, TW_x1, TW_x2, TW_avgheight] = get_TW(front_img, dist_2d);
meshscale = 10;
out = [0 0 0 0 0];
guiopen = 1;

% visualize the mesh
figure(1); clf;
set(gcf, 'units', 'normalized', 'OuterPosition', [0.05 0.1 0.4 0.8])
ptCloud = pcread(inmesh);
pcshow(ptCloud,'MarkerSize',10);

% create the measurement GUI
glob = struct();

gui = struct();
gui.window = figure();
set(gcf, 'units', 'normalized', 'OuterPosition', [0.55 0.1 0.4 0.8])
mainlayout = uiextras.HBox('Parent', gui.window);

a = uiextras.VBox('Parent',mainlayout);
b = uiextras.VBox('Parent',mainlayout);

a0 = uiextras.BoxPanel('Parent',a,'Title',' Frontal Picture');
a0a = axes('Parent',a0,'OuterPosition',[0 0 1 1]);

fig = figure(3); clf;

imshow(img1,'Border','tight');
hold on
[x, y] = circlepoints(peaks1(3));
plot(x+peaks1(1)+boxes1(1,1), y+peaks1(2)+boxes1(1,2), 'g-');
[x, y] = circlepoints(peaks2(3));
plot(x+peaks2(1)+boxes2(1,1), y+peaks2(2)+boxes2(1,2), 'g-');
[x, y] = circlepoints(peaks3(3));
plot(x+peaks3(1,1), y+peaks3(2,1), 'g-');
plot(x+peaks3(1,2), y+peaks3(2,2), 'g-');
scatter([TW_x1 TW_x2], TW_avgheight.*[1 1], 10, 'r','filled');
clear('x','y');

set(fig.CurrentAxes,'XTick',[],'YTick',[]);
a0a = fig.CurrentAxes;
set(a0a,'Parent',a0);
close(fig);

a4 = uiextras.BoxPanel('Parent',a,'Title',' Input Coordinates for Point 1');
a4a = uiextras.HBox('Parent',a4);
a4a1 = uicontrol('Parent',a4a,'style','edit');
a4a2 = uicontrol('Parent',a4a,'style','edit');
a4a3 = uicontrol('Parent',a4a,'style','edit');
a4a4 = uicontrol('Parent',a4a,'style','push','string','get scale','callback',@ongetscale);
a4a5 = uicontrol('Parent',a4a,'style','edit','String',num2str(meshscale),'BackgroundColor','yellow');
set(a4a,'Sizes',[-1 -1 -1 -1 -1]);

a1 = uiextras.BoxPanel('Parent',a,'Title',' Input Coordinates for Point 1');
a1a = uiextras.HBox('Parent',a1);
a1a1 = uicontrol('Parent',a1a,'style','edit');
a1a2 = uicontrol('Parent',a1a,'style','edit');
a1a3 = uicontrol('Parent',a1a,'style','edit');
set(a1a,'Sizes',[-1 -1 -1]);

a2 = uiextras.BoxPanel('Parent',a,'Title',' Input Coordinates for Point 2');
a2a = uiextras.HBox('Parent',a2);
a2a1 = uicontrol('Parent',a2a,'style','edit');
a2a2 = uicontrol('Parent',a2a,'style','edit');
a2a3 = uicontrol('Parent',a2a,'style','edit');
set(a2a,'Sizes',[-1 -1 -1]);

a3 = uiextras.HBox('Parent',a);
a3a = uicontrol('Parent',a3,'style','push','string','IPD','BackgroundColor','cyan','callback',@onconfirm1);
a3b = uicontrol('Parent',a3,'style','push','string','Temple Width','BackgroundColor','cyan','callback',@onconfirm2);
a3c = uicontrol('Parent',a3,'style','push','string','Nose Bridge','BackgroundColor','cyan','callback',@onconfirm3);
a3d = uicontrol('Parent',a3,'style','push','string','Eye Size','BackgroundColor','cyan','callback',@onconfirm4);
a3e = uicontrol('Parent',a3,'style','push','string','Custom','BackgroundColor','cyan','callback',@onconfirm5);

set(a,'Sizes',[-25 -3 -3 -3 -2]);

b1 = uicontrol('Parent',b,'style','list');
temp = {['IPD: ' num2str(IPD)],['TW: ' num2str(TW)],'NBW: NA','ES: NA','Custom: NA'};
b1string = 'Measurements';
for i = 1:4
	b1string = sprintf('%s \n%s',b1string, temp{i});
end
set(b1,'string',b1string);
b2 = uicontrol('Parent',b,'style','push','string','close GUI','BackgroundColor','red','callback',@onclose);

set(b,'Sizes',[-30 -1]);
set(mainlayout,'Sizes',[-4 -1]);

status = 0; rcounter = 0;
while guiopen==1
	b1string = 'Measurements';
	for i = 1:5
		b1string = sprintf('%s \n%s',b1string, temp{i});
	end
	set(b1,'string',b1string);
	set(a4a5,'string',num2str(meshscale));
	disp(status);

	while status==0
		pause(0.1);
	end
	status = 0;
end

close all

%-----------------------------------------------------------------------------%
function onconfirm1(varargin)
% on point confirmation

	x1(1) = str2num(get(a1a1,'string'));
	x1(2) = str2num(get(a1a2,'string'));
	x1(3) = str2num(get(a1a3,'string'));
	x2(1) = str2num(get(a2a1,'string'));
	x2(2) = str2num(get(a2a2,'string'));
	x2(3) = str2num(get(a2a3,'string'));
	rcounter = rcounter + 1;

	temp{1} = ['IPD: ' num2str(meshscale*sqrt(sum((x1-x2).^2)))];
	out(1) = meshscale*sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function onconfirm2(varargin)
% on point confirmation

	x1(1) = str2num(get(a1a1,'string'));
	x1(2) = str2num(get(a1a2,'string'));
	x1(3) = str2num(get(a1a3,'string'));
	x2(1) = str2num(get(a2a1,'string'));
	x2(2) = str2num(get(a2a2,'string'));
	x2(3) = str2num(get(a2a3,'string'));
	rcounter = rcounter + 1;

	temp{2} = ['TW: ' num2str(meshscale*sqrt(sum((x1-x2).^2)))];
	out(2) = meshscale*sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function onconfirm3(varargin)
% on point confirmation

	x1(1) = str2num(get(a1a1,'string'));
	x1(2) = str2num(get(a1a2,'string'));
	x1(3) = str2num(get(a1a3,'string'));
	x2(1) = str2num(get(a2a1,'string'));
	x2(2) = str2num(get(a2a2,'string'));
	x2(3) = str2num(get(a2a3,'string'));
	rcounter = rcounter + 1;

	temp{3} = ['NBW: ' num2str(meshscale*sqrt(sum((x1-x2).^2)))];
	out(3) = meshscale*sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function onconfirm4(varargin)
% on point confirmation

	x1(1) = str2num(get(a1a1,'string'));
	x1(2) = str2num(get(a1a2,'string'));
	x1(3) = str2num(get(a1a3,'string'));
	x2(1) = str2num(get(a2a1,'string'));
	x2(2) = str2num(get(a2a2,'string'));
	x2(3) = str2num(get(a2a3,'string'));
	rcounter = rcounter + 1;

	temp{4} = ['ES: ' num2str(meshscale*sqrt(sum((x1-x2).^2)))];
	out(4) = meshscale*sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function onconfirm5(varargin)
% on point confirmation

	x1(1) = str2num(get(a1a1,'string'));
	x1(2) = str2num(get(a1a2,'string'));
	x1(3) = str2num(get(a1a3,'string'));
	x2(1) = str2num(get(a2a1,'string'));
	x2(2) = str2num(get(a2a2,'string'));
	x2(3) = str2num(get(a2a3,'string'));
	rcounter = rcounter + 1;

	temp{5} = ['Custom: ' num2str(meshscale*sqrt(sum((x1-x2).^2)))];
	out(5) = meshscale*sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function ongetscale(varargin)
% on get scale

	temp1 = strsplit(get(a4a1,'string'));
	temp2 = strsplit(get(a4a2,'string'));
	for i = 1:3
		x1(i) = str2num(temp1{i});
		x2(i) = str2num(temp2{i});
	end
	x3 = str2num(get(a4a3,'string'));
	meshscale = x3 / sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function onclose(varargin)
% on close
	guiopen = 0;
	status = 1;
end

end