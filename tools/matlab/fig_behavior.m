function fig_behavior(data_path, exp, reload, maze)
% function fig_behavior(data_path, exp, reload, maze)
% analyse du comportement de base
% maze = 1, 2 or 3 (resp. small, big ang huge tolman maze)

exp_all1 = zeros(length(exp), 3);
exp_all2 = zeros(length(exp), 3);
exp_all3 = zeros(length(exp), 3);
exp_fst1 = zeros(length(exp), 3);
exp_fst2 = zeros(length(exp), 3);
exp_fst3 = zeros(length(exp), 3);
nb_errors3 = zeros(length(exp), 1);
nb_errors2 = zeros(length(exp), 1);
nb_errors1 = zeros(length(exp), 1);
% choice_short = zeros(length(exp), 2);
for i = exp	
	fprintf('Working on experiment %d\n', i);
    exp_data = ExpData(data_path, i, reload, maze);
    
    [m_all1, m_fst1] = exp_data.get_mean_behavior(1);
    exp_all1(i, :) = m_all1; exp_fst1(i, :) = m_fst1; 
    nb_errors1(i) = exp_data.get_nb_errors(1, 1);
    [m_all2, m_fst2] = exp_data.get_mean_behavior(2:14);
    exp_all2(i, :) = m_all2; exp_fst2(i, :) = m_fst2; 
    nb_errors2(i) = exp_data.get_nb_errors(2:14, 2);
    [m_all3, m_fst3] = exp_data.get_mean_behavior(15);
    exp_all3(i, :) = m_all3; exp_fst3(i, :) = m_fst3;
    nb_errors3(i) = exp_data.get_nb_errors(15, 3);

%     choice_short(i, :) = hist(exp_data.trial(7).fst_choice, 0:1);
    clear exp_data;
end

% filename = sprintf ('%s/behavior.mat', data_path);
% save('-mat', filename, 'exp_all1', 'exp_all2', 'exp_all3', ...'exp_fst1', 'exp_fst2', 'exp_fst3');
figure, boxplot(exp_fst1);
%figure, hist(exp_fst1);
anova1(exp_fst1(:,1:2));
anova1(exp_fst1(:,[1,3]));
figure, hist(nb_errors1);
figure, boxplot(exp_fst2);
%figure, hist(exp_fst2);
anova1(exp_fst2(:,2:3));
figure, hist(nb_errors2);
figure, boxplot(exp_fst3);
%figure, hist(exp_fst3);
anova1(exp_fst3(:,2:3));
figure, hist(nb_errors3);
%save exp_fst3_huge exp_fst3;

% For the shortcut maze
% bar(sum(choice_short));
% [h,p,s] = chi2gof(0:1, 'frequency', sum(choice_short), 'expected',
% [length(exp)/2 length(exp)/2])

end
