function [aks, paks, ak] = active_kurtosis (save_file, frs, max_activated_pop)

nb_places = size(frs, 2);

aks = [];
for i=1:nb_places
	aks(i) = size(find(frs(:,i) > std(frs(:,i))), 1);	
end
paks = [];
for i=1:max_activated_pop
	paks(i)= size(find(aks == i), 2) / nb_places;
end
ak = kurtosis (paks);

save ("-mat", save_file, "aks", "paks", "ak");
