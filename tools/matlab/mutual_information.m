function [info, surprise, e, ce, ce_all, sample] = mutual_information (fr_vect, stim)

idx = find (stim > 0);
nb_stim = size(idx, 1);
s = stim(idx);
% we can try with a constant p(xj)
%s(:) = 1 / nb_stim;

% max_fr is used to normalized the firing rates
max_fr = 0;
fr_all = [];
m_all = [];
s_all = [];
N = [];
for i=1:size(fr_vect, 1)
	c2m  = fr_vect{i};
	if isempty(c2m)
		s_all (i) = nan;
		N(i) = nan;
		continue;
	end
	max_fr = max ([max_fr, c2m]);
	fr_all = [fr_all c2m];
	s_all (i) = std(c2m);
	N(i) = size(c2m, 2);
end
#s_all

% bins for the firing rate
%[std(fr_all), mean(s_all), sum(s_all.*s'), max(s_all), 0.1]
bin = nanmean(s_all);
nb_bin = ceil(1/bin);

% we compute P(r|si), P(r,si) and H(R|si)
joint_proba = [];
cond_proba = [];
ce_all=[];
for i=1:nb_stim
	c2m = fr_vect{i} ./ max_fr;
	% trick to avoir fr==1, which creates an unwanted bin
	c2m(find(c2m == 1)) = 0.999999;
	
	% trick to avoid bug for neurons
	% without sampling in place i (don't know if it's still useful)
	if isempty(c2m)
		cond_proba(i,:) = zeros (1,nb_bin) .* nan;
	else
		bins = zeros(1,nb_bin);
		cpt = count_element(fix(c2m ./ bin));
		bins(1+cpt.keys) = cpt.value;
		bins = bins ./ sum(bins);
		cond_proba(i,:) = bins;
	end
	joint_proba(i,:) = cond_proba(i,:) .* s(i);
	ce_all(i) = -nansum(cond_proba(i,:) .* log2(cond_proba(i,:)));
end

% we compute H(R|si) / log2(N(si))
% should be << 1
sample = ce_all ./ log2(N);

% we compute H(R|S)
ce = sum(ce_all .* s');

% we compute P(r) and H(R)
proba_r = nansum(joint_proba, 1);
e=-nansum(proba_r .* log2(proba_r));

% now we can compute MI (using sum(...) or using e-ce)
surprise = nansum(cond_proba .* log2 (joint_proba ./ (s * proba_r)), 2)';
info = sum (surprise .* s');

% formule de Skaggs et al 1996
% fr: firing rate
%info = sum((fr .* log2 (fr ./ mean(fr)) + (mean(fr) .- fr)./log(2)) .* p);

