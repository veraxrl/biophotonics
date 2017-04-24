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
set(gcf, 'units', 'normalized', 'OuterPosition', [0.05 0.05 0.45 0.9])
ptCloud = pcread(inmesh);
pcshow(ptCloud,'MarkerSize',10);

% create the measurement GUI
gui = struct();
gui.window = figure();
set(gcf, 'units', 'normalized', 'OuterPosition', [0.5 0.05 0.45 0.9])
mainlayout = uiextras.VBox('Parent', gui.window);

a = uiextras.BoxPanel('Parent',mainlayout);
a0 = axes('Parent',a,'OuterPosition',[0 0 1 1]);

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
a0 = fig.CurrentAxes;
set(a0,'Parent',a);
close(fig);

b = uiextras.HBox('Parent',mainlayout);
b1 = uiextras.VBox('Parent',b);

b1ax = uicontrol('Parent',b1,'style','edit','string','Scaling Points','BackgroundColor',[15 93 48]/255,'ForegroundColor',[1 1 1]);
b1a1 = uiextras.HBox('Parent',b1);
b1a1a = uicontrol('Parent',b1a1,'style','edit');
b1a1b = uicontrol('Parent',b1a1,'style','edit');
b1a1c = uicontrol('Parent',b1a1,'style','edit');
b1a1d = uicontrol('Parent',b1a1,'style','push','string','get scale','callback',@ongetscale);
b1a1e = uicontrol('Parent',b1a1,'style','edit','String',num2str(meshscale),'BackgroundColor',[173 223 173]/255,'ForegroundColor',[0 0 0]);
set(b1a1,'Sizes',[-1 -1 -1 -1 -1]);

b1bx = uicontrol('Parent',b1,'style','edit','string','Input Point 1','BackgroundColor',[15 93 48]/255,'ForegroundColor',[1 1 1]);
b1b = uiextras.HBox('Parent',b1);
b1b1 = uicontrol('Parent',b1b,'style','edit');
b1b2 = uicontrol('Parent',b1b,'style','edit');
b1b3 = uicontrol('Parent',b1b,'style','edit');
set(b1b,'Sizes',[-1 -1 -1]);

b1cx = uicontrol('Parent',b1,'style','edit','string','Input Point 2','BackgroundColor',[15 93 48]/255,'ForegroundColor',[1 1 1]);
b1c = uiextras.HBox('Parent',b1);
b1c1 = uicontrol('Parent',b1c,'style','edit');
b1c2 = uicontrol('Parent',b1c,'style','edit');
b1c3 = uicontrol('Parent',b1c,'style','edit');
set(b1c,'Sizes',[-1 -1 -1]);

b1d = uiextras.HBox('Parent',b1);
b1d1 = uicontrol('Parent',b1d,'style','push','string','IPD','BackgroundColor',[173 223 173]/255,'ForegroundColor',[0 0 0],'callback',@onconfirm1);
b1d2 = uicontrol('Parent',b1d,'style','push','string','Temple Width','BackgroundColor',[173 223 173]/255,'ForegroundColor',[0 0 0],'callback',@onconfirm2);
b1d3 = uicontrol('Parent',b1d,'style','push','string','Nose Bridge','BackgroundColor',[173 223 173]/255,'ForegroundColor',[0 0 0],'callback',@onconfirm3);
b1d4 = uicontrol('Parent',b1d,'style','push','string','Eye Size','BackgroundColor',[173 223 173]/255,'ForegroundColor',[0 0 0],'callback',@onconfirm4);
b1d5 = uicontrol('Parent',b1d,'style','push','string','Custom','BackgroundColor',[173 223 173]/255,'ForegroundColor',[0 0 0],'callback',@onconfirm5);

b2 = uiextras.VBox('Parent',b);
b2b = uicontrol('Parent',b2,'style','edit','string','Measurements','BackgroundColor',[15 93 48]/255,'ForegroundColor',[1 1 1]);
b2a = uicontrol('Parent',b2,'style','list');
temp = {['IPD: ' num2str(IPD)],['TW: ' num2str(TW)],'NBW: NA','ES: NA','Custom: NA'};
bstring = '';
for i = 1:4
	if i==1
		bstring = sprintf('%s%s',bstring, temp{i});
	else
		bstring = sprintf('%s \n%s',bstring, temp{i});
	end
end
set(b2a,'string',bstring);
b2b = uicontrol('Parent',b2,'style','push','string','close GUI','BackgroundColor','red','callback',@onclose);

set(mainlayout,'Sizes',[-7 -2]);
set(b,'Sizes',[-4 -1]);
set(b1,'Sizes',[-2 -3 -2 -3 -2 -3 -3]);
set(b2,'Sizes',[-2 -13 -3]);

status = 0; rcounter = 0;
while guiopen==1
	bstring = '';
	for i = 1:4
		if i==1
			bstring = sprintf('%s%s',bstring, temp{i});
		else
			bstring = sprintf('%s \n%s',bstring, temp{i});
		end
	end
	set(b2a,'string',bstring);
	set(b1a1e,'string',num2str(meshscale));
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

	x1(1) = str2num(get(b1b1,'string'));
	x1(2) = str2num(get(b1b2,'string'));
	x1(3) = str2num(get(b1b3,'string'));
	x2(1) = str2num(get(b1c1,'string'));
	x2(2) = str2num(get(b1c2,'string'));
	x2(3) = str2num(get(b1c3,'string'));
	rcounter = rcounter + 1;

	temp{1} = ['IPD: ' num2str(meshscale*sqrt(sum((x1-x2).^2)))];
	out(1) = meshscale*sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function onconfirm2(varargin)
% on point confirmation

	x1(1) = str2num(get(b1b1,'string'));
	x1(2) = str2num(get(b1b2,'string'));
	x1(3) = str2num(get(b1b3,'string'));
	x2(1) = str2num(get(b1c1,'string'));
	x2(2) = str2num(get(b1c2,'string'));
	x2(3) = str2num(get(b1c3,'string'));
	rcounter = rcounter + 1;

	temp{2} = ['TW: ' num2str(meshscale*sqrt(sum((x1-x2).^2)))];
	out(2) = meshscale*sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function onconfirm3(varargin)
% on point confirmation

	x1(1) = str2num(get(b1b1,'string'));
	x1(2) = str2num(get(b1b2,'string'));
	x1(3) = str2num(get(b1b3,'string'));
	x2(1) = str2num(get(b1c1,'string'));
	x2(2) = str2num(get(b1c2,'string'));
	x2(3) = str2num(get(b1c3,'string'));
	rcounter = rcounter + 1;

	temp{3} = ['NBW: ' num2str(meshscale*sqrt(sum((x1-x2).^2)))];
	out(3) = meshscale*sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function onconfirm4(varargin)
% on point confirmation

	x1(1) = str2num(get(b1b1,'string'));
	x1(2) = str2num(get(b1b2,'string'));
	x1(3) = str2num(get(b1b3,'string'));
	x2(1) = str2num(get(b1c1,'string'));
	x2(2) = str2num(get(b1c2,'string'));
	x2(3) = str2num(get(b1c3,'string'));
	rcounter = rcounter + 1;

	temp{4} = ['ES: ' num2str(meshscale*sqrt(sum((x1-x2).^2)))];
	out(4) = meshscale*sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function onconfirm5(varargin)
% on point confirmation

	x1(1) = str2num(get(b1b1,'string'));
	x1(2) = str2num(get(b1b2,'string'));
	x1(3) = str2num(get(b1b3,'string'));
	x2(1) = str2num(get(b1c1,'string'));
	x2(2) = str2num(get(b1c2,'string'));
	x2(3) = str2num(get(b1c3,'string'));
	rcounter = rcounter + 1;

	temp{5} = ['Custom: ' num2str(meshscale*sqrt(sum((x1-x2).^2)))];
	out(5) = meshscale*sqrt(sum((x1-x2).^2));
	status = 1;
end

%-----------------------------------------------------------------------------%
function ongetscale(varargin)
% on get scale

	temp1 = strsplit(get(b1a1a,'string'));
	temp2 = strsplit(get(b1a1b,'string'));
	for i = 1:3
		x1(i) = str2num(temp1{i});
		x2(i) = str2num(temp2{i});
	end
	x3 = str2num(get(b1a1c,'string'));
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