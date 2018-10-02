function [frs, trs] = treves_rolls (exp, nbcells, step, type, reload, poolsize)

frs = load_fr(exp, nbcells, step, type, reload);
nb_places = size(frs)(2);

pks = [];
for i=1:nb_places
	% we complete missing data
	noise = mean (frs(find(frs(:,i) < std (frs(:,i)))));
	%noise = 0.01;
	for ind=nbcells+1:poolsize
		frs(ind,i) = noise;
	end
	
	den = 0;
	num = 0;
	%for j=1:nbcells
	for j=1:poolsize
		num += abs(frs(j,i));
		den += frs(j,i)**2;
	end
	%trs(i) = 1 - (num/nbcells)**2 / (den/nbcells);
	trs(i) = 1 - (num/poolsize)**2 / (den/poolsize);
end

