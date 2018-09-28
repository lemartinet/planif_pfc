cd ~/workspace/planif_pfc/tools/matlab/
load ~/Dropbox/figs/neuron_data/huge/exp_data.mat
exp_data.path = '~/Dropbox/share/karim/neuron_data/';

probav = exp_data.pv_space;
oldprobav = probav;
probav(127,27:97) = probav(128,27:97);
probav(50:126,27) = probav(50:126,26);
probav = probav / sum(probav(:));
save ~/Dropbox/share/karim/neuron_data/probav.mat probav

%% Number and maps
C1_col = exp_data.spatial_0.pop1';
NC1 = [];
for i = C1_col
    NC1 = [NC1, exp_data.from_col(i)];
    n = exp_data.minicols_of_col(i);
    for j = n
        NC1 = [NC1, exp_data.from_minicol(j)];
    end
end
% for k = NC1
%     fprintf('%d.mat ', k);
% end
NC1_s = NC1(ismember(NC1, exp_data.from_type(0)));
NC1_d = NC1(ismember(NC1, exp_data.from_type(1)));
NC1_p = NC1(ismember(NC1, exp_data.from_type(2)));
NC1_q = NC1(ismember(NC1, exp_data.from_type(3)));
NC1_v = NC1(ismember(NC1, exp_data.from_type(4)));

C2_col = exp_data.spatial_0.pop2';
NC2 = [];
for i = C2_col
    NC2 = [NC2, exp_data.from_col(i)];
    n = exp_data.minicols_of_col(i);
    for j = n
        NC2 = [NC2, exp_data.from_minicol(j)];
    end
end
% for k = NC2
%     fprintf('%d.mat ', k);
% end
NC2_s = NC2(ismember(NC2, exp_data.from_type(0)));
NC2_d = NC2(ismember(NC2, exp_data.from_type(1)));
NC2_p = NC2(ismember(NC2, exp_data.from_type(2)));
NC2_q = NC2(ismember(NC2, exp_data.from_type(3)));
NC2_v = NC2(ismember(NC2, exp_data.from_type(4)));

C = cell(1, length(NC1)+length(NC2));
N = [NC1, NC2];
for i = 1:length(N)
    p = oldprobav;
    p(p > 0) = exp_data.neuron(N(i)).fr;
    p(127,27:97) = p(128,27:97);
    p(50:126,27) = p(50:126,26);
    C{i} = p;
end
save ~/Dropbox/share/karim/neuron_data/C.mat C N NC1 NC1_s NC1_d NC1_p NC1_q NC1_v NC2 NC2_s NC2_d NC2_p NC2_q NC2_v;


%% vars for the PCA
build_Q('sim', Q_PATH);


%% Boxplots & PCA
S1 = ismember(N, NC1_s);
D1 = ismember(N, NC1_d);
P1 = ismember(N, NC1_p);
Q1 = ismember(N, NC1_q);
V1 = ismember(N, NC1_v);
S2 = ismember(N, NC2_s);
D2 = ismember(N, NC2_d);
P2 = ismember(N, NC2_p);
Q2 = ismember(N, NC2_q);
V2 = ismember(N, NC2_v);

ALL = S1 + 2*D1 + 3*P1 + 4*Q1 + 5*V1 + 6*S2 + 7*D2 + 8*P2 + 9*Q2 + 10*V2;
for i=1:length(names)
    figure, boxplot(Q(:,i), ALL, ...
        'labels', {'s1' 'd1' 'p1' 'q1' 'v1' 's2' 'd2' 'p2' 'q2' 'v2'});
    title(names{i})
    saveas(gcf, sprintf('%s/%s.pdf', exp_data.path, names{i}), 'pdf');
end


%v = 1:13;
%v = [4,5,6,8,9,10];
v = [4,5,6,8,9,10,13];
QZ = Q;
QZ(isnan(QZ)) = 0;
QZ = zscore(QZ(:,v));
[coeff, score, latent] = princomp(QZ);
save  ~/Dropbox/share/karim/neuron_data/PCA.mat QZ v coeff score latent;

styl = {'r+' 'm+' 'g+' 'k+' 'b+' 'r.' 'm.' 'g.' 'k.' 'b.'};
numb = {S1 D1 P1 Q1 V1 S2 D2 P2 Q2 V2};
figure, hold on;
title('PCA');
xlabel('C1'); ylabel('C2'); zlabel('C3')
for i =  1:length(numb)
    plot3(score(numb{i},1), score(numb{i},2), score(numb{i},3), styl{i});
end
saveas(gcf, sprintf('%s/pca.fig', exp_data.path), 'fig');

figure
subplot(1,3,1);
hold on;
xlabel('C1'); ylabel('C2');
for i =  1:length(numb)
    plot(score(numb{i},1), score(numb{i},2), styl{i});
end
subplot(1,3,2);
hold on;
xlabel('C1'); ylabel('C3');
for i =  1:length(numb)
    plot(score(numb{i},1), score(numb{i},3), styl{i});
end
subplot(1,3,3);
hold on;
xlabel('C2'); ylabel('C3');
for i =  1:length(numb)
    plot(score(numb{i},2), score(numb{i},3), styl{i});
end
saveas(gcf, sprintf('%s/pca_subplot.pdf', exp_data.path), 'pdf');



%% plot the pref frequency
%FR = [];
figure, hold on
N_v = [NC1_v, NC2_v];
for i = 1:length(N_v)
    fr = exp_data.load_neuron(N_v(i));
    %fr = fr(21700:22887);
    %FR(i,:) = fr;
    [n,x] = hist(fr, 0:0.001:1);
    plot(x,n)
end
xlim([0 0.999])
saveas(gcf, sprintf('%s/pref_freq_v.pdf', exp_data.path), 'pdf');


%% histo pr les pop spatiales
N_space = S1 | D1 | P1 | S2 | D2 | P2;
figure, hist(Q(N_space, 1));
saveas(gcf, sprintf('%s/hist_fsize_space.pdf', exp_data.path), 'pdf');
figure, hist(Q(N_space, 13));
saveas(gcf, sprintf('%s/hist_skew_space.pdf', exp_data.path), 'pdf');
