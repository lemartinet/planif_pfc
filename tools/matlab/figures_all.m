function figures_all(data_path)
% remettre boxplot et anova1 sous la forme (data, num_group) plutôt que
% (data_grp1, data_grp2) pour éviter de limiter à une taille de grp

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Boxplot of densities for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load(sprintf ('%s/aks_o.mat', data_path), 'aks_o');
load(sprintf ('%s/aks_c.mat', data_path), 'aks_c');

f = figure(); 
% we can mean the density for each experiment to have only nbexp values
datac = build_r_data(aks_c, 0);
datao = build_r_data(aks_o, 0);
max_data = max(length(datao), length(datac));
boxplot([datac(1:max_data)', datao(1:max_data)']);
title ('spatial density');
saveas (f, sprintf('%s/spatial_density.pdf', data_path), 'pdf');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Density distribution for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% load(sprintf('%s/paks_o.mat', data_path), 'paks_o');
% load(sprintf('%s/paks_c.mat', data_path), 'paks_c');
% 
% f = figure(); hold on;
% y_c = [0, mean(paks_c)];
% y_o = [0, mean(paks_o)];
% plot(0:100, y_c);
% plot(0:100, y_o, '--');
% title('density distribution');
% xlabel('Number of active cell');
% ylabel('Probability');
% axis([0 30 0 1]);
% saveas(f, sprintf('%s/density_distribution.pdf', data_path), 'pdf');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Boxplot of population kurtosis for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load(sprintf('%s/pks_o.mat', data_path), 'pks_o');
load(sprintf('%s/pks_c.mat', data_path), 'pks_c');

f = figure(); 
datac = build_r_data(pks_c, 0);
datao = build_r_data(pks_o, 0);
max_data = max(length(datao), length(datac));
boxplot([datac(1:max_data)', datao(1:max_data)']);
title('population kurtosis');
saveas(f, sprintf('%s/population_kurtosis.pdf', data_path), 'pdf');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Boxplot of lifetime kurtosis for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load(sprintf('%s/lks_o.mat', data_path), 'lks_o');
load(sprintf('%s/lks_c.mat', data_path), 'lks_c');

f = figure(); 
datac = build_r_data(lks_c, 0.9);
datao = build_r_data(lks_o, 0.6);
max_data = max(length(datao), length(datac));
boxplot([datac(1:max_data)', datao(1:max_data)']);
title('lifetime kurtosis');
saveas(f, sprintf('%s/lifetime_kurtosis.pdf', data_path), 'pdf');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the receptive field size vs lks for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load(sprintf('%s/fsize1_o.mat', data_path), 'fsize1_o');
load(sprintf('%s/fsize1_c.mat', data_path), 'fsize1_c');

f = figure();
datac_fsize = build_r_data(fsize1_c, 0.9);
datao_fsize = build_r_data(fsize1_o, 0.6);
max_data = max(length(datac_fsize), length(datao_fsize));
boxplot([datac_fsize(1:max_data)', datao_fsize(1:max_data)']);
title('field size');
saveas(f, sprintf('%s/field_size.pdf', data_path), 'pdf');

f = figure(); hold on;
plot(datac, datac_fsize, 'o');
plot(datao, datao_fsize, 'x');
[corr_c, p_c] = corrcoef(datac, datac_fsize);
[corr_o, p_o] = corrcoef(data_lks_o, data_fsize_o);
title(sprintf('correlation lks vs field size\nfor cells: %f %f\n...for columns: %f %f\n', corr_c, p_c, corr_o, p_o));
saveas(f, sprintf('%s/lk_vs_field.pdf', data_path), 'pdf');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the boxplot or histogram of MI for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load(sprintf('%s/infos_o.mat', data_path), 'infos_o');
load(sprintf('%s/infos_c.mat', data_path), 'infos_c');

f = figure(); 
datac = build_r_data(infos_c, 0.6);
datao = build_r_data(infos_o, 0.6);
max_data = max(length(datao), length(datac));
boxplot([datac(1:max_data)', datao(1:max_data)']);
title('mutual information');
saveas(f, sprintf('%s/mutual_info.pdf', data_path), 'pdf');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We compute ANOVA test between for information with subgroups
% of experiments
% can be used for lifetime kurtosis too
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
m = 0;
for i = 1:2:40
    datac = build_r_data(infos_c(i, :), 0.6);
    datao = build_r_data(infos_o(i, :), 0.6);
    max_data = max(length(datao), length(datac));
    m1 = anova1([datac(1:max_data)', datao(1:max_data)']);
    m = m + m1;
end
m = m / 20;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% info prospective
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
