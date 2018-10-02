function [] = info_all (save_file, frs_vect, probav, frs)

p = probav(find(probav > 0));

printf ("Computation information of single cells\n");fflush(stdout);
nbcells = size(frs_vect,1);
infos = [];
surprises = [];
e = [];
ce = [];
ce_alls = [];
samples = [];
for i=1:nbcells
	[infos(i), surprises(i,:), e(i), ce(i), ce_alls(i,:), samples(i,:)] = mutual_information (frs_vect (i,:)', probav);
	C(i) = mean((surprises(i,:) .-infos(i)) .* (frs(i,:) - sum(frs(i,:) .* p'))) / (sum(surprises(i,:)) * sum(frs(i,:)));
	correl(i) = cor (surprises(i,:), frs(i,:));
end
filename = sprintf ("%s.mat", save_file);
save ("-mat", filename, "infos", "surprises", "e", "ce", "ce_alls", "samples", "C", "correl");

%printf ("Computation of information for the population\n");fflush(stdout);
%[info, surprise, e, ce, ce_all, sample] = mutual_information_pop (frs_vect, probav);
%ratio_redundant = info / sum(infos);
%filename = sprintf ("%s_pop.mat", save_file);
%save ("-mat", filename, "info", "surprise", "e", "ce", "ce_all", "sample", "ratio_redundant");


