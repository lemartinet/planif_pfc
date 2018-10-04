function population_responses (folder_out, nbvars)
% here we plot :
%   graph des activités (en couleur) pour une position donnée, avec chaque point = le centre d'un champ recepteur
%   graph des activités cumulées en tous les points, en prenant la somme, le max, ...

filename = sprintf ('%s/frs.mat', folder_out);
load('-mat', filename, 'frs', 'centers');
filename = sprintf ("%s/proba%s.mat", folder_out, mat2str(nbvars));
load('-mat', filename, 'pv');
places = pv;

filename = sprintf ('%s/frs/', folder_out);
mkdir (filename);

nbcells = size(frs, 1);
nbplaces = size(frs, 2);
idx=find(places > 0);
r = zeros(size(places));
for i=1:nbplaces
	% sauvegarde pour chaque position l'activite de la population
	% en faisant un point colore aux centres des cellules
%	if all == 1
		f = figure;
		hold on;
		for j=1:nbcells
			level = frs(j,i);
			rgb = [];
			if level < 0.4
				rgb = [0 2.5*level 1];
			elseif 0.4 <= level & level < 0.6
				rgb = [5*(level-0.4) 1 1-5*(level-0.4)];
			elseif level >= 0.6
				rgb = [1 1-2.5*(level-0.6) 0];
			end
			plot(centers(j,1),centers(j,2), 'o', 'Color' , rgb, 'MarkerSize', 6, 'LineWidth', 5);
		end
		filename = sprintf ('%s/frs/p%d.eps', folder_out, i);
		saveas (f, filename,'psc2');
		close (f);
%	end
	r(idx(i)) = sum (frs(:,i));
end
f = figure;
filename = sprintf ('%s/frs/all_responses.eps', folder_out);
surface(r);
saveas (f, filename,'psc2');
close(f);
	
