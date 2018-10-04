function [frs] = load_fr (folder_in, folder_out, listn, step, vspace, step_idx, vars, nbvars, probav)
% function [frs] = load_fr (folder_in, folder_out, listn, step, vspace, vars, nbvars, probav)
%
% Param:
% folder_in = path to data_raw/exp/cells for example
% folder_out = path to data_extracted/exp/cells for example
% listn = list of neurones
% step = step to discretize each var (e.g., [0.05 0.01] for two vars)
% vspace = size of the space for each var (e.g., [xMax yMax tMax; xMin yMin tMin])
% vars = matrix of all the variable (e.g., x y theta ...)
% nbvars = column number of the vars used for the fr
% probav = matrix of the proba for the var, e.g occupancy grid for x,y vars
%
% Return:
% frs = matrix of firing rates for each neurone and for each n-uplet varsa x varsb x ... 
%      (a, b, ... being described in nbvars)
% additionally the matrix is saved into file named "frs" inside dir
% in data_path/data_extracted/exp/

% we get the index of positive probav
idx = find (probav > 0);
m_frs = [];
s_frs = [];
centers = [];
fsizes = [];

filename_all = sprintf ("%s/frs_%s.mat", folder_out, mat2str(nbvars));
if exist (filename_all, "file") == 2
	% the file already exist, we simply load it
	input = load("-mat", filename_all);
	frs = input.frs;
	return;
end

% we compute the firing rates for the positive probav
for i = 1:size(listn,2)
	printf ("Computation of firing rates for %d\n", listn(i)); fflush(stdout);
	filename = sprintf ("%s/%d.txt", folder_in, listn(i));
	[fr] = firing_rate (filename, step_idx, vars, nbvars, step, vspace);
	[centers(i,:), fsizes(i), m_frs(i), s_frs(i)] = field_properties (fr, probav);
	frs(i,:) = transpose(fr(idx));
end
save("-mat", filename_all, "frs", "m_frs", "s_frs", "centers", "fsizes");

