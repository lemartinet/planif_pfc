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

% This code computeq the center of mass
% it is independant of the nb of dimension of the matrix
size_fr = size(fr);
nbdim = size(size_fr, 2);
idfr = find(fr > 0);
center = [];
for i=1:nbdim
	% I: coordinates which will be multiplied by the firing rates to compute the center of mass
	% we want for all j in size_fr(i) I(:,j,:) = j
	I = zeros(size_fr);
	% dim: var to access elements in variable-dimension matrix
	% we want for all j in size_fr(i) dim = {:,j,:}
	dim = cell(1,nbdim);
	dim(:) = {:}; 
	for j=1:size_fr(i)
		dim(i) = j;
		I(dim{:}) = j;
	end
	% we need to apply multiple sum operator to sum all the dimensions
	center(i) = sum(fr(idfr) .* I(idfr));
end

sum_fr = sum (fr(idfr));
center = center ./ sum_fr;
m_fr = sum (fr(idfr) .* probav(idfr));
s_fr = sqrt(sum (fr(idfr) .**2 .* probav(idfr)) - m_fr**2);

% field area estimated by the number of pixel for which the neurone is active
% i.e. have a bigger response than the std value
% (in Johnson&Redish2007, they used a threshold of 20% of max activity)
fsize = size(find(fr > s_fr), 1);


