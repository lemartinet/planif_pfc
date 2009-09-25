function [exp_data] = linear_graph (exp_data)

% on pourrait utiliser la même routine pour calculer la vraie distance au but 
% au lieu de la distance à vol d'oiseau

% d'abord on extrait les distances pour un ensemble de coordonnées du maze
V = [];
D = [];
for i = 3:-1:1
	v(i) = exp_data.trial(1,i).v_space;
	d = sum((v(2 : end, :) - v(1 : end - 1, :)) .^ 2, 2);
	d = [0 ; d];
	for j = 1:size(d,1)
		d2(j) = sum(d(1:j));
	end
	D = [D; d2];
	V = [V; v];
end
% on garde le minimum pour chaque position echantillonnee
[id, a, b] = unique(V, 'rows');
for i = 1:size(id,1)
	D2(i) = min(D(find(b == i)));
end
V = id;
D = D2;
% on trie les tableaux selon la distance (P1, P2 et P3 sont mélangés)
[D, b] = sort(D);
V = V(b);

% on construit la matrice des fr en fonction d'un neurone et d'une position/distance
idx = find(exp_data.pv_space > 0);
p = pv_space;
FR = []
for i = 1:size(exp_data.nbneurons)
	n = exp_data.neuron(i);
	% pour un (x,y) = n.center donné, on trouve l'élt le plus pres dans la liste
	[a, b] = min(sum((V - repmat(n.center, size(V, 1), 1) .^ 2, 2));
	% on prend sa dist et on ajoute la dist de (x,y) vers cet elt
	d(i) = sqrt(D(b) + sum((V(b,:) - n.center) .^ 2));
	
	p(idx) = exp_data.neuron(i).fr;
	for j = 1:size(V, 1)
		pos = V(j,:);
		FR(j,i) = p(pos(1), pos(2));
	end

	% on determine quel pathway
	if sum(ismember(v(1), round(exp_data.nbneuron(i).center), 'rows')) > 0
		path(i) = 1;
	elseif sum(ismember(v(2), round(exp_data.nbneuron(i).center), 'rows')) > 0
		path(i) = 2;
	else
		path(i) = 3;
	end
	
end
% on trie les neurones selon la dist de leur centre et leur pathway
listn = [];
for i = 1:3
	id = find(path == i);
	[a, b] = sort(d(id));
	listn = [listn b];
end
FR = FR(:, listn);

exp_data.lin_graph.dist = D;
exp_data.lin_graph.neurons = listn;
exp_data.lin_graph.fr = FR

% restreindre le résultat à seulement une liste de neurones
exp_data.lin_graph.sublist_fr = @(x) exp_data.lin_graph.fr(:, ismember(exp_data.lin_graph.neurons, x));

