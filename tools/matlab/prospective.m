function [info] = prospective (folder_in, pos_units, pred_units, step_idx, vars)
%function [pr_final, k, prez, p_tr, p_pr, score, k2, nprez, p_n, p_tn] = prospective (folder_in, pos_units, pred_units, step_idx, vars)
%function [pr_final, k, prez, p_tr, p_pr] = prospective (folder_in, pos_units, pred_units, step_idx, vars)
%
% folder_in = path to data_raw/exp/frs for example
% pos_units = list of unit number encoding the position
% pred_units = list of unit number supposedly encoding the predicted position
% step_idx = list of step indexes
% vars = matrix of all the variable (e.g., x y theta ...)
%
% Return:
% pr_final = the mean proba of correct prediction
% k = the list of activated state units
% prez(n1,n2) = the list of active predictive units when going from n1 to n2
% p_tr(n1,n2) = the proba of doing n1 to n2
% p_pr(n1,n2) = the predicted proba of doing n1 to n2
%
% Note:
% we ignore the transition 'end trial' to 'start trial'
% each trial is extracted after a bloc contact

% we used two hadoc variables (start position and bloc position)
start = [0,-0.05];
bloc = [0,-0.6];
% bloc = [0,-1] for 2nd block

% we extract the position indexes
nb_steps = size(step_idx, 1);
max_id_pos = zeros(nb_steps, 1);
max_fr = zeros(nb_steps, 1);
printf ("Extraction of 'temporal' firing rates of position units\n"); fflush(stdout);
for i = pos_units'
	filename = sprintf ('%s/%d.txt', folder_in, i);
	[fr] = firing_rate (filename, step_idx);
	% we find the max activity
	id = find (max_fr < fr);
	max_fr(id) = fr(id);
	max_id_pos(id) = i;
end
%nb_var = 2;
%vspace = [1.17 0; -0.68 -1.7];
%step = [0.05 0.05];
%v_max = vspace(1,:);
%v_min = vspace(2,:);
%nb_bin = ceil((v_max .- v_min) ./ step);
%v = ceil((vars(:,2:3) .-  repmat(v_min, nb_steps, 1)) ./  repmat(step, nb_steps, 1));
%max_id_pos = v(:,1) + v(:,2) .* (nb_bin (1) + 1);

max_id_pr = zeros(nb_steps, 1);
max_fr = zeros(nb_steps, 1);
printf ("Extraction of 'temporal' firing rates of predictive units\n"); fflush(stdout);
for i = pred_units'
	filename = sprintf ('%s/%d.txt', folder_in, i);
	[fr] = firing_rate (filename, step_idx);
	% we find the max activity
	id = find (max_fr < fr);
	max_fr(id) = fr(id);
	max_id_pr(id) = i;
end


% we need to start the analysis after the block is met (free decision)
% TODO: use vars.txt to extract directly these informations
% we look after the starting position indexes
id = find(sum((vars(:,2:3) .- repmat(start, size(vars,1), 1)).**2,2) < 0.01);
v = -1;
sample = [];
for i=1:size(id,1)
if id(i) - v > 1
sample = [sample id(i)];
end
v = id(i);
end
start_id = sample;
start_no = median(max_id_pos(start_id));
% we deduce the end position indexes
end_id = (sample-1)(find(sample-1 > 0));
end_id = [end_id size(vars,1)];
end_no = median(max_id_pos(end_id));
% we look after the bloc position indexes
id = find(sum((vars(:,2:3) .- repmat(bloc, size(vars,1), 1)).**2,2) < 0.01);
v = -1;
sample = [];
for i=1:size(id,1)
if id(i) - v > 1
sample = [sample id(i)];
end
v = id(i);
end
% we deduce the list of indexes for all the trials
id = [];
for i=1:size(sample,2)
find_id = find(end_id > sample(i))(1);
id = [id sample(i):end_id(find_id)];
end
max_id_pr = max_id_pr(id);
max_id_pos = max_id_pos(id);


% we extract the transition informations (prez, p_pr & p_tr)
[k,idx,jdx] = unique (max_id_pos);
[k2,idx2,jdx2] = unique (max_id_pr);
prez = cell(size(k,1), size(k,1));
p_tr = zeros(size(k,1), size(k,1));
p_n = zeros(size(k2,1),1);
nprez = zeros(size(k2,1), size(k,1), size(k,1));
%nprez = cell();
%for i=1:size(k2,1)
%	nprez(i) = zeros(size(k,1), size(k,1));
%end
last = find(k == max_id_pos(1));
tmp_pr = [];
for i=1:size(max_id_pos, 1)
	id = find(k == max_id_pos(i));
	if ~isequal(last, id)
		% nouvelle transition
		prez(last, id) = [prez{last,id} tmp_pr];
		p_tr(last, id)++;
		for n = tmp_pr
			p_n(find (k2 == n))++;
			nprez(find (k2 == n), last, id)++;
		end
		tmp_pr = [];
		last = id;
	end
	tmp_pr = [tmp_pr max_id_pr(i)];
end

for i=1:size(prez,1)
	for j=1:size(prez,2)
		c2m_prez = prez{i,j};
		if size(c2m_prez,2) == 0 
			p_pr(i,j) = 0; 
		else 
			p_pr(i,j) = max(count_element(c2m_prez).value)/size(c2m_prez,2);
		end
	end
end

% we ignore the last transtion (from end of trial to next start)
p_pr(find(k==end_no), :) = 0;
p_tr(find(k==end_no), :) = 0;

p_tr ./= sum(sum(p_tr));
pr_final = sum(sum(p_tr .* p_pr));

p_n ./= sum(p_n);
p_tn = [];
for i = 1:size(nprez,1)
	nprez(i,:,:) ./= sum(sum(nprez(i,:,:)));
	p_tn(i) = max(max(nprez(i,:,:)));
	htrs(i) = -nansum(nansum(nprez(i,:,:) .* log2(nprez(i,:,:))));
end
score = sum(p_n .* p_tn');

ht = -nansum(nansum(p_tr .* log2(p_tr)));
htr = sum(p_n .* htrs');
info = ht - htr;


