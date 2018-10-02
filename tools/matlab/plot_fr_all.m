function [] = plot_fr_all (data_path, nbexp)

for i = 1:nbexp
	filename = sprintf ('%s/data_extracted/%d/count_neuron', data_path, i);
	nbneuron = load(filename);
	
	plot_fr (data_path, i, nbneuron(1));
end
