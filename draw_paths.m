function draw_paths(logfile, n)
n=n-1; % this is because the 'beacon' shouldn't be analysed - remove this line if not using a beacon
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

[~,~,dat]=xlsread(logfile, 1);
dist=dat(2:end,7);

lostslice = zeros(n, round((length(dist)-50)/100));

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
    msgs=dat(2:end,5);
    bold=dat(2:end,6);
    dist=dat(2:end,7);
    targ=dat(2:end,8);
	targplot = zeros(1,length(X));
	xplot = zeros(1,length(X));
    yplot = zeros(1,length(Y));
    tplot = zeros(1,length(t));
    boldplot = zeros(1,length(bold));
    targplot = zeros(1,length(targ));
    reachtarg = targplot;
    if i == 1
        disttotplot = zeros(1,n*length(dist));
        boldtotplot = zeros(n,length(bold));
        targtotplot = zeros(n,length(targ));
        alpha1 = zeros(1,length(tplot));
        alpha2 = zeros(1,length(tplot));
        beta1 = zeros(1,length(tplot));
        beta2 = zeros(1,length(tplot));
        kappa1 = zeros(1,length(tplot));
    end
    distplot = zeros(1,length(dist));
    for j = 1:length(X)
        xplot(j) = X{j};
    end
    for j = 1:length(Y)
        yplot(j) = Y{j};
    end
    for j = 1:length(t)
        tplot(j) = t{j};
    end
    for j = 1:length(dist)
        distplot(j) = dist{j};
    end
    for j = 1:length(targ)
        targplot(j) = targ{j};
    end
    for j = 1:length(bold)
        boldplot(j) = bold{j};
        % attempt to numerically obtain values for alpha, beta, gamma as in
        % Edmund's ODEs --> Don't think this will work, it needs to be
        % proportional to the current population of shy/med/bold else it
        % will always even out
        if j > 2
            if (boldplot(j)>80 && boldplot(j-1)<81)
                alpha1(i) = alpha1(i) + 1;
            end
            if (boldplot(j)>160 && boldplot(j-1)<161)
                alpha2(i) = alpha2(i) + 1;
            end
            if (boldplot(j)<81 && boldplot(j-1)>80)
                beta1(i) = beta1(i) + 1;
            end
            if (boldplot(j)<161 && boldplot(j-1)>160)
                beta2(i) = beta2(i) + 1;
            end
            if (boldplot(j)<81 && boldplot(j-1)>160)
                kappa1(i) = kappa1(i) + 1;
            end
        end
    end
    for j = 1:length(msgs)
        msgsplot(j) = msgs{j};
    end
    for j = 1:length(targ)
        targplot(j) = targ{j};
        if j ~= 1
            reachtarg(j) = targ{j} - targ{j-1};
        end
    end
    count = count + length(nonzeros(reachtarg));
    % for plotting the radial vs tangential movement as an angle compared
    % to radial line
    D = sqrt(xplot.^2+yplot.^2);
    AN = atan2(yplot,xplot);
    CHAN = AN;
    travelled = 0;
    for j = 2:length(AN)
        CHAN(j) = AN(j) - AN(j-1);
        xchan = xplot(j)-xplot(j-1);
        ychan = yplot(j)-yplot(j-1);
        dot = xplot(j)*xchan+yplot(j)*ychan;
        mags = sqrt(xchan^2+ychan^2)*sqrt(xplot(j)^2+yplot(j)^2);
        VECANG(length(AN)*(i-1)+j) = acos(dot/mags);
        
        % use magnitudes/'mags' to track how far each kilobot has travelled
        travelled = travelled + mags;
    end
    travelled_tots(i) = travelled;
    
    disttotplot((i-1)*length(dist)+1:i*length(dist)) = distplot;
    %figure(1);
    disp("plotting 3D plot trajectories")
    set(0, 'currentfigure', fh(1));
    plot3(tplot,xplot,yplot);
    
    %figure(5);
    disp("plotting trajectory plot distances")
    set(0, 'currentfigure', fh(5));
    plot(tplot,sqrt(xplot.^2+yplot.^2));
    
    %figure(2);
    disp("plotting 2D trajectory")
    set(0, 'currentfigure', fh(2));
    plot(xplot(500:end), yplot(500:end), 'Color', [0,0,0,3/n]);
    ylim([-1 1]);
    xlim([-1 1]);
    
    %figure(3); - plotting here will overlay a different colour for each
    %kilobot, so this becomes unintelligible for large n
    disp("plotting histogram")
    set(0, 'currentfigure', fh(3));
    histogram(distplot);%, xbins);
    %histogram(nonzeros(distplot), xbins);
    
    %figure(7);
    %plots the boldness for a single kilobot over time. Hold is on so this
    %becomes unintelligible for large n
    disp("plotting boldness")
    set(0, 'currentfigure', fh(7));
    plot(tplot, boldplot);    
    boldtotplot(i,:) = boldplot;
    targtotplot(i,:) = targplot;
    
    % only count when kilobot is isolated for more than a few seconds
    discnct = msgsplot;
    for j = 5:length(msgsplot)
        discnct(j) = sum(msgsplot(j-4:j));
    end
    
    lost(i) = nnz(discnct);
    
    steps = round((length(disttotplot)/n-50)/100);
    for j = 1:steps
        lostslice(i,j) = nnz(discnct((j-1)*100+1:j*100));
    end
    
    disp(num2str(i) + "/" + num2str(n))
end

%{
% an attempt to numerically find alpha beta and kappa values for Edmund's
% set of ODEs
foralpha1 = zeros(1,length(tplot));
foralpha2 = zeros(1,length(tplot));
forbeta1 = zeros(1,length(tplot));
forbeta2 = zeros(1,length(tplot));
forkappa = zeros(1,length(tplot));

grnd = 1;
for i = 2:length(tplot)
    if alpha1(i) || alpha2(i) || beta1(i) || beta2(i)
        shy = (sum(boldtotplot(:,i-1)<80))/n;
        med = (sum(boldtotplot(:,i-1)>80)+sum(boldtotplot(:,i)<160))/n;
        bold = (sum(boldtotplot(:,i-1)>160))/n;
        foralpha1(i) = double(alpha1(i))/(shy*med*(i-grnd));
        foralpha2(i) = double(alpha2(i))/(med*bold*(i-grnd));
        forbeta1(i) = double(beta1(i))/(med*(i-grnd));
        forbeta2(i) = double(beta2(i))/(bold*(i-grnd));
        forkappa(i) = double(kappa1(i))/(shy*(i-grnd));
        grnd = i;
    end
end

alpha = sum(foralpha1+foralpha2)/length(tplot);
beta = sum(forbeta1+forbeta2)/length(tplot);
kappa = sum(forkappa)/length(tplot);
alpha10 = sum(foralpha1)/length(tplot);
beta10 = sum(forbeta1)/length(tplot);
alpha20 = sum(foralpha2)/length(tplot);
beta20 = sum(forbeta2)/length(tplot);
kappa10 = sum(forkappa)/length(tplot);
%}
        
%med = (255/26)*sqrt(n/pi); %med is boldness at edge of where all kilobots could
%pack into tightly (at bold = 0)
%med = (255/26)*sqrt((n+pi*3^2)/pi);

%bold = (255/26)*((sqrt(n)+1)*3)/2; %bold is the region that a homogenous spread
%bold = (255/26)*sqrt(n*6/pi); %bold is the region that a homogenous spread
%of kilobots would cover

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

tot = sum(lost)/n;
disp("plotting disconnection time")
set(0, 'currentfigure', fh(8));
bar(1:n, lost);
title("Time spent with at least 1 neighbour avg = " + num2str(tot));
disp(lost);

name = extractBetween(logfile, 1,strlength(logfile)-5);

%{
disp("giffing disconnection time by slice")
set(0, 'currentfigure', fh(11));
for i = 1:steps
	disp("plotting time " + num2str(i*100))
    bar(1:n, lostslice(:,i));
    tot = sum(lostslice(:,i))/n;
    title("Time spent with at least 1 neighbour avg = " + num2str(tot));
    if i == 1
        avgs = [tot];
        gif([char(name) ' disconnection.gif'],'DelayTime',0.2, 'frame', gcf);
    else
        avgs = [avgs; tot];
        gif
    end
end
%}

%{
disp("giffing surviving half disconnection time by slice")
set(0, 'currentfigure', fh(11));
for i = 1:steps
	disp("plotting time " + num2str(i*100))
    bar(1:n/2, lostslice(1:n/2,i));
    tot = sum(lostslice(1:n/2,i))/(n/2);
    title("Time spent with at least 1 neighbour avg = " + num2str(tot));
    if i == 1
        halfavgs = [tot];
        gif([char(name) ' half disconnection.gif'],'DelayTime',0.2, 'frame', gcf);
    else
        halfavgs = [halfavgs; tot];
        gif
    end
end
%}

%{
disp("giffing surviving quarter disconnection time by slice")
set(0, 'currentfigure', fh(11));
for i = 1:steps
	disp("plotting time " + num2str(i*100))
    bar(1:n/4, lostslice(1:n/4,i));
    tot = sum(lostslice(1:n/4,i))/(n/4);
    title("Time spent with at least 1 neighbour avg = " + num2str(tot));
    if i == 1
        quartavgs = [tot];
        gif([char(name) ' quarter disconnection.gif'],'DelayTime',0.2, 'frame', gcf);
    else
        quartavgs = [quartavgs; tot];
        gif
    end
end
%}

% plot over the top with the plots for all bots each 100s
plot([1:steps]*100, sum(lostslice));
ylim([0 n*100]);

title("Time spent by " + num2str(n) + " robots with at least 1 neighbour");
disp(lost);

%{
%figure(4); - plotting here is total of all kilobots
disp("plotting histogram total")
set(0, 'currentfigure', fh(4));
%%%%histfit(disttotplot,max(disttotplot) - min(disttotplot));
x = unique(disttotplot);
y = transpose(histc(disttotplot(:), x));
f = fit(x.',y.','gauss1');
g = fit(x.',y.','gauss2');
histogram(disttotplot);%, xbins);
%%%histogram(nonzeros(disttotplot),xbins);
% see if Gauss2 is necessary:

% regenerate data from gaussian fit
tot = 0;
for i = x
    tot = tot + f(i);
end
prob = 0*x;
probg = 0*x;
for i = x+1
    prob(i) = f(i-1)/tot;
    probg(i) = g(i-1)/tot;
end

distances = 0*x;
distances2 = 0*x;
distances_actual = 0*x;
for i = 1:length(disttotplot)
    % choose the location of kilobot as a point in the prob distribution
    loc = rand;
    distances_actual(abs(disttotplot(i))+1) = distances_actual(abs(disttotplot(i))+1) + 1;
    for j = x+1
        % incremement the relevant distance
        loc = loc - prob(j);
        loc2 = loc - probg(j);
        % if the number lies within the distribution relevant to a certain
        % distance, score that distance
        if loc < 0
            distances(j) = distances(j) + 1;
            loc = 10; % reset to any number more than 1, so it doesn't score twice
        end
        if loc2 < 0
            distances2(j) = distances2(j) + 1;
            loc2 = 10; % reset to any number more than 1, so it doesn't score twice
        end
    end
end
% see the predictions if you want 
%figure;
%bar(x,distances)
%figure;
%bar(x,distances2)
%figure;
%bar(x,distances_actual)

g1 = sqrt(sum((distances_actual - distances).^2));
g2 = sqrt(sum((distances_actual - distances2).^2));

% if using 2 peaked Gaussian significantly improves the fit, use 2 peaked
% Gaussian
if g1*0.8 > g2
    f = g;
end
plot(x,f(x));

coverage = zeros(1,x(end)+4);
area = zeros(1,x(end)+4);
area(1) = pi*0.33^2;
for i = 1:x(end)+4
area(i+1) = pi*((0.33*(i+1))^2-(0.33*(i))^2);
end
for i = x+1
coverage(i) = coverage(i) + distances(i)*(area_cross(0,1) - area_cross(1/6,1))/area(i);
coverage(i+1) = coverage(i+1) + distances(i)*(area_cross(1/6,1)-area_cross(1/6+1/3,1))/area(i+1);
coverage(i+2) = coverage(i+2) + distances(i)*(area_cross(1/6+1/3,1)-area_cross(1/6+2/3,1))/area(i+2);
coverage(i+3) = coverage(i+3) + distances(i)*(area_cross(1/6+2/3,1)-area_cross(1,1))/area(i+3);
end
%}

%figure(16)    
disp("plotting travelled distances")
set(0, 'currentfigure', fh(16));
mean_bold = zeros(1,n);
for i = 1:n
    mean_bold(i) = mean(boldtotplot(i,:));
end
scatter(mean_bold,travelled_tots);
y=polyfit(mean_bold,travelled_tots,1);
hold on
plot(mean_bold,mean_bold*y(1)+y(2));
ylim([0 1+round(max(travelled_tots*1.1))]);
xlim([0 1+round(max(mean_bold*1.1))]);

%figure(17)    
disp("plotting travelled distances")
set(0, 'currentfigure', fh(17));
mean_targ = zeros(1,n);
for i = 1:n
    mean_targ(i) = mean(targtotplot(i,:));
end
scatter(mean_targ,travelled_tots);
ylim([0 1+round(max(travelled_tots*1.1))]);
xlim([0 1+round(max(mean_targ*1.1))]);
y=polyfit(mean_targ,travelled_tots,1);
hold on
plot(mean_targ,mean_targ*y(1)+y(2));

disp("saving files")
saveas(fh(17), name + "17", 'fig')
saveas(fh(17), name + "17", 'png')

disp("plotting radial/tangential angle")
set(0, 'currentfigure', fh(6));
polarhistogram(VECANG, 'BinWidth',0.125)
title("trajectory angles for " + num2str(n) + " kilobots")

disp("saving files")

saveas(fh(1), name + "1", 'fig')
saveas(fh(1), name + "1", 'png')
saveas(fh(2), name + "2", 'png')
saveas(fh(2), name + "2", 'fig')
saveas(fh(3), name + "3", 'fig')
saveas(fh(3), name + "3", 'png')
saveas(fh(4), name + "4", 'fig')
saveas(fh(4), name + "4", 'png')
saveas(fh(5), name + "5", 'fig')
saveas(fh(5), name + "5", 'png')
saveas(fh(6), name + "6", 'fig')
saveas(fh(6), name + "6" + num2str(count/n) + ".png", 'png')
saveas(fh(7), name + "7", 'fig')
saveas(fh(7), name + "7", 'png')
saveas(fh(8), name + "8", 'fig')
saveas(fh(8), name + "8", 'png')
saveas(fh(11), name + "11", 'fig')
saveas(fh(11), name + "11", 'png')
saveas(fh(12), name + "12", 'fig')
saveas(fh(12), name + "12", 'png')
saveas(fh(13), name + "13", 'fig')
saveas(fh(13), name + "13", 'png')
saveas(fh(14), name + "14", 'fig')
saveas(fh(14), name + "14", 'png')
saveas(fh(15), name + "15", 'fig')
saveas(fh(15), name + "15", 'png')
saveas(fh(16), name + "16", 'fig')
saveas(fh(16), name + "16", 'png')


fh(9) = figure('visible', 'off');
xlabel("distance"); ylabel("time total");
hold on

set(0, 'currentfigure', fh(9));
first = zeros(1,n*100);
disp("making steady state gif")
hold off

%{
fs = [];
gs = [];
ferrs = [];
gerrs = [];

% slice into 100s sections:
slice = 100;
sections = round((length(disttotplot)/n-50)/slice);
for k = 1:sections
    disp("plotting time " + num2str(k*slice))
    % for each kilobot
    for i = 1:n
        % for each second in the current 100s slice
        %for j = (100*k)-99:100*k
        for j = 1:slice
            %first((i-1)*n+j) = disttotplot((i-1)*length(disttotplot)/n+j);
            first((i-1)*slice+j) = disttotplot((k-1)*slice+round(length(disttotplot)/n)*(i-1)+j);
            %first((i-1)*j+j) = disttotplot(((i-1)*round(length(disttotplot)/n-50)/100)+j);
        end
    end
    Y = [min(disttotplot)-1:max(disttotplot)+1];
    %h = histogram(first((k-1)*n*100+1:k*n*100), 'BinEdges', [Y(1)-0.5 Y+0.5]);
    h = histogram(first, 'BinEdges', [Y(1)-0.5 Y+0.5]);
    hold on
    
    a = unique(first);
    b = transpose(histc(first(:), a));
    %g = fit(a.',b.','gauss1');
    g = fit(a.',b.','gauss2');
    plot(a,g(a));
    
    plot(x,f(x)./sections);
    legend({"Data", "centroid = " + num2str(f.b1) + ", amplitude = " + num2str((f.a1)/sections) + ", width = " + num2str(f.c1),"centroid = " + num2str(g.b1) + ", amplitude = " + num2str((g.a1)) + ", width = " + num2str(g.c1)},'Location','northeast')
    hold off
    fs = [fs f.b1];
    ferrs = [ferrs f.c1/2];
    gs = [gs g.b1];
	gerrs = [gerrs g.b1/2];
    %length(first((k-1)*n*100+1:k*n*100))
    ylim([0 1000]);
    %display tot above each column
    for i = 1:length(Y)
        h(1).Values;
        val = num2str(h(1).Values(i));
        if h(1).Values(i) == 0
            val = "";
        end
        text(Y(i),h(1).Values(i),val,'vert','bottom','horiz','center'); 
    end
    title("Distribution at " + num2str(k*slice) + " seconds");
    if k == 1
        gif([char(name) ' reaching steady state.gif'],'DelayTime',0.2, 'frame', gcf);
    else
        gif
    end
end

%{
% for plotting quarter of the swarm's distribution every 100s

set(0, 'currentfigure', fh(9));
first = zeros(1,(n/4)*100);
disp("making steady state quarters gif")
hold off

fs = [];
gs = [];
ferrs = [];
gerrs = [];

% slice into 100s sections:
slice = 100;
sections = round((length(disttotplot)/n-50)/slice);
overall = [];
for k = 1:sections
    disp("plotting time " + num2str(k*slice))
    % for each kilobot
    for i = 1:n/4
        % for each second in the current 100s slice
        %for j = (100*k)-99:100*k
        for j = 1:slice
            %first((i-1)*n+j) = disttotplot((i-1)*length(disttotplot)/n+j);
            first((i-1)*slice+j) = disttotplot((k-1)*slice+round(length(disttotplot)/n)*(i-1)+j);
            %first((i-1)*j+j) = disttotplot(((i-1)*round(length(disttotplot)/n-50)/100)+j);
        end
    end
    Y = [min(disttotplot)-1:max(disttotplot)+1];
    %h = histogram(first((k-1)*n*100+1:k*n*100), 'BinEdges', [Y(1)-0.5 Y+0.5]);
    h = histogram(first, 'BinEdges', [Y(1)-0.5 Y+0.5]);
    hold on
    
    a = unique(first);
    b = transpose(histc(first(:), a));
    %g = fit(a.',b.','gauss1');
    g = fit(a.',b.','gauss2');
    plot(a,g(a));
    
    plot(x,f(x)./sections);
    legend({"Data", "centroid = " + num2str(f.b1) + ", amplitude = " + num2str((f.a1)/sections) + ", width = " + num2str(f.c1),"centroid = " + num2str(g.b1) + ", amplitude = " + num2str((g.a1)) + ", width = " + num2str(g.c1)},'Location','northeast')
    hold off
    fs = [fs f.b1];
    ferrs = [ferrs f.c1/2];
    gs = [gs g.b1];
	gerrs = [gerrs g.b1/2];
    %length(first((k-1)*n*100+1:k*n*100))
    ylim([0 1000]);
    %display tot above each column
    for i = 1:length(Y)
        h(1).Values;
        val = num2str(h(1).Values(i));
        if h(1).Values(i) == 0
            val = "";
        end
        text(Y(i),h(1).Values(i),val,'vert','bottom','horiz','center'); 
    end
    title("Distribution at " + num2str(k*slice) + " seconds");
    if k == 1
        gif([char(name) ' reaching steady state quarter.gif'],'DelayTime',0.2, 'frame', gcf);
    else
        gif
    end
    overall = [overall first];
    if k == 15 || k == 30 || k == 49
        histogram(overall);
        saveas(fh(9), name + num2str(k), 'png')
        overall = [];
    end
end
%}

%figure; ylim([0 25]); hold on; errorbar(1:length(fs), fs, ferrs); errorbar(1:length(gs), gs, gerrs);
fh(10) = figure('visible', 'off');
xlabel("time"); ylabel("Gauss centroid");
hold on

set(0, 'currentfigure', fh(10));
disp("plotting steady state")
hold on
ylim([0 25]);
plot([1:length(fs)].*100, fs, [1:length(gs)].*100, gs);

saveas(fh(10), name + "10", 'fig')
saveas(fh(10), name + "10", 'png')

%}
save(name + " variables")

disp("average number of targets met = " + num2str(count/n))

disp("finished")

end