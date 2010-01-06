function [aks, paks] = active_kurtosis(frs, max_activated_pop)
% aks = number of 'active' cells for each position, i.e. density
% pks = proba distribution of the aks

nb_places = size(frs, 2);
aks = zeros(1, nb_places);
for i = 1 : nb_places
	aks(i) = size(find(frs(:, i) > std(frs(:, i))), 1);	
end
paks = zeros(1, max_activated_pop);
for i = 1 : max_activated_pop
	paks(i) = size(find(aks == i), 2) / nb_places;
end

end