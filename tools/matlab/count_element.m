function frequency_table = count_element(vectr)
% COUNT_ELEMENT - fast and efficient way to count different 
%  elements in an array of integers.
% 
% Example:
%  v = round(rand(300,1)*20);
%  f = count_elements(v)
%
% See also sparse 
[keys,idx,jdx]  = unique(vectr);
frequency_table = struct('keys',keys,'value',sparse(jdx,1,1));
