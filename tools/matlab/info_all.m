function [MI, MI_pop] = info_all(exp_data, unit_type)

fprintf('Extracting complete neuron firing rate...\n');
idx = find(exp_data.pv_space > 0);
frs_vect = cell(length(exp_data.from_type(unit_type)), length(idx));
for i = exp_data.from_type(unit_type)
	fr = exp_data.load_neuron(i);
	tfr = firing_rate(fr, exp_data.v_space, v_size, true); clear fr;
    frs_vect(i, :) = tfr(idx); clear tfr;
end
frs = [exp_data.neuron(exp_data.from_type(unit_type)).fr]';
% on enleve les unites considerees comme silencieuse
idn = find(max(frs(units, :), [], 2) > 0.15);
frs_vect = frs_vect(idn, :);
frs = frs(idn, :);

p = probav(find(probav > 0));
fprintf('Computation information of single cells\n');
nbcells = size(frs_vect, 1);
for i = 1 : nbcells
	[MI(i)] = mutual_information(frs_vect(i, :)', probav);
	MI(i).C = mean((MI(i).surprise - MI(i).info) .* (frs(i, :) - dotprod(frs(i, :), p))) / (sum(MI(i).surprise(i, :)) * sum(frs(i, :)));
	MI(i).correl = corrcoef(MI(i).surprise, frs(i, :));
end

fprintf('Computation of information for the population\n');
MI_pop = mutual_information_pop(frs_vect, probav);
MI_pop.ratio_redundant = MI_pop.info / sum([MI.info]);

end
