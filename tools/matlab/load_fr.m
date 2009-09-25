function [frs] = load_fr (folder_in, folder_out, listn, step_idx, v, probav)
% function [frs] = load_fr (folder_in, folder_out, listn, step, vspace, vars, nbvars, probav)
%
% Param:
% folder_in = path to data_raw/exp/cells for example
% folder_out = path to data_extracted/exp/cells for example
% listn = list of neurones
% v = matrix of all the discretized variables (e.g., x y theta ...)
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
	load(sprintf('%s/%d.mat', folder_in, listn(i)), 'fr');
	[fr] = firing_rate (fr(step_idx), v, size(probav));
	[centers(i,:), fsizes(i), m_frs(i), s_frs(i)] = field_properties (fr, probav);
	frs(i,:) = transpose(fr(idx));
end
save("-mat", filename_all, "frs", "m_frs", "s_frs", "centers", "fsizes");

