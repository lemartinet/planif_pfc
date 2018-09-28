function [l] = cut_field(v)
% taille min de v = 3
[m,i] = max(v);
%th = 0.3 * m;
th = mean(v);
left = find(v(1:i) < th);
right = i+find(v(i+1:end) < th);
if isempty(left)
    left = 1;
end
if isempty(right)
    right = length(v);
end
l = v(max(left)+1:min(right)-1);
end