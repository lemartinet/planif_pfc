function fig_space_correlate(data_path, exp, reload)
% function fig_space_correlate(data_path, exp, reload)

% TODO: 
%       séparer pour les neurones des cols les pop1 et pop2
%       virer les neurones silencieux qd ca gêne (mesure sur les neurones
%           et non sur les positions)
%       vérifier la taille des champs pour les pop2

% type to analyse
an_type = [10, 0, 1, 2];

for i = exp	
	fprintf('Working on experiment %d... ', i);
    exp_data = ExpData(data_path, i, 1, 1);
    
    if ~reload
        for t = an_type
            fprintf('Kurtosis %d...', t);
            frs = [exp_data.neuron(exp_data.from_type(t)).fr]';
            % on peut enlever les unites considerees comme silencieuse
            idn = find(max(frs, [], 2) > 0.101);
            exp_data.(sprintf('spatial_%d', t)).active = idn;
%             frs = frs(exp_data.(sprintf('type_%d', t)).idn, :);
            exp_data.(sprintf('spatial_%d', t)).pks = population_kurtosis(frs);
            exp_data.(sprintf('spatial_%d', t)).density = spatial_density(frs);
            exp_data.(sprintf('spatial_%d', t)).lks = lifetime_kurtosis(frs, exp_data.pv_space);
            exp_data.(sprintf('spatial_%d', t)).lks_equi = kurtosis(frs') - 3;

            fprintf('Information %d...', t);
            [MI, MI_pop, MI_equi, MI_pop_equi] = info_all(exp_data, t);
            exp_data.(sprintf('spatial_%d', t)).MI = MI;
            exp_data.(sprintf('spatial_%d', t)).MI_pop = MI_pop;
            exp_data.(sprintf('spatial_%d', t)).MI_equi = MI_equi;
            exp_data.(sprintf('spatial_%d', t)).MI_pop_equi = MI_pop_equi;
        end
        save(sprintf('%s/exp_data.mat', exp_data.path), 'exp_data');
    end
    t0(i) = exp_data.spatial_0;
    fsize0(i, :) = cellfun(@max, {exp_data.neuron(exp_data.from_type(0)).fsize});
    t10(i) = exp_data.spatial_10;
    fsize10(i, :) = cellfun(@max, {exp_data.neuron(exp_data.from_type(10)).fsize});
    clear exp_data frs idn MI MI_pop MI_equi MI_pop_equi;
end

return;

% specialiser par pop1 et pop2
exp_data.spatial_0.pop1 = exp_data.spatial_0.active(exp_data.spatial_0.active <= 540);
exp_data.spatial_0.pop2 = exp_data.spatial_0.active(exp_data.spatial_0.active > 540);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Boxplot of densities for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[data10, gr10] = build_r_data(ti10(1).density, 0);
[data0, gr0] = build_r_data(ti0(1).density, 0);
% figure, hist(data10); % pour afficher la distrib des densités
% figure, hist(data0);
% f = figure('Visible', 'off'); 
figure(); 
boxplot([data10, data0], [gr10, 2 * gr0]);
anova1([data10, data0], [gr10, 2 * gr0]);
% saveas (f, sprintf('%s/spatial_density.pdf', data_path), 'pdf');

fr0 = [exp_data.neuron(exp_data.from_type(0)).fr];
fr10 = [exp_data.neuron(exp_data.from_type(10)).fr];
d10 = spatial_density(fr10(:, exp_data.spatial_10.active)');
d0_2 = spatial_density(fr0(:, pop2)');
d0_1 = spatial_density(fr0(:, pop1)');
boxplot([d10, d0_1, d0_2], [ones(size(d10)), 2 * ones(size(d0_1)), 3*ones(size(d0_2))]);
anova1([d0_1, d0_2], [2 * ones(size(d0_1)), 3 *ones(size(d0_2))]);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Boxplot of population kurtosis for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[data10, gr10] = build_r_data(ti10(1).pks, 0);
[data0, gr0] = build_r_data(ti0(1).pks, 0);
% f = figure('Visible', 'off'); 
figure(); 
boxplot([data10, data0], [gr10, 2 * gr0]);
anova1([data10, data0], [gr10, 2 * gr0]);
%saveas(f, sprintf('%s/population_kurtosis.pdf', data_path), 'pdf');

fr10 = [exp_data.neuron(exp_data.from_type(10)).fr];
d10 = population_kurtosis(fr10(:, exp_data.spatial_10.active)');
fr0 = [exp_data.neuron(exp_data.from_type(0)).fr];
for i = pop1'
   fr0(:, i) = fr0(:, pop2(1) - 1);
end
d0_2 = population_kurtosis(fr0');
fr0 = [exp_data.neuron(exp_data.from_type(0)).fr];
for i = pop2'
   fr0(:, i) = fr0(:, pop2(1) - 1);
end
d0_1 = population_kurtosis(fr0');
boxplot([d10, d0_1, d0_2], [ones(size(d10)), 2 * ones(size(d0_1)), 3*ones(size(d0_2))]);
anova1([d0_1, d0_2], [2 * ones(size(d0_1)), 3 *ones(size(d0_2))]);
 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Boxplot of lifetime kurtosis for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[data10, gr10] = build_r_data(ti10(1).lks(ti10(1).active), 0);
[data0, gr0] = build_r_data(ti0(1).lks(ti0(1).active), 0);
% f = figure('Visible', 'off'); 
figure(); 
boxplot([data10, data0], [gr10, 2 * gr0]);
anova1([data10, data0], [gr10, 2 * gr0]);
% saveas(f, sprintf('%s/lifetime_kurtosis.pdf', data_path), 'pdf');

[data10, gr10] = build_r_data(ti10(1).lks_equi(ti10(1).active), 0);
[data0, gr0] = build_r_data(ti0(1).lks_equi(ti0(1).active), 0);
% f = figure('Visible', 'off'); 
figure(); 
boxplot([data10, data0], [gr10, 2 * gr0]);
anova1([data10, data0], [gr10, 2 * gr0]);
% saveas(f, sprintf('%s/lifetime_kurtosis.pdf', data_path), 'pdf');

[data0_1, gr0_1] = build_r_data(ti0(1).lks_equi(pop1), 0);
[data0_2, gr0_2] = build_r_data(ti0(1).lks_equi(pop2), 0);
% f = figure('Visible', 'off'); 
figure(); 
boxplot([data0_1, data0_2], [gr0_1, 2 * gr0_2]);
anova1([data0_1, data0_2], [gr0_1, 2 * gr0_2]);
figure, boxplot([data10, data0_1, data0_2], [gr10, 2 * gr0_1, 3 * gr0_2]);
 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the receptive field size vs lks for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
f10 = fsize10(1, :);
f0 = fsize0(1, :);
[data10, gr10] = build_r_data(f10(ti10(1).active), 0);
[data0_1, gr0_1] = build_r_data(f0(pop1), 0);
[data0_2, gr0_2] = build_r_data(f0(pop2), 0);
% f = figure('Visible', 'off'); 
figure(); 
boxplot([data10, data0_1, data0_2], [gr10, 2 * gr0_1, 2 * gr0_2]);
anova1([data10, data0_1, data0_2], [gr10, 2 * gr0_1, 2 * gr0_2]);
% saveas(f, sprintf('%s/field_size.pdf', data_path), 'pdf');

figure, boxplot([data10, data0_1, data0_2], [gr10, 2 * gr0_1, 3 * gr0_2]);
anova1([data10, data0_1], [gr10, 2 * gr0_1]);
anova1([data0_1, data0_2], [2 * gr0_1, 3 * gr0_2]);

[datalk10] = build_r_data(ti10(1).lks_equi(ti10(1).active), 0);
[datalk0] = build_r_data(ti0(1).lks_equi(ti0(1).active), 0);
[data0] = build_r_data(f0([pop1, pop2]), 0);
f = figure(); plot(datalk10, data10, '+');
hold on, plot(datalk0, data0, '+');
[corr_10, p_10] = corrcoef(datalk10, data10);
[corr_0, p_0] = corrcoef(datalk0, data0);
[corr_all, p_all] = corrcoef([datalk0, datalk10], [data0,data10])
title(sprintf('lks vs fsize\ncells: %f %f\ncolumns: %f %f\nall: %f %f', ...
    corr_10(1,2), p_10(1,2), corr_0(1,2), p_0(1,2), corr_all(1,2), p_all(1,2)));
% saveas(f, sprintf('%s/lk_vs_field.pdf', data_path), 'pdf');
 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We plot the boxplot or histogram of MI for cells and columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MI10 = [ti10(1).MI.info] - [ti10(1).MI.correction];
% MI0 = [ti0(1).MI.info] - [ti0(1).MI.correction];
MI10 = [ti10(1).MI_equi.info] - [ti10(1).MI_equi.correction];
MI0 = [ti0(1).MI_equi.info] - [ti0(1).MI_equi.correction];
[data10, gr10] = build_r_data(MI10(ti10(1).active), 0);
% [data0, gr0] = build_r_data(MI0(ti0(1).active), 0);
[data0_1, gr0_1] = build_r_data(MI0(pop1), 0);
[data0_2, gr0_2] = build_r_data(MI0(pop2), 0);
% f = figure('Visible', 'off'); 
figure(); 
% boxplot([data10, data0], [gr10, 2 * gr0]);
boxplot([data10, data0_1, data0_2], [gr10, 2 * gr0_1, 3 * gr0_2]);
anova1([data10, data0_1], [gr10, 2 * gr0_1]);
anova1([data0_1, data0_2], [gr0_1, 2 * gr0_2]);
% saveas(f, sprintf('%s/mutual_info_equi.pdf', data_path), 'pdf');


MI10 = [ti10(1).MI_equi.sp_corr];
MI0 = [ti0(1).MI_equi.sp_corr];
[data10, gr10] = build_r_data(MI10(ti10(1).active), 0);
% [data0, gr0] = build_r_data(MI0(ti0(1).active), 0);
[data0_1, gr0_1] = build_r_data(MI0(pop1), 0);
[data0_2, gr0_2] = build_r_data(MI0(pop2), 0);
% f = figure('Visible', 'off'); 
figure(); 
% boxplot([data10, data0], [gr10, 2 * gr0]);
boxplot([data10, data0_1, data0_2], [gr10, 2 * gr0_1, 3 * gr0_2]);
anova1([data10, data0_1], [gr10, 2 * gr0_1]);
anova1([data0_1, data0_2], [gr0_1, 2 * gr0_2]);
% saveas(f, sprintf('%s/mutual_info_equi.pdf', data_path), 'pdf');

% for i = 1 : 10
%     MI10 = [ti10(1).MI_equi.info] - [ti10(1).MI_equi.correction];
%     MI0 = [ti0(1).MI_equi.info] - [ti0(1).MI_equi.correction];
% end

unit_type = 0;
idx = find(exp_data.pv_space > 0);
unit_list = exp_data.from_type(unit_type);
unit_list = unit_list(pop1);
frs_vect = cell(length(unit_list), length(idx));
for i = 1 : length(unit_list)
	fr = exp_data.load_neuron(unit_list(i));
	tfr = firing_rate(fr, exp_data.v_space, size(exp_data.pv_space), true); clear fr;
    frs_vect(i, :) = tfr(idx); clear tfr;
end
probav = ones(1, length(idx))' .* (1 / length(idx));
MI_pop_equi_1 = mutual_information_pop(frs_vect, probav);
unit_list = exp_data.from_type(unit_type);
unit_list = unit_list(pop2);
frs_vect = cell(length(unit_list), length(idx));
for i = 1 : length(unit_list)
	fr = exp_data.load_neuron(unit_list(i));
	tfr = firing_rate(fr, exp_data.v_space, size(exp_data.pv_space), true); clear fr;
    frs_vect(i, :) = tfr(idx); clear tfr;
end
MI_pop_equi_2 = mutual_information_pop(frs_vect, probav)
MI_pop1 = [exp_data1.spatial_0.MI_pop_equi_pop1.info, exp_data2.spatial_0.MI_pop_equi_pop1.info] ...
- [exp_data1.spatial_0.MI_pop_equi_pop1.correction, exp_data2.spatial_0.MI_pop_equi_pop1.correction];
MI_pop2 = [exp_data1.spatial_0.MI_pop_equi_pop2.info, exp_data2.spatial_0.MI_pop_equi_pop2.info] ...
- [exp_data1.spatial_0.MI_pop_equi_pop2.correction, exp_data2.spatial_0.MI_pop_equi_pop2.correction];
MI_pc = [exp_data1.spatial_10.MI_pop_equi.info, exp_data2.spatial_10.MI_pop_equi.info] ...
- [exp_data1.spatial_10.MI_pop_equi.correction, exp_data2.spatial_10.MI_pop_equi.correction];
bar([MI_max, mean(MI_pc), mean(MI_pop1), mean(MI_pop2)])
hold on
errorbar([2 3 4], [mean(MI_pc), mean(MI_pop1), mean(MI_pop2)],[std(MI_pc), std(MI_pop1), std(MI_pop2)], '+');

MI10_1 = [exp_data1.spatial_10.MI_equi.info] - [exp_data1.spatial_10.MI_equi.correction];
MI10_2 = [exp_data2.spatial_10.MI_equi.info] - [exp_data2.spatial_10.MI_equi.correction];
MI0_1 = [exp_data1.spatial_0.MI_equi.info] - [exp_data1.spatial_0.MI_equi.correction];
MI0_2 = [exp_data2.spatial_0.MI_equi.info] - [exp_data2.spatial_0.MI_equi.correction];
red_pc = MI_pc ./ [sum(MI10(exp_data1.spatial_10.active)), sum(MI10(exp_data2.spatial_10.active))];
red_pop1 = MI_pop1 ./ [sum(MI0_1(exp_data1.spatial_0.pop1)), sum(MI0_2(exp_data2.spatial_0.pop1))];
red_pop2 = MI_pop2 ./ [sum(MI0_1(exp_data1.spatial_0.pop2)), sum(MI0_2(exp_data2.spatial_0.pop2))];
bar([mean(red_pc), mean(red_pop1), mean(red_pop2)])
hold on
errorbar([mean(red_pc), mean(red_pop1), mean(red_pop2)], [std(red_pc), std(red_pop1), std(red_pop2)], '+');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We compute ANOVA test between for information with subgroups
% of experiments (can be used for lifetime kurtosis too)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% m = 0;
% for i = 1:2:40
%     datac = build_r_data(infos_c(i, :), 0.6);
%     datao = build_r_data(infos_o(i, :), 0.6);
%     max_data = max(length(datao), length(datac));
%     m1 = anova1([datac(1:max_data)', datao(1:max_data)']);
%     m = m + m1;
% end
% m = m / 20;

% POP1 MI_pop_equi = 
%         ce_all: [1x128 double]
%         sample: [1x128 double]
%             ce: 2.7434
%              e: 8.8498
%       surprise: [1x128 double]
%           info: 6.1064
%     correction: 0.0203
% 
% POP2 MI_pop_equi = 
%         ce_all: [1x128 double]
%         sample: [1x128 double]
%             ce: 1.9978
%              e: 6.8249
%       surprise: [1x128 double]
%           info: 4.8271
%     correction: 0.0177   

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Regularity of the sampling by pop1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
c=reshape([exp_data1.neuron(exp_data1.from_type(0)).center], 2, 600)';
c = c(exp_data1.spatial_0.pop1, :);
plot(c(:,1), c(:,2), 'o');
c_1 = c(1:23, :);
c_2 = c(24:43, :);
c_3 = c(44:end, :);
d_1 = sqrt(sum((c_1(1:end-1, :) - c_1(2:end, :)) .^ 2, 2));
d_2 = sqrt(sum((c_2(1:end-1, :) - c_2(2:end, :)) .^ 2, 2));
d_3 = sqrt(sum((c_3(1:end-1, :) - c_3(2:end, :)) .^ 2, 2));
d_all=[d_1;d_2;d_3]';
mean(d_all), std(d_all)
figure, boxplot(d_all);
    
end