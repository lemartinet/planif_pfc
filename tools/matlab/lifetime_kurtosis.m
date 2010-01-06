function [lks] = lifetime_kurtosis(frs, probav)

nbcells = size(frs, 1);
idx = probav > 0;

% we can try with a constant p(xj)
%nb_stim = size(frs, 2);
%probav(:) = 1 / nb_stim;

lks = zeros(1, nbcells);
for i = 1 : nbcells
	m = sum(frs(i, :) .* probav(idx)');
	s = sqrt(sum(frs(i, :) .^ 2 .* probav(idx)') - m ^ 2);
	lks(i) = sum(probav(idx)' .* ((frs(i, :) - m) ./ s) .^ 4) - 3;
%	lks(i) = kurtosis (frs(i,:));
end

end
