function [prez, p_tr, MI, MI_pop, max_info] = prospective_intersection_only(exp_data, unit_list)
% function [prez, p_tr, MI, MI_pop] = prospective_intersection_only(exp_data, unit_list)
%
% unit_list = list of unit number encoding the predicted position
%
% Return:
% MI_pop structure (see mutual_information_pop)

% we extract the position indexes
steps = [exp_data.day(1).start : exp_data.step_size : exp_data.day(1).end, ...
    exp_data.day(14).start : exp_data.step_size : exp_data.day(14).end, ...
    exp_data.day(15).start : exp_data.step_size : exp_data.day(15).end] ./ exp_data.step_size;
nb_steps = length(steps);
zones = zeros(1, nb_steps);
v_space = exp_data.v_space(steps, :);
fprintf('Extraction of decision zones\n');
for i = 1 : length(v_space)
    v = v_space(i, :);
    if v(1) < 43.5 
        zones(i) = 2;
    elseif v(1) > 53.5
        zones(i) = 3;
    elseif v(2) < 93.5
        zones(i) = 1;
    elseif v(2) > 103.5
        zones(i) = 4;
    else
        zones(i) = 0;
    end
end
frs = zeros(length(unit_list), nb_steps);
fprintf('Extraction of temporal firing rates of predictive units\n');
for i = 1 : length(unit_list)
    fr = exp_data.load_neuron(unit_list(i));
    fr = fr(steps);
	frs(i, :) = fr';
    clear fr;
end

fprintf('Extraction the transition informations\n');
prez = cell(length(unit_list), 4);
p_tr = zeros(4, 1);
last = -1;
tmp_pr = [];
cpt_forced_run = 0;
for i = 1 : length(zones)
	id = zones(i);
	if last ~= id
		% nouvelle transition
        if last == 0 && (id ~= 1 || (id == 1 && mod(cpt_forced_run, 2) == 0))
            for j = 1 : length(unit_list)
                prez{j, id} = [prez{j, id}, tmp_pr(:, j)'];
            end
            p_tr(id) = p_tr(id) + 1;
        end
        if last == 0 && id == 1
            cpt_forced_run = cpt_forced_run + 1;
        end
		tmp_pr = [];
		last = id;
	end
	tmp_pr = [tmp_pr; frs(:, i)'];
end
p_tr = p_tr ./ sum(p_tr);

for i = 1 : length(unit_list)
	MI(i) = mutual_information(prez(i, :), p_tr);
end
MI_pop = mutual_information_pop(prez, p_tr);

max_info = [max([MI.info] - [MI.correction]), MI_pop.info - MI_pop.correction]
real_info = -nansum(p_tr .* log2(p_tr))

end

% max_info = zeros(4, 2);
% exp_data = exp_data1;
% pop1 = exp_data.spatial_0.active(exp_data.spatial_0.active <= 540);
% unit_list = exp_data.from_type(0);
% unit_list = unit_list(pop1);
% [prez, p_tr, MI, MI_pop, max_info(1, :)] = prospective_intersection_only(exp_data, unit_list);
% unit_list = exp_data.from_type(2);
% unit_list = unit_list(pop1);
% [prez, p_tr, MI, MI_pop, max_info(2, :)] = prospective_intersection_only(exp_data, unit_list);
% unit_list = exp_data.from_type(4);
% unit_list = unit_list(pop1);
% [prez, p_tr, MI, MI_pop, max_info(3,:)] = prospective_intersection_only(exp_data, unit_list);
% unit_list = exp_data.from_type(10);
% [prez, p_tr, MI, MI_pop, max_info(4,:)] = prospective_intersection_only(exp_data, unit_list);
% max_info(5,:) = [1.5860, 1.5860];
% max_info1 = max_info;
% 
% max_info = zeros(4, 2);
% exp_data = exp_data2;
% pop1 = exp_data.spatial_0.active(exp_data.spatial_0.active <= 540);
% unit_list = exp_data.from_type(0);
% unit_list = unit_list(pop1);
% [prez, p_tr, MI, MI_pop, max_info(1, :)] = prospective_intersection_only(exp_data, unit_list);
% unit_list = exp_data.from_type(2);
% unit_list = unit_list(pop1);
% [prez, p_tr, MI, MI_pop, max_info(2, :)] = prospective_intersection_only(exp_data, unit_list);
% unit_list = exp_data.from_type(4);
% unit_list = unit_list(pop1);
% [prez, p_tr, MI, MI_pop, max_info(3,:)] = prospective_intersection_only(exp_data, unit_list);
% unit_list = exp_data.from_type(10);
% [prez, p_tr, MI, MI_pop, max_info(4,:)] = prospective_intersection_only(exp_data, unit_list);
% max_info(5,:) = [1.5860, 1.5860];
% max_info2 = max_info;
% 
% max_info = 0.5 * (max_info1 + max_info2)
% max_info_std = sqrt(0.5 .* ((max_info1 - max_info) .^ 2 + (max_info2 - max_info) .^ 2));
% save ../../figs/prospective/info_prospectintersection.mat max_info1 max_info2 max_info max_info_std
% 
% bar(max_info(:,2))
% hold on
% errorbar(max_info(:,2), max_info_std(:,2), '+');
% figure, bar(max_info(:,1))
% hold on
% errorbar(max_info(:,1), max_info_std(:,1), '+');
