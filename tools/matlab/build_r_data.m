function [data, data1, data2] = build_r_data (nbexp, group1, group2, is_cell, is_rand, rand1, rand2)
% matlab and octave compatibility

% nbexp = 100
% rand1 = 0.9 pr lk_c, rand2 = 0.6 pr lk_o

data = [];
data1 = [];
data2 = [];
idx=1;
for i=1:nbexp
	if is_cell
		val = group1{i,:};
	else
		val = group1(i,:);
	end
	nb_elt = size(val,2);
	for j=1:nb_elt
		if ~is_rand || rand > rand1
			data (idx,:) = [1 val(j)];
			data1 (idx) = val(j);
			idx = idx + 1;
		end
	end
end

idx2 = 1;
for i=1:nbexp
	if is_cell
		val = group2{i,:};
	else
		val = group2(i,:);
	end
	nb_elt = size(val,2);
	for j=1:nb_elt
		if ~is_rand || rand > rand2
			data (idx,:) = [2 val(j)];
			data2 (idx2) = val(j);
			idx2 = idx2 + 1;
			idx = idx + 1;
		end
	end
end

% autre manière de faire (ici sans echantillonnage)
%g1=[];
%g2=[];
%for i=1:100
%	g1(i)=1;
%	g2(i)=2;
%end
%data = [ [g1';g2'] [aks_c';aks_o'] ];


