%load ../../data/data_raw/1/exp_data.mat;
imagesc(exp_data.pv_space); % trouver les coord du centre
bn = exp_data.at_position(14,27,0.1); % x,y inversé par rapport au dessin
p = exp_data.pv_space;
p(p>0) = exp_data.neuron(bn(1)).fr; figure,imagesc(p); % bn = 606
c = exp_data.neuron(bn(1)).col;
m = exp_data.minicols_of_col(c);
idx2 = exp_data.get_step_idx(2,1);
idx15 = exp_data.get_step_idx(15,1:2);

c = ['r', 'g', 'b'];
for i = 1:2
    for j = 1:3
        n = exp_data.from_minicol(m(j));
        fr = exp_data.load_neuron(n(i));
        figure((i - 1) * 2 + 1)
        plot(fr(idx2), c(j))
        hold on
        figure((i - 1) * 2 + 2)
        plot(fr(idx15), c(j))
        hold on
    end
end
