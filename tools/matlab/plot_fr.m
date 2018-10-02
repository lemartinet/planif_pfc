function [] = plot_fr (data_path, exp, nbcol)

for i=1:nbcol
	filename = sprintf ('%s/data_extracted/%d/frs/o_%i', data_path, exp, i-1);
	load('-mat', filename, 'fr');
	
	f=figure(i);
	clf (f);
	contourf(fr);
	hold on
	contour(fr,10);
	colorbar;
	axis('off');
	figname = sprintf ('%s/data_extracted/%d/frs/o_%i.eps', data_path, exp, i-1);
	saveas (f, figname,'psc2');
	close (f);
end

% for i=1:24; fr=zeros(size(pv)); fr(id)=frs_3(i,:); figure; contourf(fr);end;

