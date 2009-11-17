function [exp_data] = load_trials(exp_data)
% charge les infos de comportement:
% trials[day,trial].start/.end
% trials[...].num
% trials[...].protocol
% trials[...].decision
% trials[...].pos
% trials[...].angle

% infos for each var (one var by column)
% vspace, e.g. [0 xMax yMax tMax rMax; 0 xMin yMin tMin rMin]
vspace_all = [0 1.17 0 pi 1; 0 -0.68 -1.7 -pi 0];
vspace = vspace_all(:, 2:3);
% step, e.g. [0 xStep yStep tStep rStep]
step_all = [0 0.05 0.05 0.2 0.05];
step = step_all(:, 2:3);
% coordinate of the goal in small tolman maze
goal = [0 -1.6];

% ajout de fonctions utiles
exp_data.from_dt = @(x,y) (x-1) * 12 + y;
exp_data.from_num = @(x) 1 + [floor((x-1)/12) , mod(x-1,12)];

load( sprintf('%s/vars.txt', exp_data.path) );
load( sprintf('%s/protocol.txt', exp_data.path) );
load( sprintf('%s/events.txt', exp_data.path) );
var = [];
for i = 1:15
	dvar = [];
	for j = 1:12
		if i == 15 && j > 7
			break;
		end
		id = find( vars(:,2) == i & vars(:,3) == j );
		if ~isempty(id)
			exp_data.trial(i,j).start = vars(id(1),1);
			exp_data.trial(i,j).end = vars(id(end),1);
			exp_data.trial(i,j).pos = vars(id,4:5);
			exp_data.trial(i,j).angle = vars(id,6);
			% TODO changer si on vire la premiere colonne
			id = find( protocol(:,2) == i & protocol(:,3) == j );
			exp_data.trial(i,j).protocol = protocol(id,4);
			% TODO changer si on met P4 à la place de P0
			id = find( events(:,2) == i & events(:,3) == j );
			exp_data.trial(i,j).decision = events(id,5);
			nbstep = size(exp_data.trial(i,j).pos, 1);
			
			exp_data.trial(i,j).goal_dist = ...
				sqrt(sum((exp_data.trial(i,j).pos - repmat(goal, nbstep, 1)).^2, 2));
			[exp_data.trial(i,j).pv_space, exp_data.trial(i,j).v_space] = ...
				proba_var (exp_data.trial(i,j).pos, step, vspace);
			dvar = [dvar ; exp_data.trial(i,j).pos];
		end
	end
	exp_data.day(i).start = {exp_data.trial(i,:).start};
	exp_data.day(i).start = exp_data.day(i).start{1};
	exp_data.day(i).end = {exp_data.trial(i,:).end};
	exp_data.day(i).end = exp_data.day(i).end{end};
	[exp_data.day(i).pv_space, exp_data.day(i).v_space] = proba_var (dvar, step, vspace);
	var = [var ; dvar];
end
[exp_data.pv_space, exp_data.v_space] = proba_var (var, step, vspace);

