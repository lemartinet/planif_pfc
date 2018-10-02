function [vars, step_idx] = load_vars (file, days, trials)
% function [vars, step_idx] = load_vars (file, days, trials)


% pour l'instant on ne traite que le cas ou l'on étudie des segments 
% de temps continues les uns à la suite des autres
% je pense qu'avec des cells on pourrait enregistrer des listes de segments
vars_all = load(file);
vars = [];
step_idx = [];
for d=days
	for t=trials
		idx = find (vars_all(:,2) == d & vars_all(:,3) == t);
		if ~isempty(idx)
			vars_tmp = [vars_all(idx,1) vars_all(idx,4) vars_all(idx,5) vars_all(idx,6)];
			vars = [vars ; vars_tmp];
			step_idx = [step_idx ; idx];
		end
	end
end

