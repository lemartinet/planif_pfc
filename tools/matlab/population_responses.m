function population_responses(exp_data, listn, pop_name)
% here we plot :
%   graph des activités (en couleur) pour une position donnée, avec chaque point = le centre d'un champ recepteur
%   graph des activités cumulées en tous les points, en prenant la somme, le max, ...

places = exp_data.pv_space;
fig_path = sprintf('%s/population_responses_%s/', exp_data.path, pop_name);
mkdir(fig_path);
nbplaces = length(find(places > 0));
centers = cell2mat({exp_data.neuron(listn).center}');
frs = [exp_data.neuron(listn).fr];
max_fr = max(frs(:));
for i = 1 : nbplaces
%     % ma propre échelle rgb
%     rgb = zeros(length(listn), 3);
%     c1 = frs(i, :) < 0.4; 
%     rgb(c1, 1) = 0; rgb(c1, 2) = 2.5 * frs(i, c1); rgb(c1, 3) = 1;
%     c2 = 0.4 <= frs(i, :) & frs(i, :) < 0.6;
%     rgb(c2, 1) = 5 * (frs(i, c2) - 0.4); rgb(c2, 2) = 1; rgb(c2, 3) =  1 - 5 * (frs(i, c2) - 0.4);
%     c3 = frs(i, :) >= 0.6;
%     rgb(c3, 1) = 1; rgb(c3, 2) = 1 - 2.5 * (frs(i, c3) - 0.6); rgb(c3, 3) = 0;
%     scatter(centers(:,1), centers(:,2), 50, rgb, 'filled');
    f = figure('Visible', 'off');
    scatter(centers(:,1), centers(:,2), 50, frs(i, :), 'filled');
    hold on; scatter(0, 0, 1, max_fr); % pour forcer l'échelle commune à 1
    colorbar;
    saveas(f, sprintf('%s/p%d.pdf', fig_path, i), 'pdf');
    close(f);
end
r = zeros(size(places));
r(places > 0) = sum(frs'); 
r(places > 0) = r(places > 0) - min(r(places > 0)); 
f = figure('Visible', 'off');
imagesc(r);
colorbar;
%surface(r);
%surf(r, 'FaceColor', 'interp', 'EdgeColor', 'none', 'Grid', 'none'); view([0 90]);
saveas(f, sprintf ('%s/all_responses.pdf', fig_path),'pdf');

frs(frs >= 0.3) = 1;
frs(frs < 0.3) = 0;
r = zeros(size(places));
r(places > 0) = sum(frs');
f = figure('Visible', 'off');
imagesc(r);
colorbar;
saveas(f, sprintf ('%s/all_responses_bin.pdf', fig_path),'pdf');
close(f);

end
	
