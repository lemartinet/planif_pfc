function load_trials_shortcut(exp_data, maze)
% charge les infos de mouvement:
% trial[day,trial].start/.end
% trial[...].pos
% trial[...].angle

% infos for each var (one var by column)
% vspace, e.g. [0 xMax yMax tMax rMax; 0 xMin yMin tMin rMin]
vspace_allmaze{1} = [0 1.7 0.87 pi 1; 0 -0.02 -2.34 -pi 0];
vspace_all = vspace_allmaze{maze};
vspace = vspace_all(:, 2:3);
% step, e.g. [0 xStep yStep tStep rStep]
step_all = [0 0.05 0.05 0.2 0.05];
step = step_all(:, 2:3);
% coordinate of the goal in small tolman maze
goal = [0 -1.6];

vars = load(sprintf('%s/vars.txt', exp_data.path));
exp_data.step_size = vars(2,1) - vars(1,1);
var = [];
for i = 1:7
    id = find(vars(:,2) == i);
    if ~isempty(id)
        exp_data.trial(i).start = vars(id(1),1);
        exp_data.trial(i).end = vars(id(end),1);
        exp_data.trial(i).pos = vars(id,3:4);
        exp_data.trial(i).angle = vars(id,5);
        nbstep = size(exp_data.trial(i).pos, 1);
        exp_data.trial(i).goal_dist = ...
            sqrt(sum((exp_data.trial(i).pos - repmat(goal, nbstep, 1)) .^2 , 2));
        [exp_data.trial(i).pv_space, exp_data.trial(i).v_space] = ...
            proba_var(exp_data.trial(i).pos, step, vspace);
        var = [var ; exp_data.trial(i).pos];
    end
end
[exp_data.pv_space, exp_data.v_space] = proba_var(var, step, vspace);

end

