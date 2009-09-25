function [t] = theta(fr)

fr = zscore(fr);
for j = 1 : 8
	t.bin(j) = sum(fr(j : 8 : end));
end
%t.bin = t.bin ./ sum(t.bin .^ 2);

[m, i] = max(t.bin);
t.amp = m - min(t.bin);
t.phi = (i - 1) * 2 * pi / 8;

end

%ft=fittype('a*sin(b*x+c) + d');
%vx=0:pi/10:2*pi;
%v=sin(0:pi/10:2*pi);
%[cf,good] = fit(vx',v',ft);
%figure, hold on, plot(vx,v), plot(cf)
%coeff = coeffvalues(cf);


