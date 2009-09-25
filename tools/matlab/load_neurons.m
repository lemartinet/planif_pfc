function [exp_data] = load_neurons(exp_data)

% charge un fichier de donnee brut
exp_data.load_neuron = @(x) load(sprintf('%s/frs/%d.mat', exp_data.path, x), 'fr');

load(sprintf('%s/type.txt', exp_data.path));
exp_data.nbneurons = size(type,1);
load(sprintf('%s/col.txt', exp_data.path));
load(sprintf('%s/minicol.txt', exp_data.path));
load(sprintf('%s/minicol_col.txt', exp_data.path));
exp_data.col_of_minicol(minicol_col(:,1)) = minicol_col(:,2);

% we get the index of positive probav
idx = find(exp_data.pv_space > 0);
v_size = size(exp_data.pv_space);
for i = 1 : exp_data.nbneurons
	fprintf('Extracting neuron %d: ', i);
	exp_data.neuron(i).type = type(i);
	exp_data.neuron(i).col = col(find(col(:,1) == i), 2);
	exp_data.neuron(i).minicol = minicol(find(minicol(:,1) == i), 2);
	fr = exp_data.load_neuron(i);

	fprintf(', field and theta info...\n');	
	for d = 1 : 15
		for t = 1 : 12
			if d == 15 && t > 7
				break;
			end
			step_idx = exp_data.trial(d,t).start : exp_data.trial(d,t).end;
			[tfr] = firing_rate (fr(step_idx), exp_data.trial(d,t).v_space, v_size);
			exp_data.neuron(i).trial(d,t).fr = tfr(idx);
			[c, fs, m_fr, s_fr] = field_properties (tfr, exp_data.trial(d,t).pv_space);
			exp_data.neuron(i).trial(d,t).center = c;
			exp_data.neuron(i).trial(d,t).fsize = fs;
			exp_data.neuron(i).trial(d,t).m_fr = m_fr;
			exp_data.neuron(i).trial(d,t).s_fr = s_fr;
			exp_data.neuron(i).trial(d,t).theta = theta(fr(step_idx));
		end
		step_idx = exp_data.day(d).start : exp_data.day(d).end;
		[tfr] = firing_rate (fr(step_idx), exp_data.day(d).v_space, v_size);
		exp_data.neuron(i).day(d).fr = tfr(idx);
		[c, fs, m_fr, s_fr] = field_properties (tfr, exp_data.day(d).pv_space);
		exp_data.neuron(i).day(d).center = c;
		exp_data.neuron(i).day(d).fsize = fs;
		exp_data.neuron(i).day(d).m_fr = m_fr;
		exp_data.neuron(i).day(d).s_fr = s_fr;
		exp_data.neuron(i).day(d).theta = theta(fr(step_idx));
	end
	[tfr] = firing_rate (fr, exp_data.v_space, v_size);
	exp_data.neuron(i).fr = tfr(idx);
	[c, fs, m_fr, s_fr] = field_properties (tfr, exp_data.pv_space);
	exp_data.neuron(i).center = c;
	exp_data.neuron(i).fsize = fs;
	exp_data.neuron(i).m_fr = m_fr;
	exp_data.neuron(i).s_fr = s_fr;
	exp_data.neuron(i).theta = theta(fr);
end

% retourne les indices de neurone en fonction de leur type x
exp_data.from_type = @(x) find([exp_data.neuron.type] == x);

% retourne les indices de neurone appartenant à une colonne x
% !!! some neurons don't belong to a col (e.g., sup neuron) -> cellfun
exp_data.from_col = @(x) find(cellfun(@(y) ~isempty(y) && y == x, {exp_data.neuron.col}) == 1);

% retourne les indices de neurone appartenant à une minicolonne x
% !!! some neurons don't belong to a minicol (e.g., state neuron) -> cellfun
exp_data.from_minicol = @(x) find(cellfun(@(y) ~isempty(y) && y == x, {exp_data.neuron.minicol}) == 1);

% retourne les indices de minicolonnes appartenant à une colonne x
exp_data.minicols_of_col = @(x) find(exp_data.col_of_minicol == x);

% retourne les indices de neurone dont le centre de ch recep est proche (< z) d'une position (x,y)
exp_data.at_position = @(x,y,z) find(cellfun(@(v) sum((v - [x y]).^2, 2), {exp_data.neuron.center}) < z);

