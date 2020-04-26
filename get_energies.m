function get_energies(logfile, n, a)
n=n-1;
% scatter the boldness vs distance (bolder should generally be further from
% home)

% take smaller time slices to compare random and bold dispersions

% find a way for there to be a catastrophe, so that we can see how quickly
% the population re-stabilises

% explore the effects of having the robots only become bolder from bolder
% robots, rather than simply any

% perhaps the increase and the decrease could both be much slower - more
% stable pop?

warning("check number of kilobots (" + num2str(n) + ") is right!")
%warning("check that you are(n't) plotting zeros for BOTH histograms!")

% to view, something like:
% fig = openfig('log2019-06-10 14:48:32.fig');
% set(fig, 'Visible', 'on')

fh(1) = figure('visible', 'off');
xlabel("time (ms)"); ylabel("X"); zlabel("Y");
hold on

fh(2) = figure('visible', 'off');
xlabel("X"); ylabel("Y");
title("Paths of n="+num2str(n)+ " Kilobots")
hold on

fh(3) = figure('visible', 'off');
xlabel("dist"); ylabel("freq per robot");
hold on

fh(4) = figure('visible', 'off');
xlabel("dist"); ylabel("freq total");
hold on

fh(5) = figure('visible', 'off');
xlabel("dist"); ylabel("freq total");
hold on

fh(7) = figure('visible', 'off');
xlabel("time"); ylabel("bold");
hold on

fh(8) = figure('visible', 'off');
xlabel("kbot"); ylabel("time disconnected");
hold on

fh(9) = figure('visible', 'off');
xlabel("distance"); ylabel("time total");
hold on

fh(10) = figure('visible', 'off');
xlabel("time"); ylabel("Gauss centroid");
hold on

fh(11) = figure('visible', 'off');
xlabel("kbot"); ylabel("time disconnected");
hold off

fh(12) = figure('visible', 'off');
xlabel("time"); ylabel("number of robots");
hold on

fh(13) = figure('visible', 'off');
xlabel("time"); ylabel("proportion of swarm");
hold on

fh(14) = figure('visible', 'off');
xlabel("time"); ylabel("proportion of swarm");
title("Relative Boldness of n="+num2str(n)+" Kilobots Enduring Population Catastrophes")
hold on

fh(15) = figure('visible', 'off');
xlabel("time"); ylabel("proportion of swarm");
title("Relative Boldness of n="+num2str(n)+" Kilobots Enduring Boldness Catastrophes")
hold on

fh(16) = figure('visible', 'off');
xlabel("Average Boldness"); ylabel("Distance Travelled");
title("Distance Travelled by n="+num2str(n)+" Kilobots")
hold on

fh(17) = figure('visible', 'off');
xlabel("Average Target Distance"); ylabel("Distance Travelled");
title("Distance Travelled by n="+num2str(n)+" Kilobots")
hold on

fh(6) = figure('visible', 'off');
hold off

lost = zeros(1,n);

path = split(logfile, ".xlsx");

name = path(1)+" energy for " + num2str(n) + " Kilobots ";

[~,~,dat]=xlsread(logfile, 1);
dist=dat(2:end,7);

xbins = -10:10;

%count number of kilobots that reach their target
count = 0;

travelled_tots = zeros(1,n);

for i = 2:n+1
    [~,~,dat]=xlsread(logfile, i);
    i = i - 1; % some jiggery pokery - first robot is the counter, so shouldn't take part

    X=dat(2:end,2);
    Y=dat(2:end,3);
    t=dat(2:end,1);
    targ=dat(2:end,8);
    bold=dat(2:end,6);
	targplot = zeros(1,length(X));
    tplot = zeros(1,length(t));
    xplot = zeros(1,length(X));
    yplot = zeros(1,length(Y));
    boldplot = zeros(1,length(bold));
    if i == 1
        boldtotplot = zeros(n,length(bold));
    end
    for j = 1:length(X)
        xplot(j) = X{j};
    end
    for j = 1:length(Y)
        yplot(j) = Y{j};
    end
    for j = 1:length(t)
        tplot(j) = t{j};
    end
    for j = 1:length(targ)
        targplot(j) = targ{j};
    end
    for j = 1:length(bold)
        boldplot(j) = bold{j};
    end
    travelled = 0;
    AN = atan2(yplot,xplot);
    for j = 2:length(AN)
        xchan = xplot(j)-xplot(j-1);
        ychan = yplot(j)-yplot(j-1);
        mags = sqrt(xchan^2+ychan^2)*sqrt(xplot(j)^2+yplot(j)^2);
        % use magnitudes/'mags' to track how far each kilobot has travelled
        travelled = travelled + mags;
    end
    travelled_tots(i) = travelled;
    boldtotplot(i,:) = boldplot;
    targtotplot(i,:) = targplot;
    
    disp(num2str(i) + "/" + num2str(n))
end

med = (255/26)*sqrt((n+pi*3^2)/pi);
bold = (255/26)*((sqrt(n)-1)*3)/2;

%figure(15);
disp("plotting boldness levels")
set(0, 'currentfigure', fh(15));
plot(tplot, sum(boldtotplot<med)/n);
plot(tplot, sum(boldtotplot>med&boldtotplot<bold)/n);
plot(tplot, sum(boldtotplot>bold)/n);
ylim([0 1]);
legend({"Shy", "Medium", "Bold"}, 'Location','northwest');

%figure(12);
disp("plotting boldness levels")
set(0, 'currentfigure', fh(12));
plot(tplot, sum(boldtotplot<80)/n);
plot(tplot, sum(boldtotplot>80&boldtotplot<160)/n);
plot(tplot, sum(boldtotplot>160)/n);
ylim([0 1]);
legend({"Shy", "Medium", "Bold"}, 'Location','northwest');

%figure(13);
disp("plotting split boldness levels")
set(0, 'currentfigure', fh(13));
plot(tplot, sum(boldtotplot<40)/n);
plot(tplot, sum(boldtotplot>40&boldtotplot<80)/n);
plot(tplot, sum(boldtotplot>80&boldtotplot<120)/n);
plot(tplot, sum(boldtotplot>120&boldtotplot<160)/n);
plot(tplot, sum(boldtotplot>160&boldtotplot<200)/n);
plot(tplot, sum(boldtotplot>200)/n);
ylim([0 1]);
legend({"Shy", "Shy-Medium", "Medium", "Medium-Bold", "Bold", "Very Bold"}, 'Location','northwest');


%med = (255/26)*sqrt(n/pi); %med is boldness at edge of where all kilobots could
%pack into tightly (at bold = 0)
med = (255/26)*sqrt((n+pi*3^2)/pi);
med2 = (255/26)*sqrt(((n/2)+pi*3^2)/pi);
med3 = (255/26)*sqrt(((n/4)+pi*3^2)/pi);

bold = (255/26)*((sqrt(n)-1)*3)/2; %bold is the region that a homogenous spread
bold2 = (255/26)*((sqrt(n/2)-1)*3)/2; %bold is the region that a homogenous spread
bold3 = (255/26)*((sqrt(n/4)-1)*3)/2; %bold is the region that a homogenous spread
%bold = (255/26)*sqrt(n*6/pi); %bold is the region that a homogenous spread
%of kilobots would cover

%figure(14);
disp("plotting boldness levels")
set(0, 'currentfigure', fh(14));
sh = [sum(boldtotplot(:,1:1500)<med)/n sum(boldtotplot(1:(n/2),1501:3000)<med2)/(n/2) sum(boldtotplot(1:(n/4),3001:end)<med3)/(n/4)];
me = [sum(boldtotplot(:,1:1500)>med&boldtotplot(:,1:1500)<bold)/n sum(boldtotplot(1:(n/2),1501:3000)>med2&boldtotplot(1:(n/2),1501:3000)<bold2)/(n/2) sum(boldtotplot(1:(n/4),3001:end)>med3&boldtotplot(1:(n/4),3001:end)<bold3)/(n/4)];
bo = [sum(boldtotplot(:,1:1500)>bold)/n sum(boldtotplot(1:(n/2),1501:3000)>bold2)/(n/2) sum(boldtotplot(1:(n/4),3001:end)>bold3)/(n/4)];
plot(tplot, sh, tplot, me, tplot, bo);

%plot(tplot(:,1:1500), sum(boldtotplot(:,1:1500)>bold)/128);
%plot(tplot(:,1501:3000), sum(boldtotplot(:,1501:3000)>bold)/64);
%plot(tplot(:,3001:end), sum(boldtotplot(:,3001:end)>bold)/32);
ylim([0 1]);
legend({"Shy", "Medium", "Bold"}, 'Location','northwest');


%figure(16)    
disp("plotting travelled distances")
set(0, 'currentfigure', fh(16));
mean_bold = zeros(1,n);
for i = 1:n
    mean_bold(i) = mean(boldtotplot(i,:))/255;
end
scatter(mean_bold,travelled_tots);
y=polyfit(mean_bold,travelled_tots,1);
hold on
plot(mean_bold,mean_bold*y(1)+y(2));
ylim([0 round(max(travelled_tots*1.1))]);
xlim([0 1]);

disp("saving files")
saveas(fh(16), name + "16" + a, 'fig')
saveas(fh(16), name + "16" + a, 'png')
saveas(fh(12), name + "12" + a, 'fig')
saveas(fh(12), name + "12" + a, 'png')
saveas(fh(13), name + "13" + a, 'fig')
saveas(fh(13), name + "13" + a, 'png')
saveas(fh(14), name + "14" + a, 'fig')
saveas(fh(14), name + "14" + a, 'png')
saveas(fh(15), name + "15" + a, 'fig')
saveas(fh(15), name + "15" + a, 'png')

%figure(17)    
disp("plotting travelled distances")
set(0, 'currentfigure', fh(17));
mean_targ = zeros(1,n);
for i = 1:n
    mean_targ(i) = mean(targtotplot(i,:))/26;
end
scatter(mean_targ,travelled_tots);
ylim([0 round(max(travelled_tots*1.1))]);
xlim([0 1]);
y=polyfit(mean_targ,travelled_tots,1);
hold on
plot(mean_targ,mean_targ*y(1)+y(2));

disp("saving files")
saveas(fh(17), name + "17" + a, 'fig')
saveas(fh(17), name + "17" + a, 'png')

save(name + " variables " + a)

end