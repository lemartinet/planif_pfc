function [fr] = firing_rate_vect (file, vars, nbvars, step, vspace)
% function [fr] = firing_rate_vect (file, vars, nbvars, step, vspace)
%
% Param:
% file = file of recordings for a cell
% vars = matrix of all the variable (e.g., x y theta ...)
% nbvars = column number of the vars used for this fr
% step = step to discretize each var (e.g., [0.05 0.01] for two vars)
% vspace = size of the space for each var (e.g., [xMax yMax tMax; xMin yMin tMin])
%
% Return:
% fr = multidim cell array of all the firing rates for each n-uplet varsa x varsb x ... 
%      (a, b, ... being described in nbvars)
%
% Example:
% vars=load_vars('../../data/data_raw/2/vars.txt', 1, 1:12);
% fr=firing_rate_vect('../../data/data_raw/2/frs/130.txt', vars, 2:3, [0.05 0.05], [1.17 0; -0.68 -1.7]);
% This computes all the responses fr(x,y) of the unit

r = load(file);
% TODO: revoir ce idx pour calculer pas forcément depuis le premier jour
idx = 1;	
nb_stim = size(vars, 1);	
r = r(idx:idx-1+nb_stim, 1);
nb_var = size(nbvars, 2);
v_max = vspace(1,:);
v_min = vspace(2,:);
nb_bin = ceil((v_max .- v_min) ./ step);
if nb_var == 1
	nb_bin = [nb_bin 1];
end
v = ceil((vars(:,nbvars) .-  repmat(v_min, nb_stim, 1)) ./  repmat(step, nb_stim, 1));
[keys,idx,jdx]  = unique(v, 'rows');
fr=cell(nb_bin);
for i=1:size(keys,1)
	fr (num2cell(keys(i,:)){:}) = r(find (jdx == i))';
end


