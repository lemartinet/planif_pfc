function [fr] = firing_rate (r, v, nb_bin)
% function [fr] = firing_rate (r, v, nb_bin)
%
% Param:
% r = temporal firing rate
% v = matrix of all the discretized variable (e.g., x y theta ...)
% nb_bin = description of the space size
%
% Return:
% fr = multidim matrix of recptive field firing rate for each n-uplet varsa x varsb x ... 
%      (a, b, ... being described in nbvars)
%      OR all the firing values in the time range of vars if the first three arguments are given
%
% Example: computes the place field fr(x,y) of the unit
% [pv, v] = proba_var(vars(:, 2:3), [0.05 0.05], [1.17 0; -0.68 -1.7]);
% r = load('../../data/data_raw/2/frs/130.mat', 'fr');
% fr=firing_rate(r(step_idx), v, size(pv));
%
% Note:
% we could use firing_rate_vect to compute firing_rate, using cellfun to apply mean 
% (if issues with NaN, define mymean which filters []->0)

[keys, idx, jdx] = unique(v, 'rows');
keys = num2cell(keys);
fr = zeros(nb_bin);
for i = 1 : size(keys, 1)
	val = r(find (jdx == i));
	fr(keys{i, :}) = mean (val);
end
fr(isnan(fr)) = 0;

