function [pks, m_pks, s_pks] = population_kurtosis (save_file, frs, poolsize, probav)

nbcells = size(frs,1);
nb_stim = size(frs,2);
pks = [];
for i=1:nb_stim
	% we complete the population to have poolsize units
	tmp = frs(find(frs(:,i) < std (frs(:,i))));
	if isempty(tmp)
		% adding a non zero noise
		noise = 0.001; 
	else	
		noise = mean (tmp);
	end	
	frs(nbcells+1:poolsize, i) = noise;
	pks(i) = kurtosis (frs(:,i));
end
idx = find(probav > 0);
m_pks = sum (pks .* probav(idx)');
s_pks = sqrt( sum (pks .**2 .* probav(idx)') - m_pks**2);

save ("-mat", save_file, "pks");
