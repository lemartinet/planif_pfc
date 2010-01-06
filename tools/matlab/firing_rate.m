function [fr] = firing_rate (r, v, nb_bin, vect)
% function [fr] = firing_rate (r, v, nb_bin[, vect])
%
% Param:
% r = temporal firing rate
% v = matrix of all the discretized variable (e.g., x y theta ...)
% nb_bin = description of the space size
% vect = optional bool to indicate that we want the vector of all the fr
%
% Return:
% fr = multidim matrix of mean firing rate (or multidim cell array of all 
%       the firing rates if vect is given) for each n-uplet v1 x v2 x ... 
%
% Example: computes the place field fr(x,y) of the unit
% [pv, v] = proba_var(vars(:, 2:3), [0.05 0.05], [1.17 0; -0.68 -1.7]);
% r = load('../../data/data_raw/2/frs/130.mat', 'fr');
% fr = firing_rate(r(step_idx), v, size(pv));

if nargin == 3
    vect = false;
end

[keys, idx, jdx] = unique(v, 'rows');
keys = num2cell(keys);
if ~vect
    fr = zeros(nb_bin);
else
    fr = cell(nb_bin);
end
for i = 1 : size(keys, 1)
    if ~vect
        fr(keys{i, :}) = mean(r(jdx == i));
    else
        fr{keys{i, :}} = r(jdx == i)';
    end
end
if ~vect
    fr(isnan(fr)) = 0;
end

end
