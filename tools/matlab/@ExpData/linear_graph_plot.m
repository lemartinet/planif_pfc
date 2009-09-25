function linear_graph_plot(exp_data, path, neurons)

fr = exp_data.lin_graph.fr{path};
fr = fr(ismember(exp_data.lin_graph.neurons{path}, neurons), :);
%figure, imagesc(exp_data.lin_graph.dist{path}, exp_data.lin_graph.neurons{path}, fr);
figure, imagesc(fr);
end
