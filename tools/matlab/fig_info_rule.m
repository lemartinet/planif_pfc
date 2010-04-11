function [MI, MI_pop, max_info] = fig_info_rule(exp_data, unit_list)
% function [MI, MI_pop] = fig_info_rule(exp_data, unit_list)
%
% pop1 = exp_data.spatial_0.active(exp_data.spatial_0.active <= 540);
% unit_list = exp_data.from_type(type);
% unit_list = unit_list(pop1);
% [MI, MI_pop] = fig_info_rule(exp_data, unit_list);
% c = exp_data.get_centers(exp_data.from_type(0));
% figure, plot3(c(pop1, 1), c(pop1, 2), [MI.info]', 'o');

Q_1 = exp_data.getQ(1, 12);
%Q_1 = Q_1(1:50, :);
Q_2 = exp_data.getQ(14, 12);
%Q_2 = Q_2(1:50, :);
Q_3 = exp_data.getQ(15, 7);
%Q_3 = Q_3(1:50, :);

frs_vect = cell(length(unit_list), 3);
for i = 1 : length(unit_list)
    frs_vect(i, 1) = {Q_1(:, unit_list(i))'};
    frs_vect(i, 2) = {Q_2(:, unit_list(i))'};
    frs_vect(i, 3) = {Q_3(:, unit_list(i))'};
end
probav = ones(1, 3)' .* (1 / 3);
for i = 1 : length(unit_list)
	MI(i) = mutual_information(frs_vect(i, :), probav);
end
MI_pop = mutual_information_pop(frs_vect, probav);
max_info = [max([MI.info] - [MI.correction]), MI_pop.info - MI_pop.correction]
real_info = -nansum(probav .* log2(probav))
end
 
% max_info = zeros(4, 2);
% exp_data = exp_data1;
% pop1 = exp_data.spatial_0.active(exp_data.spatial_0.active <= 540);
% unit_list = exp_data.from_type(0);
% unit_list = unit_list(pop1);
% [MI, MI_pop, max_info(1, :)] = fig_info_rule(exp_data, unit_list);
% unit_list = exp_data.from_type(2);
% unit_list = unit_list(pop1);
% [MI, MI_pop, max_info(2, :)] = fig_info_rule(exp_data, unit_list);
% unit_list = exp_data.from_type(4);
% unit_list = unit_list(pop1);
% [MI, MI_pop, max_info(3,:)] = fig_info_rule(exp_data, unit_list);
% unit_list = exp_data.from_type(10);
% [MI, MI_pop, max_info(4,:)] = fig_info_rule(exp_data, unit_list);
% max_info(5,:) = [1.5860, 1.5860];
% max_info1 = max_info;
% 
% max_info = zeros(4, 2);
% exp_data = exp_data2;
% pop1 = exp_data.spatial_0.active(exp_data.spatial_0.active <= 540);
% unit_list = exp_data.from_type(0);
% unit_list = unit_list(pop1);
% [MI, MI_pop, max_info(1, :)] = fig_info_rule(exp_data, unit_list);
% unit_list = exp_data.from_type(2);
% unit_list = unit_list(pop1);
% [MI, MI_pop, max_info(2, :)] = fig_info_rule(exp_data, unit_list);
% unit_list = exp_data.from_type(4);
% unit_list = unit_list(pop1);
% [MI, MI_pop, max_info(3,:)] = fig_info_rule(exp_data, unit_list);
% unit_list = exp_data.from_type(10);
% [MI, MI_pop, max_info(4,:)] = fig_info_rule(exp_data, unit_list);
% max_info(5,:) = [1.5860, 1.5860];
% max_info2 = max_info;
% 
% max_info = 0.5 * (max_info1 + max_info2)
% max_info_std = sqrt(0.5 .* ((max_info1 - max_info) .^ 2 + (max_info2 - max_info) .^ 2));
% save ../../figs/prospective/info_rule.mat max_info1 max_info2 max_info max_info_std
% 
% bar(max_info(:,2))
% hold on
% errorbar(max_info(:,2), max_info_std(:,2), '+');
% figure, bar(max_info(:,1))
% hold on
% errorbar(max_info(:,1), max_info_std(:,1), '+');
