function [lks, m_lks, s_lks] = lifetime_kurtosis (save_file, frs, probav)

nbcells = size(frs, 1);
idx = find(probav > 0);

% we can try with a constant p(xj)
%nb_stim = size(frs, 2);
%probav(:) = 1 / nb_stim;

for i=1:nbcells
	% we compute mean et std of each cell for all the presented stimuli
	m = sum (frs(i,:) .* probav(idx)');
	s = sqrt( sum (frs(i,:) .**2 .* probav(idx)') - m**2);
	lks(i) = sum(probav(idx)' .* ((frs(i,:) .- m) ./ s).**4) - 3;
%	lks(i) = kurtosis (frs(i,:));
end
m_lks = mean (lks);
s_lks = std (lks);

save ("-mat", save_file, "lks");
