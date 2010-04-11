function [MI, MI_pop, MI_equi, MI_pop_equi] = info_all(exp_data, unit_type)

fprintf('Extracting complete neuron firing rate...\n');
idx = find(exp_data.pv_space > 0);
unit_list = exp_data.from_type(unit_type);
frs_vect = cell(length(unit_list), length(idx));
for i = 1 : length(unit_list)
	fr = exp_data.load_neuron(unit_list(i));
	tfr = firing_rate(fr, exp_data.v_space, size(exp_data.pv_space), true); clear fr;
    frs_vect(i, :) = tfr(idx); clear tfr;
end
frs = [exp_data.neuron(unit_list).fr]';
% on enleve les unites considerees comme silencieuse
%idn = find(max(frs, [], 2) > 0.101);
%frs_vect = frs_vect(idn, :);
%frs = frs(idn, :);

probav = exp_data.pv_space(idx);
fprintf('Computation information of single cells\n');
nbcells = size(frs_vect, 1);
MI = struct('ce_all', [], 'sample', [], 'ce', 0, 'e', 0, 'surprise', [], 'info', 0, 'correction', 0);
for i = 1 : nbcells
	MI(i) = mutual_information(frs_vect(i, :), probav);
end
for i = 1 : nbcells
    [m1, s1] = spatial_mean_std(exp_data, MI(i).surprise);
    [m2, s2] = spatial_mean_std(exp_data, frs(i, :));
	MI(i).sp_corr = exp_data.spatial_mean_std((MI(i).surprise - m1) .* (frs(i, :) - m2)) / (s1 * s2);
end

fprintf('Computation of information for the population\n');
MI_pop = mutual_information_pop(frs_vect, probav);
MI_pop.ratio_redundant = MI_pop.info / sum([MI.info]);
MI_pop.ratio_redundant_correct = (MI_pop.info - MI_pop.correction) / sum([MI.info] - [MI.correction]);

probav = ones(1, length(idx))' .* (1 / length(idx));
fprintf('Computation information of single cells\n');
nbcells = size(frs_vect, 1);
MI_equi = struct('ce_all', [], 'sample', [], 'ce', 0, 'e', 0, 'surprise', [], 'info', 0, 'correction', 0);
for i = 1 : nbcells
	MI_equi(i) = mutual_information(frs_vect(i, :), probav);
end
for i = 1 : nbcells
    m1 = mean(MI_equi(i).surprise); s1 = std(MI_equi(i).surprise);
    m2 = mean(frs(i, :)); s2 = std(frs(i, :));
	MI_equi(i).sp_corr = mean((MI_equi(i).surprise - m1) .* (frs(i, :) - m2)) / (s1 * s2);
end

fprintf('Computation of information for the population\n');
MI_pop_equi = mutual_information_pop(frs_vect, probav);
MI_pop_equi.ratio_redundant = MI_pop_equi.info / sum([MI_equi.info]);
MI_pop_equi.ratio_redundant_correct = (MI_pop_equi.info - MI_pop_equi.correction) / sum([MI_equi.info] - [MI_equi.correction]);

end
