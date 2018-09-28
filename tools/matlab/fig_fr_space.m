%% commande utilisée pour produire la figure avec les réponses des neurones
%% V dans le domaine fréquentiel, plus le calcul du kurtosis de population

cd /media/disk/martinet/normal/tools/matlab/
load ../../data/data_raw/1/exp_data_neuron.mat;
exp_data
exp_data.neuron[exp_data.from_type(1)]
exp_data.neuron(exp_data.from_type(1))
exp_data.neuron(exp_data.from_type(1)).fsize
[exp_data.neuron(exp_data.from_type(1)).fsize]
plot([exp_data.neuron(exp_data.from_type(1)).fsize])
figure, hist([exp_data.neuron(exp_data.from_type(1)).fsize])
exp_data.neuron(exp_data.from_type(1)).fr
figure, plot(exp_data.neuron(exp_data.from_type(1)).fr)
figure, plot(exp_data.neuron(exp_data.from_type(1)).fr, '+')
figure, plot(exp_data.neuron(exp_data.from_type(1)).fr, 'o')
figure, plot(exp_data.neuron(exp_data.from_type(1)).fr, '-o')
figure, plot(exp_data.neuron(exp_data.from_type(1)).fr', '-o')
figure, plot([exp_data.neuron(exp_data.from_type(1)).fr]', '-o')
figure, plot([exp_data.neuron(exp_data.from_type(1)).fr], '+')
exp_data.from_type(1)
v=exp_data.from_type(1)
v(1)
figure, plot([exp_data.neuron(2404).fr], '+')
hold on
plot(mean([exp_data.neuron(2404).fr]));
plot(ones(size([exp_data.neuron(2404).fr])) .* mean([exp_data.neuron(2404).fr]));
p = exp_data.pv_space;
p(p>0) = [exp_data.neuron(2404).fr];
image(p)
imagesc(p)
mean([exp_data.neuron(2404).fr])
contourf(p)
v = exp_data.from_type(4);
v(1)
figure, plot([exp_data.neuron(604).fr], '+')
p(p>0) = [exp_data.neuron(604).fr];
imagesc(p)
p(p>0) = [exp_data.neuron(604).day(15).fr];
imagesc(p)
plot([exp_data.neuron(604).day(15).fr], '+');
hist([exp_data.neuron(604).day(15).fr])
figure, hist([exp_data.neuron(v(2)).day(15).fr])
figure, hist([exp_data.neuron(v(3)).day(15).fr])
for i=1:30
figure, hist([exp_data.neuron(v(i)).day(15).fr])
end
figure, hist([exp_data.neuron(v(1:20)).day(15).fr])
exp_data.neuron(v(1:20))
exp_data.neuron(v(1:20)).day(15)
exp_data.neuron(v).day(15)
v
exp_data.neuron(v(:)).day(15)
exp_data.neuron(1:100).
exp_data.neuron(1:100).day
exp_data.neuron(1:100).day(15)
exp_data.neuron(v(1:20))
e2=exp_data.neuron(v(1:20));
e2.day
e2.day(15)
exp_data.neuron.day(15)
exp_data.neuron.day
exp_data.neuron.day.fr
exp_data.neuron(1).day(15).fr
exp_data.neuron(1:2).day(15).fr
m=[]
for i=1:30
m = [m, [exp_data.neuron(v(i)).day(15).fr]];
end
size(m)
hist(m(:),100)
figure, hist(m(20), 100)
figure, hist(m(:,20), 100)
figure, hist(m(:,21), 100)
figure, hist(m(:,5), 100)
figure, hist(m(:,6), 100)
figure, hist(m(:,7), 100)
m=[];
or i=1:7
m = [m, [exp_data.neuron(v(i)).day(1).fr]];
end
for i=1:7
m = [m, [exp_data.neuron(v(i)).day(1).fr]];
end
hist(m(:),100)
exp_data
exp_data.lin_graph
exp_data.lin_graph.neurons
exp_data.lin_graph.neurons{1}
hist(m(:,7),100)
hist(m(:,6),100)
hist(m(:,5),100)
hist(m(:,4),100)
m=[]; for i=1:7
m = [m, [exp_data.neuron(v(i)).day(14).fr]];
end
hist(m(:),100)
m=[]; for i=1:20
m = [m, [exp_data.neuron(v(i)).day(14).fr]];
end
hist(m(:),100)
for i=1:20
figure, hist(m(:,i),100)
end
for i=1:20
figure, axis([0 1 axis(3:4)]), hist(m(:,i),100)
end
axis
axis(3:4)
axis([3:4])
axis([0 1])
doc axis
for i=1:20
figure, xlim([0 1]), hist(m(:,i),100)
end
for i=1:20
figure, hist(m(:,i),100), xlim([0 1])
end
m2 = m(:,[10:19, 5, 6]);
hist(m2(:), 100);
xlim([0.3 1])
figure,hist(m, 100)
xlim([0.3 1])
[a,b,c,d]=hist(m, 100)
[a,b,c]=hist(m, 100)
[a,b]=hist(m, 100)
a
size(a)
size(m(1,:))
kurtosis(m(1,:))
figure, plot(m(1,:))
figure, plot(m(2,:))
[a,b]=hist(m2, 100)
figure, plot(m(1,:))
figure, plot(a(1,:))
figure, plot(a(2,:))
figure, hold on
for i=1:12
plot(a(i,:))
end
plot(a(:,11))
plot(a(10:end,11))
plot(a(10:end,10))
plot(a(10:end,9))
figure
% on ignore les dix premiers bins (activités nulles)
a = a(10:end, :);
for i=1:12
plot(a(:,i))
end
hold on
figure, hold on, for i=1:12
plot(a(:,i))
end
a(27,:)
a(28,:)
a(1,:)
kurtosis(a(1,:))
kurtosis(a(1:2,:))
for i=1:length(a)
pk(i) = kurtosis(a(i,:));
end
pk
for i=1:length(a)
nb_a = sum(a(i,:));
end
nb_a
for i=1:length(a)
nb_a(i) = sum(a(i,:));
end
nb_a
nb_a = nb_a / sum(nb_a)
nansum(pk .* nb_a)

figure, hold on, for i=v([10:19, 5, 6])
fr=exp_data.load_neuron(i);
fr = fr(exp_data.get_step_idx(14, 12));
plot(fr);
end

pk_V = pk;
%% en refaisant avec un 20 bins (au lieu de 100) on trouve  9.1673 pr le
%% kurtosis (au lieu de 10,0909)


%%  affichage de la même chose pour les S
v=exp_data.from_type(0);
FR=[];figure, hold on, for i=v([2,6:19])
fr=exp_data.load_neuron(i);
fr2 = fr(exp_data.get_step_idx(15, 2));
FR(i,:) = fr2(63:191);
end
plot(FR')

FR2 = FR(v([2,6:19]), :);
figure,hist(FR2')
figure,hist(FR2(:), 100)
a=hist(FR2(:), 100);
[a,b]=hist(FR2(:), 100);
[a,b]=hist(FR2, 100);
[a,b]=hist(FR2', 100);
% on ignore les dix premiers bins (bruit de base)
a = a(10:end, :);
a
for i=1:length(a)
pk(i) = kurtosis(a(i,:));
end
pk
for i=1:length(a)
nb_a = sum(a(i,:));
end
nb_a
for i=1:length(a)
nb_a(i) = sum(a(i,:));
end
nb_a
nansum(pk .* nb_a)
nb_a = nb_a / sum(nb_a)
nansum(pk .* nb_a)
pk_S = pk;

boxplot([pk_S' pk_V'])
anova1([pk_S' pk_V'])
%% nanmean(pk_S) = 4.2371
%% nanmean(pk_V) = 10.0909

%% maintenant on fait le calcul de la redondance
s=std(a_V,[],2);
for i=1:length(a_V)
a_V_thresh(i,:) = a_V(i,:) > s(i);
end
redond_V=sum(a_V_thresh,2)

s=std(a_S,[],2);
for i=1:length(a_S)
a_S_thresh(i,:) = a_S(i,:) > s(i);
end
redond_S=sum(a_S_thresh,2)

boxplot([redond_S redond_V])
anova1([redond_S redond_V])
%% nanmean(redond_S) = 2.5934
%% nanmean(redond_V) = 0.4176


%% on refait ces calculs dans l'espace du maze
for i=1:length(m2)
pk(i) = kurtosis(m2(i,:));
end
boxplot(pk)
size(FR2)
length(exp_data.pv_space > 0)
length(find(exp_data.pv_space > 0))
pk_V2=pk;
pk=[];for i=1:length(FR2)
pk(i) = kurtosis(FR2(:,i));
end
boxplot(pk)
FR2(:,1)
FR2(:,10)
pk_S2 = pk;
boxplot([pk_S2' pk_V2'])
anova1([pk_S2' pk_V2'])
pk_S2(1=
pk_S2(1)
pk_S2(end)
pk_S2=pk_S2(1:128);
boxplot([pk_S2' pk_V2'])
anova1([pk_S2' pk_V2'])
%% nanmean(pk_S2) = 9.6274
%% nanmean(pk_V2) = 1.9713  

s=std(m2,[],2);
for i=1:length(m2)
m2_V_thresh2(i,:) = m2(i,:) > s(i);
end
redond_V2=sum(m2_V_thresh2,2);
%% on elimine les zones non samplées
id = find(redond_V2 ~= 0);
redond_V2 = redond_V2(id)

s=std(FR2',[],2);
for i=1:length(FR2)
a_S_thresh2(i,:) = FR2(:,i) > s(i);
end
redond_S2=sum(a_S_thresh2,2);
redond_S2=redond_S2(id)

redond_S2 = redond_S2(7:end)
redond_V2 = redond_V2(7:end)

boxplot([redond_S2 redond_V2])
anova1([redond_S2 redond_V2])

%% j'ai refait quelques courbes qu'il me manquait (ou à améliorer)
[a,b]=hist(FR2', 100);
hist(FR2', 100);
hist(FR2(:), 100);
xlim([0.1 1])
hist(FR2(:), 100);
xlim([0.1 1])
plot(a)
doc plot
plot(a)
hist(FR2(:), 100);
xlim([0.1 1])
plot(a)
plot(a(10:end,:))
length(a)
a(10:end, :)
plot(a(10:end,:))
plot(sum(a(10:end,:),2))
[a,b]=hist(m2, 100);
plot(a(10:end,:))
plot(sum(a(10:end,:),2))


%% figure de l'article
exp_data.day(15)
v=exp_data.from_type(4);
FR=[];figure, hold on, 
for i=v([2,6:19])
fr=exp_data.load_neuron(i);
fr2 = fr(21700:22887);
FR(i,:) = fr2;
end
vlist=FR(v([2,6:19]),:)
figure, hold on 
for i=1:length(vlist)-1
hist(vlist(i,:))
end
xlim([0.4 0.95])
ylim([0 350])

figure, hold on 
for i=1:size(vlist,1)
[n,x]=hist(vlist(i,:),0:0.01:1);
plot(x,n)
end
