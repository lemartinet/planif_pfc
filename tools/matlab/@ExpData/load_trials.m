function load_trials(exp_data, maze)
% charge les infos de mouvement:
% trial[day,trial].start/.end
% trial[...].pos
% trial[...].angle

% infos for each var (one var by column)
% vspace, e.g. [0 xMax yMax tMax rMax; 0 xMin yMin tMin rMin]
vspace_allmaze{1} = [0 1.17 0 pi 1; 0 -0.68 -1.7 -pi 0];
vspace_allmaze{2} = [0 2.09 0.77 pi 1; 0 -1.19 -2.40 -pi 0];
vspace_allmaze{3} = [0 4 2.46 pi 1; 0 -2.4 -3.78 -pi 0];
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
for i = 1:15
	dvar = [];
	for j = 1:12
        if (i == 15 && j > 7) || (i == 16 && j > 6)
            break;
        end
		id = find( vars(:,2) == i & vars(:,3) == j );
		if ~isempty(id)
			exp_data.trial(i,j).start = vars(id(1),1);
			exp_data.trial(i,j).end = vars(id(end),1);
			exp_data.trial(i,j).pos = vars(id,4:5);
			exp_data.trial(i,j).angle = vars(id,6);
			nbstep = size(exp_data.trial(i,j).pos, 1);
			exp_data.trial(i,j).goal_dist = ...
				sqrt(sum((exp_data.trial(i,j).pos - repmat(goal, nbstep, 1)) .^2 , 2));
			[exp_data.trial(i,j).pv_space, exp_data.trial(i,j).v_space] = ...
				proba_var (exp_data.trial(i,j).pos, step, vspace);
			dvar = [dvar ; exp_data.trial(i,j).pos];
		end
    end
	exp_data.day(i).start = [exp_data.trial(i,:).start];
	exp_data.day(i).start = exp_data.day(i).start(1);
	exp_data.day(i).end = [exp_data.trial(i,:).end];
	exp_data.day(i).end = exp_data.day(i).end(end);
	[exp_data.day(i).pv_space, exp_data.day(i).v_space] = proba_var (dvar, step, vspace);
	var = [var ; dvar];
end
[exp_data.pv_space, exp_data.v_space] = proba_var (var, step, vspace);

end

