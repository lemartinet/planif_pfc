function [pv] = proba_var (save_file, vars, nbvars, step, vspace)
% function [pv] = proba_var (save_file, vars, nbvars, step, vspace)
%
% Param:
% save_file = to save the proba
% vars = matrix of all the variable (e.g., x y theta ...)
% nbvars = column number of the vars used for this proba
% step = step to discretize each var (e.g., [0.05 0.01] for two vars)
% vspace [optional] = size of the space for each var (e.g., [xMax yMax tMax; xMin yMin tMin])
%			we keep this parameter to output a fixed size matrix for receptive field plots
%
% Return:
% pv = multidim matrix of the proba for each n-uplet varsa x varsb x ... 
%      (a, b, ... being described in nbvars)
%
% Example:
% vars = load_vars('../../data/data_raw/2/vars.txt', 1, 1:12);
% pv = proba_var(vars, 2:3, [0.05 0.05], [1.17 0; -0.68 -1.7]);
% This computes the occupancy grid (proba of being at each (x,y) of the maze)

% Can be used to save the proba
%if exist (save_file, "file") == 2
	% the file already exist, we simply load it
%	load ("-mat", save_file, 'pv');
%	return;
%end

nb_stim = size(vars, 1);	
nb_var = size(nbvars, 2);
if nargin == 5
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
value = sparse(jdx,1,1);
pv = zeros(nb_bin);
nb_total = 0;
for i=1:size(keys,1)
	pv (num2cell(keys(i,:)){:}) = value(i, 1);
	nb_total += value(i, 1);
end
pv ./= nb_total;
save ("-mat", save_file, "pv");

