function fig_recept_field(data_path, exp, reload, maze, linear)
% function fig_recept_field(data_path, exp, reload, maze)
% Simple plots des ch récept spatiaux = place fields
% maze = 1, 2 or 3 (resp. small, big ang huge tolman maze)

PATH = 3;
MAX_FR = 1;

if nargin == 4
    linear = false;
end

% type to analyse
an_type = [10, 0, 1, 2, 3, 4];

for i = exp	
	fprintf('Working on experiment %d\n', i);
    exp_data = ExpData(data_path, i, reload, maze);
    exp_data.linear_graph();
    
    mkdir(sprintf('%s/plot_fr/', exp_data.path));
    for t = an_type
        if ~linear
            mkdir(sprintf('%s/plot_fr/%d', exp_data.path, t));
            for n = exp_data.from_type(t)
                fr = exp_data.neuron(n).fr;
                p = exp_data.pv_space;
                p(p>0) = fr;
                f = figure('Visible', 'off');
                contourf(p, 'LineWidth', 0.01); %hold on; contour(p);
                colorbar;
                saveas(f, sprintf('%s/plot_fr/%d/%d.pdf', exp_data.path, t, n), 'pdf');
                close(f);

                f = figure('Visible', 'off');
                imagesc(p);
                colorbar;
                saveas(f, sprintf('%s/plot_fr/%d/%d_pix.pdf', exp_data.path, t, n), 'pdf');
                close(f);
            end
        else
            mkdir(sprintf('%s/plot_fr/%d_lin_path%d', exp_data.path, t, PATH));
            fr = exp_data.linear_graph_plot(PATH, 1:exp_data.nbneurons);
            for n = exp_data.from_type(t)
                f = figure('Visible', 'off');
                area(fr(exp_data.lin_graph.neurons{PATH} == n, :));
                v = axis; axis([v(1:2), 0, MAX_FR]);
                saveas(f, sprintf('%s/plot_fr/%d_lin_path%d/%d.pdf', exp_data.path, t, PATH, n), 'pdf');
                close(f);

                f = figure('Visible', 'off');
                plot(fr(exp_data.lin_graph.neurons{PATH} == n, :));
                v = axis; axis([v(1:2), 0, MAX_FR]);
                saveas(f, sprintf('%s/plot_fr/%d_lin_path%d/%d_plot.pdf', exp_data.path, t, PATH, n), 'pdf');
                close(f);
            end
        end
    end
end

end