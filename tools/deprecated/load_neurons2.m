function [exp_data] = load_neurons2(exp_data)

idx = find(exp_data.pv_space > 0);
v_size = size(exp_data.pv_space);
for i = 1 : exp_data.nbneurons
    p = exp_data.pv_space;
	fprintf('Extracting neuron %d\n', i);
	for d = 1 : 16
		for t = 1 : 12
			if (d == 15 && t > 7) || (d == 16 && t > 6)
				break;
            end
            tfr = p;
			tfr(idx) = exp_data.neuron(i).trial(d,t).fr;
			[c, fs, m_fr, s_fr] = field_properties (tfr, exp_data.trial(d,t).pv_space);
			exp_data.neuron(i).trial(d,t).center = c;
			exp_data.neuron(i).trial(d,t).fsize = fs;
			exp_data.neuron(i).trial(d,t).m_fr = m_fr;
			exp_data.neuron(i).trial(d,t).s_fr = s_fr;
        end
        tfr = p;
		tfr(idx) = exp_data.neuron(i).day(d).fr;
		[c, fs, m_fr, s_fr] = field_properties (tfr, exp_data.day(d).pv_space);
		exp_data.neuron(i).day(d).center = c;
		exp_data.neuron(i).day(d).fsize = fs;
		exp_data.neuron(i).day(d).m_fr = m_fr;
		exp_data.neuron(i).day(d).s_fr = s_fr;
    end
    tfr = p;
	tfr(idx) = exp_data.neuron(i).fr;
	[c, fs, m_fr, s_fr] = field_properties (tfr, exp_data.pv_space);
	exp_data.neuron(i).center = c;
	exp_data.neuron(i).fsize = fs;
	exp_data.neuron(i).m_fr = m_fr;
	exp_data.neuron(i).s_fr = s_fr;
end

end

