function [pv, v] = proba_var (vars, step, vspace)
% function [pv, v] = proba_var (vars, step, vspace)
%
% Param:
% vars = matrix of the vars used for this proba (e.g., x y...)
% step = step to discretize each var (e.g., [0.05 0.01] for two vars)
% vspace [optional] = size of the space for each var (e.g., [xMax yMax tMax; xMin yMin tMin])
%			we keep this parameter to output a fixed size matrix for receptive field plots
%
% Return:
% pv = multidim matrix of the proba for each n-uplet varsa x varsb x ... 
%      (a, b, ... being described in nbvars)
% v = discretized vars
%
% Example: computes the occupancy grid (proba of being at each (x,y) of the maze)
% vars = load_vars('../../data/data_raw/2/vars.txt', 1, 1:12);
% [pv, v] = proba_var(vars(:, 2:3), [0.05 0.05], [1.17 0; -0.68 -1.7]);

nb_stim = size(vars, 1);	
nb_var = size(vars, 2);
if nargin == 3
	v_max = vspace(1,:);
	v_min = vspace(2,:);
else
	v_max = max(vars);
	v_max = v_max + step; % v_max = borne sup non atteinte
	v_min = min(vars);
	v_min = v_min - step; % v_min = borne inf non atteinte
end
nb_bin = ceil((v_max - v_min) ./ step);
if nb_var == 1
	nb_bin = [nb_bin 1];
end
v = ceil((vars - repmat(v_min, nb_stim, 1)) ./ repmat(step, nb_stim, 1));
[keys, idx, jdx]  = unique(v, 'rows');
value = sparse(jdx, 1, 1);
pv = zeros(nb_bin);
nb_total = 0;
keys = num2cell(keys);
for i=1:size(keys,1)
	pv (keys{i,:}) = value(i,1);
	nb_total = nb_total + value(i,1);
end
pv = pv ./ nb_total;

