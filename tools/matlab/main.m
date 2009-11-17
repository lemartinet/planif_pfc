function main(data_path, exp, reload)

exp_all1 = zeros(length(exp), 3);
exp_all2 = zeros(length(exp), 3);
exp_all3 = zeros(length(exp), 3);
exp_fst1 = zeros(length(exp), 3);
exp_fst2 = zeros(length(exp), 3);
exp_fst3 = zeros(length(exp), 3);
for i = exp	
	fprintf('Working on experiment %d\n', i);
    exp_data = ExpData(data_path, i, reload);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Analyse du comportement de base
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    [m_all1, m_fst1] = exp_data.get_mean_behavior(1);
    exp_all1(i, :) = m_all1; exp_fst1(i, :) = m_fst1; 
    [m_all2, m_fst2] = exp_data.get_mean_behavior(2:14);
    exp_all2(i, :) = m_all2; exp_fst2(i, :) = m_fst2; 
    [m_all3, m_fst3] = exp_data.get_mean_behavior(15);
    exp_all3(i, :) = m_all3; exp_fst3(i, :) = m_fst3; 

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Analyse  neuronale de base
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%     exp_data.linear_graph_plot(1, 1:200);
%     exp_data.linear_graph_plot(2, 1:200);
%     exp_data.linear_graph_plot(3, 1:200);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Analyse theta
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % TODO:
    % * analyser par periode de forte cohérence (à calculer par ex
    %   multiunit, i.e. à chaque pas de temps on somme l'activite
    %   des neurones, voir karim)
    % * ajouter une analyse stat pour voir si modulation est
    %   significative (voir Siapas2005, test de raleigh)
    % * fitter avec une distrib de von Mises
    
%     exp_data.plot_theta(exp_data.from_type(0));
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Analyse PCA et Reactivation
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % TODO pour la PCA
    % * regarder le type de neurone, le ch récep
    % * regarder la matrice des poids (distance comp. principales <-> W)
    % * regarder pendant les ripples
    % * composante modulée par theta ?
    
    % prendre le dernier jour sur tous les neurones
%     Q = [];
%     for i = 1:6
%         Q = [Q , exp_data.getQ(16, i)];
%     end
%     figure, imagesc(Q);
%     [C, v, l, sup, id] = pca_stuff (Q, 1);
%     
%     % affiche la valeur selon la pc1 ordonnée
%     pc1 = v(:,1);
%     figure, plot(pc1(id), 1:length(id))
%     % affiche seulement le Q et val selon pc1 des 200 cellules faibles <0 selon pc1
%     figure, imagesc(Q(id(1:200), :));
%     %axis xy; % pour mettre les axes dans l'ordre croissant
%     %xlim([2200 2400]); % pour reduire l'axe des x
%     figure, plot(pc1(id(1:200)), 1:200);
%     % affiche les numéros
%     figure, plot(id(1:200), 1:200, '-o');
%     % il faut étudier ceux qui ont un signe + selon pc1
%     figure, imagesc(Q(id(end-100:end), :)), axis xy;
%     figure, plot(pc1(id(1104:1204)), 1:101);
%     figure, plot(id(end-100:end), 1:101, '-o');
%     
%     % TODO: calculer reactivation sur toute l'exp ou des sous-parties.
%     % R modulé par theta ? Champ spatial de R ?
%     [R] = reactivation (v, l, Q, 1);
%     figure, plot(R);
%     
%     % analyse sur le smooth Qs (réechantillonnée sur un cycle theta)
%     Qs = [];
%     for i = 1:6
%         Qs = [Qs , exp_data.getQs(16, i)];
%     end
%     figure, imagesc(Qs);
%     [Cs, vs, ls, sups, ids] = pca_stuff (Qs, 1);
%     [Rs] = reactivation (vs, ls, Qs, 1);
%     % comparaison des résultats
%     pc1s = vs(:,1);
%     figure, plot(pc1, pc1s, '.');
%     figure, plot(R, Rs, '.');
    
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Analyse des poids
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    % en fonction d'une seuil, savoir les neurones connectés
    % matrice de poids
    % extraire les poids entre deux types de pop (en particulier HP et State)
    % poids dans les composantes principales
    
    % charger tous les poids
%     for i=0:204
%         load(sprintf('weight/%d.mat', i), 'w');
%         W{i+1} = w;
%     end
%     
%     % pour etudier l'evolution pendant essai
%     % 140-141 et 147-148 = sommeil, 141-147 = essai
%     % i.e. les sommeils sont entre %7 et %7+1
%     figure, plot(W{141}(:),(W{147}(:)-W{141}(:))./W{141}(:),'.');
%     ylim([-8 10]);
%     hold on, line([-9 1],[0 0],'Color','k');
%     hold on, line([0 0],[-8 10],'Color','k');
%     % si on veut pas en relatif, on enleve le ./W{141}(:)
%     
%     % pour afficher la distrib des poids avt et après appr
%     % on peut voir qu'il y a bcp de poids négatifs
%     % faut-il borner les poids, utiliser une règle multiplicative ?
%     test=W{141}(:);
%     figure, hist(test(test~=0),30);
%     test=W{147}(:);
%     figure, hist(test(test~=0),30);
%     % et pour superposer les 2 distrib
%     test=W{141}(:);
%     h=hist(test(test~=0),-10:0.1:2);
%     plot(h);
%     hold on;
%     test=W{147}(:);
%     h=hist(test(test~=0),-10:0.1:2);
%     plot(h, 'r');
    
    % En faisant les mêmes analyses avec un set sans sommeil,
    % on voit qu'après 40 essais pas ex, il y a une distribution bimodale
    % des poids, mais après 120 essais, il reste pas bpc de poids importants
    % pkoi ? peut-être parce qu'on passe tjs par le même chemin, du coup on 
    % oublie les restes à cause de la LTP compétitice ?
    
    % on peut afficher par jour
%     for i=1:12:172
%         test=W{i}(:);
%         figure, hist(test(test~=0),30);
%     end
    clear exp_data;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Plots et save généraux
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% filename = sprintf ('%s/behavior.mat', data_path);
% save('-mat', filename, 'exp_all1', 'exp_all2', 'exp_all3', ...'exp_fst1', 'exp_fst2', 'exp_fst3');
figure, subplot(211), boxplot(exp_all1), subplot(212), boxplot(exp_fst1);
figure, subplot(211), boxplot(exp_all2), subplot(212), boxplot(exp_fst2);
figure, subplot(211), boxplot(exp_all3), subplot(212), boxplot(exp_fst3);

