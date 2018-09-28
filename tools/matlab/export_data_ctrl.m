%% Initializations
clear all
Q_PATH = '~/Dropbox/figs/karim/pca/data/';
load([Q_PATH, 'QExp.mat']);
max_rate = max(Qexp(:,4)); % 1
controls = {'randomUniform', 'randomPoissonRnd'};
% controls = {'randomUniform', 'randomPoissonExp', 'randomPoissonExpNorm', 'randomPoissonRnd', 'randomPoissonSim'};
NB_NEURON = 600;
probav = exp_data.pv_space;
oldprobav = probav;
probav(127,27:97) = probav(128,27:97);
probav(50:126,27) = probav(50:126,26);
probav = probav / sum(probav(:));


%% Simulated data
load('~/Dropbox/figs/neuron_data/huge/exp_data.mat');
exp_dataSim = exp_data;
load([Q_PATH, 'empty_exp_data.mat']);
load([Q_PATH, 'CategorieNeuron.mat']);
exp_data.nbneurons = length(N);
idx = find(probav > 0);
tfrs = cell(exp_data.nbneurons, length(idx));
C = cell(1, exp_data.nbneurons);
for i = 1:exp_data.nbneurons
    pbar.update(i / exp_data.nbneurons);
    p = oldprobav;
    p(p > 0) = exp_dataSim.neuron(N(i)).fr * max_rate;
    p(127,27:97) = p(128,27:97);
    p(50:126,27) = p(50:126,26);
    C{i} = p;
    exp_data.neuron(i).fr = p(oldprobav > 0);
    
    fr = exp_dataSim.load_neuron(N(i));
    tfr = firing_rate(fr, exp_dataSim.v_space, size(exp_dataSim.pv_space), true);
    tfr(127,27:97) = tfr(128,27:97);
    tfr(50:126,27) = tfr(50:126,26);
    tfrs(i,:) = tfr(idx) * max_rate; clear tfr fr;
end
save([Q_PATH, 'data_sim.mat'], 'exp_data', 'C', 'tfrs');
clear exp_dataSim tfrs C exp_data;



%% Generates random neurons
load([Q_PATH, 'empty_exp_data.mat']);
pbar = ProgressBar();
for i = 1:length(controls)
    if strcmp(controls{i}, 'randomPoissonSim')
        % poisson population based on simulated neurons
        load([Q_PATH, 'Q.mat']);
    elseif strcmp(controls{i}, 'randomPoissonExp')
        % poisson population based on data
        load([Q_PATH, 'QExp.mat']);
        Q = Qexp; clear Qexp;
    elseif strcmp(controls{i}, 'randomPoissonExpNorm')
        % normalized means
        load([Q_PATH, 'QExp.mat']);
        Q = Qexp; clear Qexp;
        Q(:,4) = Q(:,4) ./ max(Q(:,4)); 
    elseif strcmp(controls{i}, 'randomPoissonRnd')
        % poisson population based on random means
        Q = rand(NB_NEURON) * max_rate;
    end

    exp_data.nbneurons = NB_NEURON;
    idx = find(probav > 0);
    tfrs = cell(exp_data.nbneurons, length(idx));
    C = cell(1, exp_data.nbneurons);
    for j = 1:exp_data.nbneurons
        pbar.update(((i - 1) * exp_data.nbneurons + j) / (exp_data.nbneurons * length(controls)));
        if strcmp(controls{i}, 'randomNormal')
            % normal population 
            m = rand([length(idx), 100]) * max_rate;
        else
            m = poissrnd(Q(j,4), length(idx), 100);
        end
        tfrs(j,:) = mat2cell(m, ones(1, length(idx)), 100);
        ID = zeros(size(probav));
        ID(idx) = mean(m, 2);
        ID(isnan(ID)) = 0;
        C{j} = ID;
        exp_data.neuron(j).fr = ID(oldprobav > 0);
    end
    save([Q_PATH, 'data_', controls{i}, '.mat'], 'exp_data', 'C', 'tfrs');
    exp_data.nbneurons = [];
    exp_data.neuron = [];
    clear Q idx tfrs C;
end



%% Q for all the populations
pop_q = {'sim', controls};
pbar = ProgressBar();
for j = 1:length(pop_q)
    pbar.update(j / length(controls));
    build_Q(pop_q{j}, Q_PATH);
end



%% Plots
v = 1:13;
%v = [1,2,3,5,7,8];
QZ = Q;
QZ(isnan(QZ)) = 0;
QZ = zscore(QZ(:,v));
[coeff, score, latent] = princomp(QZ);
%save  ~/Dropbox/share/karim/neuron_data/random/PCA_random.mat QZ v coeff score latent;
save  ~/Dropbox/share/karim/neuron_data/random/PCA_randompoisson.mat QZ v coeff score latent;

styl = {'r+' 'm+' 'g+' 'k+' 'b+' 'r.' 'm.' 'g.' 'k.' 'b.'};
numb = {1:400};
figure, hold on;
title('PCA');
xlabel('C1'); ylabel('C2'); zlabel('C3')
for i =  1:length(numb)
    plot3(score(numb{i},1), score(numb{i},2), score(numb{i},3), styl{i});
end
%saveas(gcf, sprintf('%s/random/pca_random.fig', Q_PATH), 'fig');
saveas(gcf, sprintf('%s/random/pca_randompoisson.fig', Q_PATH), 'fig');

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
%saveas(gcf, sprintf('%s/random/pca_subplot_random.pdf', Q_PATH), 'pdf');
saveas(gcf, sprintf('%s/random/pca_subplot_randompoisson.pdf', Q_PATH), 'pdf');

QZR = QZ;
load ~/Dropbox/share/karim/neuron_data/PCA.mat;
openfig('/home/martinet/Dropbox/share/karim/neuron_data/pca.fig')
scoreR = QZR(:,v) * coeff;
hold on;
plot3(scoreR(:,1), scoreR(:,2), scoreR(:,3), 'ys');
%saveas(gcf, sprintf('%s/random/pca_random_mycoeff.fig', Q_PATH), 'fig');
saveas(gcf, sprintf('%s/random/pca_randompoisson_mycoeff_withmodel.fig', Q_PATH), 'fig');

QR = Q;
load ~/Dropbox/share/karim/neuron_data/Q.mat;
load ~/Dropbox/share/karim/neuron_data/C.mat
QM = [Q;QR];
v = [4,5,6,8,9,10,13];
QZ = QM;
QZ(isnan(QZ)) = 0;
QZ = zscore(QZ(:,v));
[coeff, score, latent] = princomp(QZ);
save  ~/Dropbox/share/karim/neuron_data/random/PCA_randompoisson_withmodel.mat QZ v coeff score latent;

styl = {'r+' 'm+' 'g+' 'k+' 'b+' 'r.' 'm.' 'g.' 'k.' 'b.' 'ys'};
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
numb = {find(S1) find(D1) find(P1) find(Q1) find(V1) find(S2) ...
    find(D2) find(P2) find(Q2) find(V2) length(Q)+[1:length(QR)]};
figure, hold on;
title('PCA');
xlabel('C1'); ylabel('C2'); zlabel('C3')
for i =  1:length(numb)
    plot3(score(numb{i},1), score(numb{i},2), score(numb{i},3), styl{i});
end
saveas(gcf, sprintf('%s/random/pca_randompoisson_mycoeff_withmodel_commonQZ.fig', Q_PATH), 'fig');

M(isnan(M))=0;
for w=1:13
figure; hold on;
for j=1:length(numb)
bar(j, mean(M(numb{j},w)));
errorbar(j, mean(M(numb{j},w)), std(M(numb{j},w))/sqrt(length(M(numb{j},w))));
end
title(names{w});
end
