function [MI] = mutual_information(fr_vect, stim)

idx = find(stim > 0);
nb_stim = length(idx);
s = stim(idx);
% we can try with a constant p(xj)
%s(:) = 1 / nb_stim;

% max_fr is used to normalized the firing rates
msN_all = cellfun(@(x) [mean(x), std(x), length(x)], fr_vect, 'UniformOutput', false);
msN_all = cell2mat(msN_all(:));
%m_all = msN_all(:, 1); s_all = msN_all(:, 2); 
N = msN_all(:, 3)';
fr_all = [fr_vect{:}];
max_fr = max(fr_all); min_fr = min(fr_all);

% bins for the firing rate
%std(fr_all), nanmean(s_all), sum(s_all.*s), max(s_all), 0.1
% bin = 10;
% nb_bin = ceil((1 - 0) * bin)
bin = 10 * 1 / (max_fr + 10*eps);
nb_bin = 10; % ceil(length(fr_all)^(1/3));
joint_proba = zeros(nb_stim, nb_bin); % P(r,si)
cond_proba = zeros(nb_stim, nb_bin); % P(r|si)
MI.ce_all = zeros(1, nb_stim); % H(R|si)
Rs = zeros(1, nb_stim);
for i = 1 : nb_stim
	% normalisation 
    % c2m = (fr_vect{i} - min_fr) / (max_fr - min_fr); 
    c2m = fr_vect{i}; 
    % trick to avoid fr=1, which creates an unwanted bin
    % c2m(c2m == 1) = 1 - eps;
    bins = zeros(1, nb_bin);
    cpt = count_element(fix(c2m .* bin));
    bins(1 + cpt.keys) = cpt.value;
    cond_proba(i, :) = bins(1:size(cond_proba, 2)) ./ sum(bins);
	joint_proba(i, :) = cond_proba(i, :) .* s(i);
	MI.ce_all(i) = -nansum(cond_proba(i, :) .* log2(cond_proba(i, :)));
    Rs(i) = length(find(cond_proba(i, :) > 0));
end

MI.sample = MI.ce_all ./ log2(N); % H(R|si) / log2(N(si)), should be << 1
MI.ce = dotprod(MI.ce_all, s); % H(R|S)
proba_r = nansum(joint_proba, 1); % P(r)
R = length(find(proba_r > 0));
MI.e = -nansum(proba_r .* log2(proba_r)); % H(R)
MI.surprise = nansum(cond_proba .* log2(joint_proba ./ (s * proba_r)), 2)';
MI.info = dotprod(MI.surprise, s); % MI (using surprise or e-ce)
MI.correction = (sum(Rs) - R - (nb_stim - 1)) / (2 * sum(N) * log(2));

% formule de Skaggs et al 1996 (fr: firing rate)
%info = sum((fr .* log2 (fr ./ mean(fr)) + (mean(fr) .- fr)./log(2)) .* p);

end
