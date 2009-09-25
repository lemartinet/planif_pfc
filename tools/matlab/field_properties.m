function [center, fsize, m_fr, s_fr] = field_properties (fr, probav)
% [center, fsize, m_fr, s_fr] = field_properties (fr, probav)
% 
% Param:
% fr = multidim matrix of mean firing rate for each n-uplet varsa x varsb x ... 
% probav = matrix of the proba for the var, e.g occupancy grid for x,y vars
%
% Return:
% center = estimated center of the receptive field
% fsize = estimated size of the receptive field
% m_fr = mean firing rate (take into account the occupancy probability)
% s_fr = std firing rate (take into account the occupancy probability)

m_fr = sum (fr(:) .* probav(:));
s_fr = sqrt(sum (fr(:) .^ 2 .* probav(:)) - m_fr .^ 2);

% field area estimated by the number of pixel for which the neurone is active
% i.e. have a bigger response than the std firing rate, the grand mean rate, ...
% (in Johnson&Redish2007, they used a threshold of 20% of max activity)
%rf = find(fr > s_fr);
%rf = find(fr > 0.2 * max(fr(:)));
rf = find(fr > m_fr);
fsize = length(rf);

% This code computes the center of mass
% it is independant of the nb of dimension of the matrix
nbdim = ndims(fr);
ind(nbdim).c = [];
[ind.c] = ind2sub(size(fr), rf);
I = [ind.c] .* repmat(fr(rf), 1, nbdim);
center = sum(I) ./ sum (fr(rf));
end