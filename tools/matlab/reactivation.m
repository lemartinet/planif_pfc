function [R] = reactivation (v, l, Q, nb_pc)

Q = zscore(Q')';
if nargin == 3
	q = size(Q,2) / size(Q,1);
	sup = (1 + sqrt(1 / q)) ^ 2;
	nb_pc = max(find(l > sup));
end

for i=1:nb_pc
	PC = v(:,i) * v(:,i)';
	PC = PC - diag(diag(PC));
	for j=1:size(Q,2)
		R(i,j) = Q(:,j)' * PC * Q(:,j);
	end
end

