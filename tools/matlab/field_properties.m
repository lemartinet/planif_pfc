function [center, fsize, m_fr, s_fr] = field_properties (fr, probav)
% [center, fsize, m_fr, s_fr] = field_properties (fr, probav)
% 
% Param:
% fr = multidim matrix of mean firing rate for each n-uplet varsa x varsb x ... 
% probav = matrix of the proba for the var, e.g occupancy grid for x,y vars
%
% Return:
% center = estimated center of the bigger receptive field
% fsize = estimated size of the receptive fields (bigger than 2 pixels)
% m_fr = mean firing rate (take into account the occupancy probability)
% s_fr = std firing rate (take into account the occupancy probability)

m_fr = sum (fr(:) .* probav(:));
s_fr = sqrt(sum (fr(:) .^ 2 .* probav(:)) - m_fr .^ 2);

% field area estimated by the number of pixel for which the neurone is active
%[rfx rfy] = find(fr > 0.2 * max(fr(:)));
[rfx rfy] = find(fr > m_fr + s_fr);
rf = [];
done = [];
nb_rf = 0;
fsize = 0;
for i = 1 : length(rfx)
    if ismember(i, done)
        continue;
    end
    rf_set = [rfx(i), rfy(i)];
    done = [done, i];
    old_size = 0;
    while size(rf_set, 1) > old_size
        old_size = size(rf_set, 1);
        for j = 1 : length(rfx)
            v = [rfx(j), rfy(j)];
            for k = 1 : size(rf_set, 1)
                if voisin(v, rf_set(k, :)) && ~ismember(v, rf_set, 'rows')
                    done = [done, j];
                    rf_set = [rf_set; v];
                    break;
                end
            end
        end
    end
    if size(rf_set, 1) > 2
        nb_rf = nb_rf + 1;
        rf{nb_rf} = rf_set;
        fsize(nb_rf) = size(rf_set, 1);
    end
end

% This code computes the center of mass
% it is independant of the nb of dimension of the matrix (not anymore !!!)
if fsize > 0
    [smax, idmax] = max(fsize);
    rf = rf{idmax};
    rf = sub2ind(size(fr), rf(:,1), rf(:,2));
    nbdim = ndims(fr);
    ind(nbdim).c = [];
    [ind.c] = ind2sub(size(fr), rf);
    I = [ind.c] .* repmat(fr(rf), 1, nbdim);
    center = sum(I) ./ sum (fr(rf));
else
    center = [0, 0];
end

end

function [is_voisin] = voisin(v1, v2)
    is_voisin = sum((v1 - v2) .^ 2) <= 1;
end
