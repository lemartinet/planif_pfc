function [pks] = population_kurtosis(frs, poolsize)

[nbcells, nb_stim] = size(frs);
pks = zeros(1, nb_stim);
for i = 1:nb_stim
	% we complete the population to have poolsize units
	tmp = frs(frs(:, i) < std(frs(:, i)));
	if isempty(tmp)
		% adding a non zero noise
		noise = 0.001; 
	else	
		noise = mean(tmp);
	end	
	frs(nbcells + 1 : poolsize, i) = noise;
	pks(i) = kurtosis(frs(:, i));
end

end
