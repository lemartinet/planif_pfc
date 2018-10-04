function [fr] = firing_rate (file, step_idx, vars, nbvars, step, vspace)
% function [fr] = firing_rate (file, step_idx, vars, nbvars, step, vspace)
%
% Param:
% file = file of recordings for a cell
% step_idx = list of step indexes according to the days and trials
% vars = matrix of all the variable (e.g., x y theta ...)
% nbvars = column number of the vars used for this fr
% step = step to discretize each var (e.g., [0.05 0.01] for two vars)
% vspace [optional] = size of the space for each var (e.g., [xMax yMax tMax; xMin yMin tMin])
%			we keep this parameter to output a fixed size matrix for receptive field plots
%
% Return:
% fr = multidim matrix of mean firing rate for each n-uplet varsa x varsb x ... 
%      (a, b, ... being described in nbvars)
%      OR all the firing values in the time range of vars if the first three arguments are given
%
% Example:
% vars=load_vars('../../data/data_raw/2/vars.txt', 1, 1:12);
% fr=firing_rate('../../data/data_raw/2/frs/130.txt', vars, 2:3, [0.05 0.05], [1.17 0; -0.68 -1.7]);
% This computes the place field fr(x,y) of the unit
%
% Note:
% we could use firing_rate_vect to compute firing_rate, using cellfun to apply mean 
% (if issues with NaN, define mymean which filters []->0)

r = load(file);
r = r(step_idx);
if nargin == 2
	fr = r;
	return;
end
nb_stim = size(vars, 1);	
nb_var = size(nbvars, 2);
if nargin == 6
	v_max = vspace(1,:);
	v_min = vspace(2,:);
else
	v_max = max(vars(:,nbvars));
	v_max .+= step; % v_max = borne sup non atteinte
	v_min = min(vars(:,nbvars));
	v_min .-= step; % v_min = borne inf non atteinte
end
nb_bin = ceil((v_max .- v_min) ./ step);
if nb_var == 1
	nb_bin = [nb_bin 1];
end
v = ceil((vars(:,nbvars) .-  repmat(v_min, nb_stim, 1)) ./  repmat(step, nb_stim, 1));
[keys,idx,jdx]  = unique(v, 'rows');
fr=zeros(nb_bin);
for i=1:size(keys,1)
	v = r(find (jdx == i));
	if isempty(v)
		fr (num2cell(keys(i,:)){:}) = 0;
	else 
		fr (num2cell(keys(i,:)){:}) = mean (v);
	end
end


