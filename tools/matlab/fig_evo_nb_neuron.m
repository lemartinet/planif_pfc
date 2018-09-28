%load ../../data/data_raw/1/exp_data.mat;

%load ../../data/data_raw/2/exp_data.mat;

%load ../../data/data_raw/3/exp_data.mat;
%exp_data.spatial_0.pop1 = exp_data.spatial_0.active(exp_data.spatial_0.active <= 540);
%exp_data.spatial_0.pop2 = exp_data.spatial_0.active(exp_data.spatial_0.active > 540);

THRESH = 0.3;

POP1 = exp_data.from_type(10);
POP2 = exp_data.from_type(0);
POP2 = POP2(exp_data.spatial_0.pop1);
POP3 = exp_data.from_type(0);
POP3 = POP3(exp_data.spatial_0.pop2);

v_tot1 = zeros(1, exp_data.day(16).end / exp_data.step_size);
for i = POP1
	fr = exp_data.load_neuron(i);
	t = min(find(fr > THRESH));
	v = zeros(size(fr'));
	if ~isempty(t)
		v(t:end) = 1;
	end
	v_tot1 = v_tot1 + v;
end

v_tot2 = zeros(1, exp_data.day(16).end / exp_data.step_size);
for i = POP2
	fr = exp_data.load_neuron(i);
	t = min(find(fr > THRESH));
	v = zeros(size(fr'));
	if ~isempty(t)
		v(t:end) = 1;
	end
	v_tot2 = v_tot2 + v;
end

v_tot3 = zeros(1, exp_data.day(16).end / exp_data.step_size);
for i = POP3
	fr = exp_data.load_neuron(i);
	t = min(find(fr > THRESH));
	v = zeros(size(fr'));
	if ~isempty(t)
		v(t:end) = 1;
	end
	v_tot3 = v_tot3 + v;
end

figure, hold on
plot(v_tot1, 'r')
plot(v_tot2, 'g')
plot(v_tot3, 'b')

figure, hold on
plot(v_tot1, 'r')
plot(v_tot2, 'g')
plot(v_tot3, 'b')
xlim([0 exp_data.day(1).end / exp_data.step_size])

figure, hold on
plot(v_tot1 / max(v_tot1), 'r')
plot(v_tot2 / max(v_tot2), 'g')
plot(v_tot3 / max(v_tot3), 'b')
xlim([0 exp_data.day(1).end / exp_data.step_size])

% on garde que le premier jour
v_tot1 = v_tot1(1:exp_data.day(1).end / exp_data.step_size);
v_tot2 = v_tot2(1:exp_data.day(1).end / exp_data.step_size);
v_tot3 = v_tot3(1:exp_data.day(1).end / exp_data.step_size);
%v_tot1_1=v_tot1; v_tot1_2=v_tot2; v_tot1_3=v_tot3;
%v_tot2_1=v_tot1; v_tot2_2=v_tot2; v_tot2_3=v_tot3;
%v_tot3_1=v_tot1; v_tot3_2=v_tot2; v_tot3_3=v_tot3;

% même chose pr 2 autres exp
% [...]

m = max([length(v_tot1_1), length(v_tot2_1), length(v_tot3_1)]);
v_POP1 = [
	[v_tot1_1, v_tot1_1(end) * ones(1,m-length(v_tot1_1))];
	[v_tot2_1, v_tot2_1(end) * ones(1,m-length(v_tot2_1))];
	[v_tot3_1, v_tot3_1(end) * ones(1,m-length(v_tot3_1))]
	];
v_POP2 = [
	[v_tot1_2, v_tot1_2(end) * ones(1,m-length(v_tot1_2))];
	[v_tot2_2, v_tot2_2(end) * ones(1,m-length(v_tot2_2))];
	[v_tot3_2, v_tot3_2(end) * ones(1,m-length(v_tot3_2))]
	];
v_POP3 = [
	[v_tot1_3, v_tot1_3(end) * ones(1,m-length(v_tot1_3))];
	[v_tot2_3, v_tot2_3(end) * ones(1,m-length(v_tot2_3))];
	[v_tot3_3, v_tot3_3(end) * ones(1,m-length(v_tot3_3))]
	];
save evo_population_d1.mat v_POP1 v_POP2 v_POP3;

figure, hold on
shadedErrorBar(1:size(v_POP1,2), mean(v_POP1), std(v_POP1), 'r')
shadedErrorBar(1:size(v_POP2,2), mean(v_POP2), std(v_POP3), 'g')
shadedErrorBar(1:size(v_POP3,2), mean(v_POP3), std(v_POP3), 'b')

