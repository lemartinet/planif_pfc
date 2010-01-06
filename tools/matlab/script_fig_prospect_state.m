%load ../../data/data_raw/1/exp_data.mat

TYPE = 2;
PATH = 3;
MAX_FR = 0.8;
NB_NEURON = 3602;

% recherche des neurones qu'on veut afficher
% for i = exp_data.from_type(TYPE)
%     p(p>0) = exp_data.neuron(i).fr;
%     figure, imagesc(p);
% end
N = exp_data.from_type(TYPE);
P = N([10, 13, 16, 19]);
fr = exp_data.linear_graph_plot(PATH, 1:NB_NEURON);

for i = 1:length(P)
    Pidx(i) = find(exp_data.lin_graph.neurons{PATH} == P(i));
    Sidx(i) = find(exp_data.lin_graph.neurons{PATH} == P(i)-2);
    figure, hold on, area(fr(Pidx(i), :));
    area(fr(Sidx(i), :), 'FaceColor','r');
    v = axis; axis([v(1:2), 0, MAX_FR]);
    figure, hold on, plot(fr(Pidx(i), :));
    plot(fr(Sidx(i), :), 'r');
    v = axis; axis([v(1:2), 0, MAX_FR]);
end
