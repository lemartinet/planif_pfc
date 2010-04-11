function [aks] = spatial_density(frs)
% aks = number of 'active' cells for each position, i.e. density

nb_neur = size(frs, 1);
thresh = ones(1, size(frs, 2)) * 0.101; 
%thresh = mean(frs) + std(frs);
aks = sum(frs > repmat(thresh, nb_neur, 1));

end
