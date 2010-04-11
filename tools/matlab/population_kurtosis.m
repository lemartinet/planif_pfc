function [pks] = population_kurtosis(frs)
% function [pks] = population_kurtosis(frs)

pks = kurtosis(frs) - 3;

end
