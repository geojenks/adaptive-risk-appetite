function [RWMM maxat] = RWMM_pred(a,n,sev)

RWMM = zeros(1,11);

rings = 28;
inc = rings*40;

x = 0:a/(inc-1):a;
figure;
hold on;
l = 1;

r=a/(rings*2); %robots have roughly half the radius of each annulus
for k = n %[0:n/10:n] % collisions become more frequent/extreme as more robots are added
    i=1;
    y=zeros(1,inc);
    %edge = sqrt(n*r^2); %if all robots were densely packed, this is the distance they'd come to
    %norm = (k^2/6+k*5/6+1); %analytical result for sev = 1
    for j = x
        severe = ((a-j)/a)^sev; %arb
        y(i) = ((j^2*(-6/(a^3))+j*(6/(a^2)))*severe^k);%*norm;
        % need to multiply this by a Fermi-Dirac dist such that at higher
        % populations the inner distances become LESS likely to occupy due
        % to crowding
        i = i + 1;
    end
    
    %temp = y*k;
    %fd = 2 - 2./(exp((x-edge).*temp)+1) % Fermi-Dirac inspired correction
    %plot(x,fd);

	norm = 1/trapz(x,y); % slightly cheating to get the normalisation - an analytical solution would be preferred
    y = y*norm;
    
    % numerical solution to overcrowding:
    barch = zeros(1,rings);
    for i = 1:rings
        b = y(round((i-1)*inc/rings)+1:round(i*inc/rings));
        max_pop = (((a*i/rings)^2)-((a*(i-1)/rings)^2))/(r^2);
        %pop = trapz(b)*n
        %pop = trapz(b)
        pop = trapz(x(round((i-1)*inc/rings)+1:round(i*inc/rings)),b)*n;
        if pop > max_pop
            disp('a')
            increase = (pop-max_pop)/n;
            slot = (1/rings);
            y(round((i-1)*inc/rings+1):round(i*inc/rings)) = y(round((i-1)*inc/rings+1):round(i*inc/rings)) - increase/slot;%(pop - max_pop)/(inc);%n/inc;%/(trapz(b)/(max_pop/(rings/inc))); %subtract the excess that can't fit
            %round(1 + i*inc/rings)
            %(pop - max_pop)/(inc/rings)
            if i ~= rings
                %increase/slot
                %bef = trapz(x(round(i*inc/rings)+1:round((i+1)*inc/rings)),y(round(i*inc/rings)+1:round((i+1)*inc/rings)))*n
                %inc = (pop - max_pop)/rings
                %befo = sum(y(round(i*inc/rings)+1:round((i+1)*inc/rings)))
                %round(i*inc/rings)+1:round((i+1)*inc/rings)
                y(round(i*inc/rings)+1:round((i+1)*inc/rings)) = y(round(i*inc/rings)+1:round((i+1)*inc/rings)) +increase/slot;%+ (pop - max_pop)/(inc);%n/inc; % put the excess into the next region
                %aft = trapz(x(round((i)*inc/rings)+1:round((i+1)*inc/rings)),y(round(i*inc/rings)+1:round((i+1)*inc/rings)))*n
                %afte = sum(y(round(i*inc/rings)+1:round((i+1)*inc/rings)))
            end
            %disp("pops")
            %disp(i)
            %disp(max_pop)
            %disp( trapz(x(round((i-1)*inc/rings)+1:round(i*inc/rings)),b))
            %disp(pop)
        end
        barch(i) = trapz(x(round((i-1)*inc/rings)+1:round(i*inc/rings)),b);
        %norm = 1/trapz(x,y); % slightly cheating to get the normalisation - an analytical solution would be preferred
        %y = y*norm;
        %if i == 3
        %    disp("pops")
        %    disp(max_pop)
        %    disp(pop)
        %end
    end

    %plot(x,y, 'Color',[k/n,1-k/n,0])
    %fh(1) = figure('visible', 'on');
    bar((0:rings-1)/27,barch)
    xlabel("distance"); ylabel("pdf"); zlabel("Y"); title("Simulated distribution following RWP for n="+num2str(n)+" Kilobots")
    %saveas(fh(1),"log2019-09-01 22:27:47" + "_predict", 'fig')
    %saveas(fh(1),"log2019-09-01 22:27:47" + "_predict", 'png')
    %trapz(x,y)*(k^2/6+k*5/6+1)
    % y(inc/k)
    RWMM(l) = trapz(x,y);
    l = l + 1;
    %*(1.5/y(501));
    %mean(x.^k);
end
peak = findpeaks(y);
index = find(y==peak(end));
maxat = x(index);

return