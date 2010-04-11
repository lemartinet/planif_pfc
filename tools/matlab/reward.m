function [v,corr] = reward (folder_in, folder_out, pos_units, rew_units, thresh)
% thresh = 0.2 pour type 3 et 0.07 pour type 1

% TODO et pour les PC ?
% TODO faire une autre mesure pr reward: 
%	à chaque di<d<dj on note l'activité de la pop, et on peut en sortir une info ?
% car en fait l'activité d'un neurone seul ne dit rien car il faut une mesure de référence
% BUG !!! bug dans reward : on utilise toujours exp 1 !!!

% on associe les neurones d'une meme colonne
filename = sprintf('%s/col.txt', folder_in);
col=load(filename);
filename = sprintf('%s/minicol.txt', folder_in);
minicol=load(filename);
filename = sprintf('%s/minicol_col.txt', folder_in);
minicol_col=load(filename);

for i=1:size(pos_units,1)
	colpos(i,:) = col(find(col(:,1) == pos_units(i)),:);
end
j=1;
for i=1:size(rew_units,1)
	minicolrew = minicol(find(minicol(:,1) == rew_units(i)),2);
	mc = find (minicol_col(:,1) == minicolrew);
	if ~isempty(mc)
	colrew(j) = minicol_col(mc,2);
	newrew_units(j) = rew_units(i);
	j++;
	end
end
pos_units = [];
for i=1:size(colrew,2)
	pos_units(i) = colpos(find(colpos(:,2) == colrew(i)),1);
end
pos_units = pos_units';
rew_units = newrew_units';

% on recupere les bons indexes
filename = sprintf('%s/frs_[2,3].mat', folder_out);
frs_data = load(filename);
c = frs_data.centers(pos_units,:);
frs = frs_data.frs(rew_units,:);
idx = find(frs(:,1) > thresh);

% coordinate of the goal in small tolman maze
goal = [0 -1.6];
goal_dist = @(x) sqrt(sum((x .- repmat(goal, size(x, 1), 1)).**2, 2));

v= [goal_dist(c(idx,:)) max(frs(idx,:)')'];
%v= [goal_dist(c(idx,:)) frs(idx,1)];
%plot(v(:,1),v(:,2),'o');
corr = cor(v(:,1),v(:,2));

%[a,b]=hist(v(:,1),5);
%a./=sum(a);
%[a2,b2]=hist(v(:,2),5);
%a2./=sum(a2);

%folder_in = "../../data/data_raw/1cl/"
%folder_in2 = "../../data/data_extracted/1cl/d1_14t1_12/"
%filename = sprintf ('%s/type.txt', folder_in);
%units = load(filename);
%pos_units = units(find(units(:,2) == 0),1);
%rew_units = units(find(units(:,2) == 1),1);
%reward (folder_in2, pos_units, rew_units, 0.07)



