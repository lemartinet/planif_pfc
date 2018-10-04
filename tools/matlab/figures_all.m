function [] = figures_all (data_path, nbexp)

% Loading values...
filename = sprintf ('%s/data_extracted/fsize1_o.mat', data_path);
load (filename, 'fsize1_o');
filename = sprintf ('%s/data_extracted/fsize1_c.mat', data_path);
load (filename, 'fsize1_c');

filename = sprintf ('%s/data_extracted/pks_o.mat', data_path);
load (filename, 'pks_o');
filename = sprintf ('%s/data_extracted/pks_c.mat', data_path);
load (filename, 'pks_c');

filename = sprintf ('%s/data_extracted/aks_o.mat', data_path);
load (filename, 'aks_o');
filename = sprintf ('%s/data_extracted/paks_o.mat', data_path);
load (filename, 'paks_o');
filename = sprintf ('%s/data_extracted/aks_c.mat', data_path);
load (filename, 'aks_c');
filename = sprintf ('%s/data_extracted/paks_c.mat', data_path);
load (filename, 'paks_c');

filename = sprintf ('%s/data_extracted/lks_o.mat', data_path);
load (filename, 'lks_o');
filename = sprintf ('%s/data_extracted/lks_c.mat', data_path);
load (filename, 'lks_c');

filename = sprintf ('%s/data_extracted/infos_c.mat', data_path);
load (filename, 'infos_c');
filename = sprintf ('%s/data_extracted/infos_o.mat', data_path);
load (filename, 'infos_o');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the paks distribution for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fig = 1;
f = figure (fig); 
clf (fig);

y_c = [];
y_o = [];
error_c = [];
error_o = [];

nb = size (paks_c,2);
y_c (1) = 0;
error_c (1) = 0;

for i=2:nb+1
	y_c (i) = mean (paks_c (:,i-1));
	%error_c(i)=std (paks_o(:,i-1));
end
x_c = 0:100;

nb = size (paks_o,2);
y_o (1) = 0;
error_o (1) = 0;
for i=2:nb+1
	y_o (i) = mean (paks_o (:,i-1));
	%error_o(i)=std (paks_o(:,i-1));
end
x_o=0:100;

%errorbar (x_c, y_c, error_c);
plot (x_c, y_c);
hold on;
%errorbar (x_o, y_o, error_o, '--');
plot (x_o, y_o, '--');

title ('Active kurtosis');
xlabel('Number of active cell');
ylabel('Probability');
axis([0 30 0 1]);
figName = sprintf('%s/data_extracted/figures/active_kurtosis_fig1.eps', data_path);
saveas (f, figName, 'psc2');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the boxplot of aks for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fig = fig + 1;
f = figure (fig); 
clf (fig);

data = build_r_data (nbexp, aks_c', aks_o', false, false, 0, 0);

boxplot (data(:,2), data(:,1));

title ('boxplot aks');
xlabel('');
ylabel('');
%axis([0.5 2.5 0 90]);
figName = sprintf('%s/data_extracted/figures/active_kurtosis_boxplot.eps', data_path);
saveas (f, figName, 'psc2');
figName = sprintf('%s/data_extracted/group_active_kurtosis', data_path);
save ('-ASCII', figName, 'data');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the density vs aks for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fig = fig + 1;
f = figure (fig); 
clf (fig);

for i=1:nbexp
	density_c(i) = sum ([0 paks_c(i,:)] .* x_c);
end
for i=1:nbexp
	density_o(i) = sum ([0 paks_o(i,:)] .* x_o);
end

data = build_r_data (nbexp, density_c', density_o', false, false, 0, 0);

boxplot (data(:,2), data(:,1));

title ('boxplot density');
xlabel('');
ylabel('');
figName = sprintf('%s/data_extracted/figures/density_boxplot.eps', data_path);
saveas (f, figName, 'psc2');
figName = sprintf('%s/data_extracted/group_density', data_path);
save ('-ASCII', figName, 'data');

%plot (density_c, aks_c, '.');
%[corr_c, p_c] = corrcoef (density_c, aks_c);
%printf('correlation density vs aks for cells : %f %f\n', corr_c, p_c);

%fig = fig + 1;
%f = figure (fig); 
%clf (fig);
%plot (density_o, aks_o, '.');
%[corr_o, p_o] = corrcoef (density_o, aks_o);
%printf('correlation density vs aks for cells : %f %f\n', corr_c, p_c);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the boxplot of pks for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fig = fig + 1;
f = figure (fig); 
clf (fig);

data = build_r_data (nbexp, pks_c, pks_o, true, false, 0, 0);

boxplot (data(:,2), data(:,1));

title ('boxplot pks');
xlabel('');
ylabel('');
%axis([0.5 2.5 0 90]);
figName = sprintf('%s/data_extracted/figures/population_kurtosis_boxplot.eps', data_path);
saveas (f, figName, 'psc2');
figName = sprintf('%s/data_extracted/group_population_kurtosis', data_path);
save ('-ASCII', figName, 'data');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the boxplot of lks for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fig = fig + 1;
f = figure (fig); 
clf (fig);

data = build_r_data (nbexp, lks_c, lks_o, true, true, 0.9, 0.6);

boxplot (data(:,2), data(:,1));

title ('boxplot lks');
xlabel('');
ylabel('');
%axis([0.5 2.5 0 90]);
figName = sprintf('%s/data_extracted/figures/lifetime_kurtosis_boxplot.eps', data_path);
saveas (f, figName, 'psc2');
figName = sprintf('%s/data_extracted/group_lifetime_kurtosis', data_path);
save ('-ASCII', figName, 'data');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the receptive field size vs lks for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fig = fig + 1;
f = figure (fig); 
clf (fig);

[data_fsize, data_fsize_c, data_fsize_o] = build_r_data (nbexp, fsize1_c, fsize1_o, true, true, 0.9, 0.6);
[data_lks, data_lks_c, data_lks_o]  = build_r_data (nbexp, lks_c, lks_o, true, true, 0.9, 0.6);

plot (data_lks_c, data_fsize_c, 'o');
[corr_c, p_c] = corrcoef (data_lks_c, data_fsize_c);
printf('correlation lks vs field size for cells : %f %f\n', corr_c, p_c);

%fig = fig + 1;
%f = figure (fig); 
%clf (fig);
hold on
plot (data_lks_o, data_fsize_o, 'x');
[corr_o, p_o] = corrcoef (data_lks_o, data_fsize_o);
printf('correlation lks vs field size for columns : %f %f\n', corr_o, p_o);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the boxplot or histogram of MI for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fig = fig + 1;
f = figure (fig); 
clf (fig);

data = build_r_data (nbexp, infos_c, infos_o, true, true, 0.6, 0.6);

boxplot (data(:,2), data(:,1));

title ('boxplot info');
xlabel('');
ylabel('');
%axis([0.5 2.5 0 90]);
figName = sprintf('%s/data_extracted/figures/info_boxplot.eps', data_path);
saveas (f, figName, 'psc2');
figName = sprintf('%s/data_extracted/group_info', data_path);
save ('-ASCII', figName, 'data');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We compute ANOVA test between for information with subgroups
% of experiments
% can be used for lifetime kurtosis too
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
m = 0;
for i=1:2:39
indice = i;
data2 = build_r_data (2, infos_c(indice:100), infos_o(indice:100), true, false, 0.5, 0.5);
data10 = build_r_data (10, infos_c(indice:100), infos_o(indice:100), true, false, 0.5, 0.5);
val = data10(:,2);                                                              
lkc = val (find(data2(:,1)==1));                                                
lko = val (find(data10(:,1)==2));
data = build_r_data (1, lkc', lko', false, false, 0.5, 0.5);                    
m1 = anova1 (data(:,2), data(:,1));
m = m + m1;
end
m = m / 20;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% info prospective
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%bar(mean(info))
%hold on
%errorbar(1,mean(info), std(info))
%axis([0 2 0 4]);


