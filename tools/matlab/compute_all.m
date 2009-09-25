function [] = compute_all (data_path, exp, nbvars, days, trials)
% function [] = compute_all (data_path, exp, nbvars, days, trials)
%
% Params:
% data_path = '../../data/data_raw'
% exp = vector of exp to study
% nbvars = vars to use, e.g. 2:3 for spatial position
% days = vector of days to study
% trials = vector of trials to study for these days
%
% Note: i think that the program only works if you take days and trials,
% not for example d1 t1 / d2 t1 / d3 t1... To clarify !

%for f in `ls -d 1*`; do cd $f; echo "doing $f"; python frs.py; cd ..; done

% additional parameters
poolsize = 150
max_activated_pop = 100
% infos for each var (one var by column)
% vspace, e.g. [0 xMax yMax tMax rMax; 0 xMin yMin tMin rMin]
vspace_all = [0 1.17 0 pi 1; 0 -0.68 -1.7 -pi 0];
vspace = vspace_all(:,nbvars);
% step, e.g. [0 xStep yStep tStep rStep]
step_all = [0 0.05 0.05 0.2 0.05];
step = step_all(:,nbvars);
% coordinate of the goal in small tolman maze
goal = [0 -1.6];
% type to analyse
an_type = [0 1 2 3 10];
% TODO thresh pour le calcul dans reward
thresh = [0.2 0.07 0.07 0.2 0.2];

v = {};
infop = [];
for i = exp	
	printf ('Creating folders for extracted data (exp %d)\n', i);fflush(stdout);
	folder_out = sprintf ('%s/data_extracted/%d/', data_path, i);
	mkdir(folder_out);
	folder_out = sprintf ('%s/d%s_t%s/', folder_out, mat2str(days), mat2str(trials));
	mkdir(folder_out);
	
	printf ('Computation of vars for the days & trials (exp %d)\n',i);fflush(stdout);
	filename = sprintf ('%s/data_raw/%d/vars.txt', data_path, i);
	[vars, step_idx] = load_vars (filename, days, trials);
	goal_dist = @(x) sqrt(sum((x - repmat(goal, size(x, 1), 1)).**2, 2));
	vars = [vars goal_dist([vars(:,2),vars(:,3)])];
	
	printf ('Computation of the proba for selected vars (exp %d)\n',i);fflush(stdout);
	filename = sprintf ('%s/proba_%s.mat', folder_out, mat2str(nbvars));
	[probav, v] = proba_var (filename, vars(:, nbvars), step, vspace);

	filename = sprintf ('%s/data_raw/%d/type.txt', data_path, i);
	listn = load(filename);
	
	printf ('Computation of firing rates and associated values (exp %d)\n',i);fflush(stdout);
	folder_in = sprintf ('%s/data_raw/%d/frs/', data_path, i);
	frs = load_fr (folder_in, folder_out, listn(:,1), step_idx, v, probav);

%	printf ('Computation for population kurtosis (exp %d)\n',i);fflush(stdout);
%	filename = sprintf ('%s/pks_%s.mat', folder_out, mat2str(nbvars));
%	population_kurtosis (filename, frs, poolsize, probav);

%	printf ('Computation for active kurtosis (exp %d)\n',i);fflush(stdout);
%	filename = sprintf ('%s/paks_%s.mat', folder_out, mat2str(nbvars));
%	active_kurtosis (filename, frs, max_activated_pop);

%	printf ('Computation for lifetime kurtosis (exp %d)\n',i);fflush(stdout);
%	filename = sprintf ('%s/lks_%s.mat', folder_out, mat2str(nbvars));
%	lifetime_kurtosis (filename, frs, probav);

	printf ('Computation of firing rates vector (exp %d)\n',i);fflush(stdout);
	folder_in = sprintf ('%s/data_raw/%d/frs/', data_path, i);
	frs_vect = load_fr_vect (folder_in, folder_out, listn(:,1), step, vspace, vars, nbvars, probav);
	for k = an_type
		units = listn(find(listn(:,2) == k), 1);
		load('../../data/data_raw/list', 'list');
		printf ('Computation of information (exp %d)\n',i);fflush(stdout);
		filename = sprintf ('%s/info_%s_%d', folder_out, mat2str(nbvars), k);
		% on enleve les unites considerees comme silencieuse
		idx = find(max(frs(units,:)')' > 0.3);
		info_all (filename, frs_vect(idx, :), probav, frs(idx,:));
	end
	
	printf ('Computation of prospective information (exp %d)\n',i);fflush(stdout);
	pos_units = listn(find(listn(:,2) == 0),1);
	folder_in = sprintf ('%s/data_raw/%d/frs/', data_path, i);
	for k = 1:size(an_type,2)
		printf ('Computation of prospective information for type %d\n', an_type(k)); fflush(stdout);
		pred_units = listn(find(listn(:,2) == an_type(k)),1);
		infop(i,k) = prospective (filename, pos_units, pred_units, step_idx, vars)
	end	
	
	printf ('Computing reward correlations (exp %d)\n', i);fflush(stdout);
	folder_in = sprintf ('%s/data_raw/%d/', data_path, i);
	pos_units = listn(find(listn(:,2) == 0),1);
	for k = 1:size(an_type,2)
		rew_units = listn(find(listn(:,2) == an_type(k)),1);
		[v(i,k),corr(i,k)] = reward (folder_in, folder_out, pos_units, rew_units, thresh(k));
	end
end

filename = sprintf ('%s/data_extracted/prospective.mat', data_path);
save('-mat', filename, 'infop');

filename = sprintf ('%s/data_extracted/reward.mat', data_path);
save('-mat', filename, 'v', 'corr');


