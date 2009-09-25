function [C, v, l, id, sup] = pca_stuff (Q, nb_pc)

% prendre le Z-score ?
Q = zscore(Q')';
figure, imagesc(Q);
C = corrcoef(Q');
C(isnan(C)) = 0;
[v,l] = pcacov(C);
f = figure; plot (l,'-o');

if nargin == 1
	q = size(Q,2) / size(Q,1);
	sup = (1 + sqrt(1 / q)) ^ 2;
	nb_pc = max(find(l > sup));
	figure(f), hold on, plot (ones(1, size(l,1)) * sup, 'red');
end

for i=1:nb_pc
	pc = v(:,i);
	[k,id(i,:)] = sort(pc);
	figure, imagesc(Q(id(i,:), :));
end

