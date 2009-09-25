function linear_graph2 (exp_data)

% on pourrait utiliser la même routine pour calculer la vraie distance au but 
% au lieu de la distance à vol d'oiseau
exp_data.lin_graph.dist = {};
exp_data.lin_graph.neurons = {};
exp_data.lin_graph.fr = {};
for path = 1 : 3
    % d'abord on extrait les distances pour chaque pathway du maze
    v = exp_data.trial(1, path).v_space;
	d = sum((v(2 : end, :) - v(1 : end - 1, :)) .^ 2, 2);
	d = [0; d];
	for j = 1 : length(d)
		d2(j) = sum(d(1 : j));
	end
	D = d2;
	V = v;
    %figure, plot3(V(:,1), V(:,2), D);
    % on garde la distance minimum pour chaque position echantillonnee
    [id, a, b] = unique(V, 'rows');
    for i = 1 : length(id)
        D2(i) = min(D(b == i));
    end
    V = id;
    D = D2;
    %figure, plot3(V(:,1), V(:,2), D, 'o');
    % on trie les tableaux selon la distance
    [D, b] = sort(D);
    V = V(b, :);
    %figure, plot3(V(:,1), V(:,2), D, 'o');

    % on construit la matrice des fr en fonction d'un neurone et d'une position
    idx = exp_data.pv_space > 0;
    p = exp_data.pv_space;
    FR = zeros(exp_data.nbneurons, length(D));
    for i = 1 : exp_data.nbneurons
        p(idx) = exp_data.neuron(i).fr;
        for j = 1 : length(D)
            pos = V(j, :);
            FR(i,j) = p(pos(1), pos(2));
        end
    end
    % on trie les neurones selon la dist de leur centre
    % on met les neurones inactifs à la fin
    [a, b] = max(FR');
    %b(a < 0.07) = max(b) + 1;
    fs = max(FR, [], 2) - min(FR, [], 2);
    b(fs < std(fs)) = max(b) + 1;
    [a, listn] = sort(b);
    %listn = 1 : 1204;
    FR = FR(listn, :);
    exp_data.lin_graph.dist{path} = D;
    exp_data.lin_graph.neurons{path} = listn;
    exp_data.lin_graph.fr{path} = FR;
end

end

