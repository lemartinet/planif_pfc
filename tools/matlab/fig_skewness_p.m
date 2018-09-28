%load ../../data/data_raw/1/exp_data.mat

TYPE = 2;
PATH = 3;
%PATH = 2;
%PATH = 1;
MAX_FR = 0.8;
NB_NEURON = 3602;

% recherche des neurones qu'on veut afficher
% for i = exp_data.from_type(TYPE)
%     p(p>0) = exp_data.neuron(i).fr;
%     figure, imagesc(p);
% end
N = exp_data.from_type(TYPE);
fr = exp_data.linear_graph_plot(PATH, 1:NB_NEURON);

P = N([11:19,6]); % P3
%P=N([7,8,9,4,5,6]); %P2
%P=N([3,6]); %P1

for i = 1:length(P)
    Pidx(i) = find(exp_data.lin_graph.neurons{PATH} == P(i));
    Sidx(i) = find(exp_data.lin_graph.neurons{PATH} == P(i)-2);
    figure, hold on, plot(fr(Pidx(i), :));
    P3(i,:)=fr(Pidx(i), :);
    S3(i,:)=fr(Sidx(i), :);
%    P2(i,:)=fr(Pidx(i), :);
%    S2(i,:)=fr(Sidx(i), :);
%    P1(i,:)=fr(Pidx(i), :);
%    S1(i,:)=fr(Sidx(i), :);
    plot(fr(Sidx(i), :), 'r');
    v = axis; axis([v(1:2), 0, MAX_FR]);
end

% normalisation pour avoir des distrib de proba
% for i=1:size(P3,1)
%    P3(i, :) = P3(i, :) / sum(P3(i, :)); 
% end
% for i=1:size(P2,1)
%    P2(i, :) = P2(i, :) / sum(P2(i, :)); 
% end
% for i=1:size(P1,1)
%    P1(i, :) = P1(i, :) / sum(P1(i, :)); 
% end

% on complete les vecteurs trop courts par du bruit
% P3save=P3;
% P2save=P2;
% P1save=P1;
% len = size(P3,2);
% P2 = [P2, rand(size(P2,1),len-size(P2,2))*0.02+0.06];
% P1 = [P1, rand(size(P1,1),len-size(P1,2))*0.02+0.06];

% affiche tous les P sur un même graph
figure, hold on
plot(P3'); 
plot(P2'); 
plot(P1'); 

% affiche les P avec les S sur un même graph
figure, hold on
plot(P3');
plot(S3');
figure, hold on
plot(P2');
plot(S2');
figure, hold on
plot(P1');
plot(S1');

% on stocke ds syn la distance de chaque path
% et on la normalise (/max(d)) pour avoir des 
% data plus compatibles entre path
[a,b]=max(S3');
[c,d]=sort(b);
P3=P3(d,:);
syn = d/max(d); 
[a,b]=max(S2');
[c,d]=sort(b);
P2=P2(d,:);
syn = [syn, d/max(d)];
[a,b]=max(S1');
[c,d]=sort(b);
P1=P1(d,:);
syn = [syn, 1/3, 1];

% on genere des valeurs selon la distrib des ch recept
sk=[];
P=P3;
for i = 1:size(P,1)
    v=P(i,:);
    v=v(v>0.1);
    l = build_distrib(v);
    sk = [sk, skewness(l)];
end
P=P2;
for i = 1:size(P,1)
    v=P(i,:);
    v=v(v>0.1);
    v=v/sum(v);
    for i=1:length(v)
     repv(i)=sum(v(1:i));
    end
    l=[];
    for j=1:10000
        l=[l, min(find(repv-rand()>0))];
    end
    sk = [sk, skewness(l)];
end
P=P1;
for i = 1:size(P,1)
    v=P(i,:);
    v=v(v>0.1);
    v=v/sum(v);
    for i=1:length(v)
     repv(i)=sum(v(1:i));
    end
    l=[];
    for j=1:10000
        l=[l, min(find(repv-rand()>0))];
    end
    sk = [sk, skewness(l)];
end
figure,plot(syn, sk, '+');
corr2(syn, sk)
