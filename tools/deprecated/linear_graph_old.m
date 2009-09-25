function linear_graph (exp_data)

% on pourrait utiliser la même routine pour calculer la vraie distance au but 
% au lieu de la distance à vol d'oiseau

% d'abord on extrait les distances pour chaque pathway du maze
V = [];
D = [];
for i = 1 : 3
	v = exp_data.trial(1,i).v_space;
	d = sum((v(2 : end, :) - v(1 : end - 1, :)) .^ 2, 2);
	d = [0; d];
	for j = 1 : length(d)
		d2(j) = sum(d(1 : j));
	end
	D = [D, d2];
	V = [V; v];
end
%figure, plot3(V(:,1), V(:,2), D);
% on garde la distance minimum pour chaque position echantillonnee
[id, a, b] = unique(V, 'rows');
for i = 1 : length(id)
	D2(i) = min(D(b == i));
end
V = id;
D = D2;
%figure, plot3(V(:,1), V(:,2), D, 'o');
% on trie les tableaux selon la distance (P1, P2 et P3 seront mélangés)
[D, b] = sort(D);
V = V(b, :);
%figure, plot3(V(:,1), V(:,2), D, 'o');

% on construit la matrice des fr en fonction d'un neurone et d'une position
idx = exp_data.pv_space > 0;
p = exp_data.pv_space;
% FR = zeros(length(D), exp_data.nbneurons);
% path = zeros(1, exp_data.nbneurons);
% d = zeros(1, exp_data.nbneurons);
% for i = 1 : exp_data.nbneurons
FR = zeros(length(D), 200);
path = zeros(1, 200);
d = zeros(1, 200);
for i = 1 : 200
	n = exp_data.neuron(i).center;
	% pour un (x,y) = center donné, on trouve l'élt le plus pres dans la
	% liste
	[a, b] = min(sum((V - repmat(n, size(V, 1), 1)) .^ 2, 2));
	% on prend sa dist et on ajoute la dist de (x,y) vers cet elt
	d(i) = sqrt(D(b)) + sqrt(a); % A REVOIR
    [sqrt(D(b)), sqrt(a)];
	
	p(idx) = exp_data.neuron(i).fr;
	for j = 1 : length(D)
		pos = V(j, :);
		FR(j,i) = p(pos(1), pos(2));
	end

	% on determine quel pathway
	if max(ismember(exp_data.trial(1,1).v_space, V(b, :), 'rows')) == 1
		path(i) = 1;
	elseif max(ismember(exp_data.trial(1,2).v_space, V(b, :), 'rows')) == 1
		path(i) = 2;
	else
		path(i) = 3;
	end
	
end
% on trie les neurones selon la dist de leur centre et leur pathway
% listn = [];
% for i = 1:3
% 	id = find(path == i);
% 	[a, b] = sort(d(id));
% 	listn = [listn, b];
% end
[a, listn] = sort(d);
FR = FR(:, listn);

exp_data.lin_graph.dist = D;
exp_data.lin_graph.neurons = listn;
exp_data.lin_graph.fr = FR;

end

