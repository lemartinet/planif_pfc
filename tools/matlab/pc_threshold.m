function [] = pc_threshold (folder_in, dmax)
% script to correct the firing rate of the place cells computed by a real model
% in order to have single place fields
% dmax: 0.4 for example
centers = load(sprintf("%s/pref_pos.dat", folder_in));
for i = 1:size(centers, 1)
	filename = sprintf('%s/cells/old/cell_%d.dat', folder_in, i-1); 
	fr = load(filename);
	d = sqrt((fr(:,1) - centers(i,1)).**2 + (fr(:,2) - centers(i,2)).**2);
	g = fr(:,3);
	% we delete responses too far from the center of the receptive field
	g = g ./(1 + exp(30 * (d - 0.4)));
	% we add some noise to avoid low responses within [0, 0.1]
	low = find(g < 0.1);
	g(low) = rand(size(low, 1), 1) / 10;
	fout = [fr(:,1) fr(:,2) g];
	filename = sprintf ('%s/cells/%d.txt', folder_in, i-1);
	save ('-ascii', filename, 'fout');
end


