function [MI] = mutual_information_pop(fr_vect, stim)
% TODO faire un downsampling à 1000 prez par stim

idx = find(stim > 0);
nb_stim = size(idx, 1);
s = stim(idx);
% we can try with a constant p(xj)
%s(:) = 1 / nb_stim;

% max_fr is used to normalized the firing rates
%msN_all = cellfun(@(x) [mean(x), std(x), size(x, 2)], fr_vect, 'UniformOutput', false);
%msN_all = cell2mat(msN_all(:));
%m_all = msN_all(:, 1); s_all = msN_all(:, 2);
fr_all = [fr_vect{:}];
max_fr = max(fr_all); min_fr = min(fr_all);

% % first we bin the patterns
%std(fr_all), nanmean(s_all), sum(s_all.*s), max(s_all), 0.1
fprintf('Building patterns... ');
bin = 10;
nb_pattern = zeros(1, nb_stim);
pattern = cell(1, nb_stim);
for i = 1 : nb_stim
    c2m = (cell2mat(fr_vect(:, i)) - min_fr) ./ (max_fr - min_fr);
    c2m(c2m == 1) = 1 - eps;
	pattern{i} = fix(c2m .* bin)';
    nb_pattern(i) = size(pattern{i}, 1);
end

% then we find similar pattern (identified by their number)
[keys] = unique(cell2mat(pattern(:)), 'rows');

% finally we compute the proba P(r_pop|si), P(r_pop,si) and H(R_pop|si)
cond_proba = zeros(nb_stim, size(keys, 1));
joint_proba = zeros(nb_stim, size(keys, 1));
MI.ce_all = zeros(1, size(keys, 1));
for i = 1 : nb_stim
    [keys2, idx2, jdx2] = unique(pattern{i}, 'rows');
    for j = 1 : size(keys2, 1)
		cond_proba(i, ismember(keys, keys2(j, :), 'rows')) = length(find(jdx2 == j));
    end
    cond_proba(i, :) = cond_proba(i, :) ./ sum(cond_proba(i, :));
    joint_proba(i,:) = cond_proba(i,:) .* s(i);
	MI.ce_all(i) = -nansum(cond_proba(i,:) .* log2(cond_proba(i,:)));
end

MI.sample = MI.ce_all ./ log2(nb_pattern);
MI.ce = dotprod(MI.ce_all, s);
proba_r = nansum(joint_proba, 1);
MI.e = -nansum(proba_r .* log2(proba_r));
MI.surprise = nansum(cond_proba .* log2(joint_proba ./ (s * proba_r)), 2)';
MI.info = sum(MI.surprise .* s');

end
