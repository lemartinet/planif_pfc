function [info, surprise, e, ce, ce_all, sample] = mutual_information_pop (frs_vect, stim)

idx = find (stim > 0);
nb_stim = size(idx, 1);
s = stim(idx);
nb_neuron = size(frs_vect, 1);

% we compute the std of all position & neuron. Not sure that it makes sense...
s_all = mean(cellfun(@std, frs_vect));
% if we use this method, the std is much higher... But maybe we don't overestimate the MI.
#for i=1:size(frs_vect,1)
#	fr_all=[];
#	for j=1:size(frs_vect,2)
#		fr_all=[fr_all frs_vect{i,j}];
#	end
#	s_all(i) = std(fr_all);
#end

% bins for the firing rate
%[std(fr_all), mean(s_all), max(s_all), 0.1]
bin = mean(s_all);
#bin = 0.1;
nb_bin = ceil(1/bin);

% we compute P(r|si)...
% first we bin the patterns
pattern = cell();
% we compute the real number of pattern for each stim
nb_pattern = cellfun("size(x,2)", frs_vect);
%TODO faire un downsampling à 100 prez par stim
% id = find(nb_pattern > 100);
% frs_vect(id) = frs_vect{id}(find(rand(1,nb_pattern(id)) > (1-.100./nb_pattern(id))));
nb_pattern = min(nb_pattern);
if min(nb_pattern) == 0
	printf("erreur (min(nb_pattern) == 0)\n");
end

printf("Building patterns... ");fflush(stdout);
for i=1:nb_stim
	for j=1:nb_pattern(i)
		pat = [];
		for k=1:nb_neuron
			c2m = frs_vect{k,i};
#			c2m = c2m ./ max_fr;
#			% trick to avoir fr==1, which creates an unwanted bin
#			c2m(find(c2m == 1)) = 0.999999;
			pat(k) = fix(c2m(j + size(c2m,2) - nb_pattern(i)) ./ bin);
		end
		pattern(i,j) = pat;
#		fun = sprintf('fix(x(%d+size(x,2)-%d) ./ %f)', j, nb_pattern(i), bin);
#		pattern(i,j) = cellfun(fun, frs_vect(:,i));
	end
end
pattern_total = sum(nb_pattern);

% then we find similar pattern (identified by their number)
printf("counting patterns... ");fflush(stdout);
numero = 1:pattern_total;
idi = 1;
for i=1:size(pattern,1)*size(pattern,2)
	c2mi = pattern{i};
	if isempty (c2mi)
		continue
	end
	idj = 1;
	for j=1:i-1
		c2mj = pattern{j};
		if isempty (c2mj)
			continue
		end
		if isempty (find (c2mi != c2mj))
			numero(idi) = idj;
			break
		end
		idj++;
	end
	idi++;
end

% finally we compute the proba
printf("computing the proba... ");fflush(stdout);
cpt = count_element(numero);
cond_proba = zeros(nb_stim, max(size(cpt.keys)));
for i=1:nb_stim
	idnum = i:nb_stim:pattern_total;
	cpt2 = count_element(numero(idnum));
	cpt2.value /= sum(cpt2.value);
	for j=1:max(size(cpt2.keys))
		idk = find(cpt.keys == cpt2.keys(j));
		cond_proba(i,idk) = cpt2.value(j);
	end
end
time()-t;
% we compute P(r,si) and H(R|si)
joint_proba = [];
ce_all=[];
for i=1:nb_stim
	joint_proba(i,:) = cond_proba(i,:) .* s(i);
	ce_all(i) = -nansum(cond_proba(i,:) .* log2(cond_proba(i,:)));
end

% we compute H(R|si) / log2(N(si))
% should be << 1
sample = ce_all ./ log2(nb_pattern);

% we compute H(R|S)
ce = sum(ce_all .* s');

% we compute P(r) and H(R)
proba_r = sum(joint_proba, 1);
e=-nansum(proba_r .* log2(proba_r));

% now we can compute MI (using sum(...) or using e-ce)
surprise = nansum(cond_proba .* log2 (joint_proba ./ (s * proba_r)), 2)';
info = sum (surprise .* s');

