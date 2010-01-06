function [MI] = mutual_information(fr_vect, stim)

idx = find(stim > 0);
nb_stim = size(idx, 1);
s = stim(idx);
% we can try with a constant p(xj)
%s(:) = 1 / nb_stim;

% max_fr is used to normalized the firing rates
msN_all = cellfun(@(x) [mean(x), std(x), size(x, 2)], fr_vect, 'UniformOutput', false);
msN_all = cell2mat(msN_all);
%m_all = msN_all(:, 1); s_all = msN_all(:, 2); 
N = msN_all(:, 3)';
fr_all = [fr_vect{:}];
max_fr = max(fr_all); min_fr = min(fr_all);

% bins for the firing rate
%std(fr_all), nanmean(s_all), sum(s_all.*s), max(s_all), 0.1
bin = 10;
nb_bin = ceil((max_fr - min_fr) * bin);
joint_proba = zeros(nb_stim, nb_bin); % P(r,si)
cond_proba = zeros(nb_stim, nb_bin); % P(r|si)
MI.ce_all = zeros(1, nb_stim); % H(R|si)
for i = 1 : nb_stim
	% normalisation 
    c2m = (fr_vect{i} - min_fr) / (max_fr - min_fr); 
    % trick to avoid fr=1, which creates an unwanted bin
    c2m(c2m == 1) = 1 - eps;
    bins = zeros(1, nb_bin);
    cpt = count_element(fix(c2m .* bin));
    bins(1 + cpt.keys) = cpt.value;
    bins = bins ./ sum(bins);
    cond_proba(i, :) = bins;
	joint_proba(i, :) = bins .* s(i);
	MI.ce_all(i) = -nansum(bins .* log2(bins));
end

MI.sample = MI.ce_all ./ log2(N); % H(R|si) / log2(N(si)), should be << 1
MI.ce = dotprod(MI.ce_all, s); % H(R|S)
proba_r = nansum(joint_proba, 1); % P(r)
MI.e = -nansum(proba_r .* log2(proba_r)); % H(R)
MI.surprise = nansum(cond_proba .* log2(joint_proba ./ (s * proba_r)), 2)';
MI.info = dotprod(MI.surprise, s); % MI (using surprise or e-ce)

% formule de Skaggs et al 1996 (fr: firing rate)
%info = sum((fr .* log2 (fr ./ mean(fr)) + (mean(fr) .- fr)./log(2)) .* p);

end
