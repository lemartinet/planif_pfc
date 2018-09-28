cd ~/workspace/planif_pfc/tools/matlab/
load ~/Dropbox/figs/neuron_data/figures/num.mat
load ~/Dropbox/figs/neuron_data/huge/exp_data.mat
exp_data.path = '~/Dropbox/figs/neuron_data/karim/';

probav = exp_data.pv_space;
save ~/Dropbox/figs/neuron_data/karim/probav.mat probav

C2_s = {};
C2_v = {};
C2_p = {};
j = 1;
for i=N_C2
    p = probav;
    p(p>0) = exp_data.neuron(i).fr;
    C2_s{j} = p;
    p = probav;
    p(p>0) = exp_data.neuron(i+1).fr;
    C2_v{j} = p;
    p = probav;
    p(p>0) = exp_data.neuron(i+2).fr;
    C2_p{j} = p;
    j = j + 1;
end
NC2_s = N_C2;
NC2_v = NC2_s + 1;
NC2_p = NC2_s + 2;
save ~/Dropbox/figs/neuron_data/karim/C2.mat C2_s C2_v C2_p NC2_s NC2_v NC2_p;

C1_s = {};
C1_v = {};
C1_p = {};
j = 1;
for i=N_C1(44:4:83)
    p = probav;
    p(p>0) = exp_data.neuron(i).fr;
    C1_s{j} = p;
    p = probav;
    p(p>0) = exp_data.neuron(i+1).fr;
    C1_v{j} = p;
    p = probav;
    p(p>0) = exp_data.neuron(i+2).fr;
    C1_p{j} = p;
    j = j + 1;
end
NC1_s = N_C1(44:4:83);
NC1_v = NC1_s + 1;
NC1_p = NC1_s + 2;
save ~/Dropbox/figs/neuron_data/karim/C1.mat C1_s C1_v C1_p NC1_s NC1_v NC1_p;

% calcul du field
N=1:10;
idx = find(probav > 0);
exp_data.linear_graph();
LG1 = exp_data.lin_graph.fr{1};
O1 = exp_data.lin_graph.neurons{1};
LG2 = exp_data.lin_graph.fr{2};
O2 = exp_data.lin_graph.neurons{2};
LG3 = exp_data.lin_graph.fr{3};
O3 = exp_data.lin_graph.neurons{3};
for i=N
   %[center, fsize, mfr_s1(i), sfr_s1(i)] = field_properties (C1_s{i}, probav);
   %center_s1(i,:) = center;
   %fsize_s1(i) = max(fsize);
   %lks_s1(i) = sum(sum(probav .* ((C1_s{i} - mfr_s1(i)) ./ sfr_s1(i)) .^ 4)) - 3;
   fr = C1_s{i}; 
   mfr_s1(i) = mean(fr(idx));
   sfr_s1(i) = std(fr(idx));
   lkse_s1(i) = kurtosis(fr(idx)) - 3;
   fr = exp_data.load_neuron(NC1_s(i));
   tfr = firing_rate(fr, exp_data.v_space, size(exp_data.pv_space), true); clear fr;
   %info_s1(i) = mutual_information(tfr(idx), probav(idx));
   infoe_s1(i) = mutual_information(tfr(idx), ones(length(idx),1) / length(idx));
   fr={};
   fr{1} = LG1(find(O1 == NC1_s(i)), :);
   fr{2} = LG2(find(O2 == NC1_s(i)), :);
   fr{3} = LG3(find(O3 == NC1_s(i)), :);
   [a,b]=max([max(fr{1}), max(fr{2}), max(fr{3})]);
   l = cut_field(fr{b});
   sk_s1(i) = skewness(build_distrib(l));
   [a,c] = max(fr{b});
   skd_s1(i) = sk_s1(i) / b;
   fsize_s1(i) = length(l);
   
   %[center, fsize, mfr_v1(i), sfr_v1(i)] = field_properties (C1_v{i}, probav);
   %center_v1(i,:) = center;
   %fsize_v1(i) = max(fsize);
   %lks_v1(i) = sum(sum(probav .* ((C1_v{i} - mfr_v1(i)) ./ sfr_v1(i)) .^ 4)) - 3;
   fr = C1_v{i}; 
   mfr_v1(i) = mean(fr(idx));
   sfr_v1(i) = std(fr(idx));
   lkse_v1(i) = kurtosis(fr(idx)) - 3;
   fr = exp_data.load_neuron(NC1_v(i));
   tfr = firing_rate(fr, exp_data.v_space, size(exp_data.pv_space), true); clear fr;
   %info_v1(i) = mutual_information(tfr(idx), probav(idx));
   infoe_v1(i) = mutual_information(tfr(idx), ones(length(idx),1) / length(idx));
   fr={};
   fr{1} = LG1(find(O1 == NC1_v(i)), :);
   fr{2} = LG2(find(O2 == NC1_v(i)), :);
   fr{3} = LG3(find(O3 == NC1_v(i)), :);
   [a,b]=max([max(fr{1}), max(fr{2}), max(fr{3})]);
   l = cut_field(fr{b});
   sk_v1(i) = skewness(build_distrib(l));
   [a,c] = max(fr{b});
   skd_v1(i) = sk_v1(i) / b;
   fsize_v1(i) = length(l);
   
   %[center, fsize, mfr_p1(i), sfr_p1(i)] = field_properties (C1_p{i}, probav);
   %center_p1(i,:) = center;
   %fsize_p1(i) = max(fsize);
   %lks_p1(i) = sum(sum(probav .* ((C1_p{i} - mfr_p1(i)) ./ sfr_p1(i)) .^ 4)) - 3;
   fr = C1_p{i}; 
   mfr_p1(i) = mean(fr(idx));
   sfr_p1(i) = std(fr(idx));
   lkse_p1(i) = kurtosis(fr(idx)) - 3;
   fr = exp_data.load_neuron(NC1_p(i));
   tfr = firing_rate(fr, exp_data.v_space, size(exp_data.pv_space), true); clear fr;
   %info_p1(i) = mutual_information(tfr(idx), probav(idx));
   infoe_p1(i) = mutual_information(tfr(idx), ones(length(idx),1) / length(idx));
   fr={};
   fr{1} = LG1(find(O1 == NC1_p(i)), :);
   fr{2} = LG2(find(O2 == NC1_p(i)), :);
   fr{3} = LG3(find(O3 == NC1_p(i)), :);
   [a,b]=max([max(fr{1}), max(fr{2}), max(fr{3})]);
   l = cut_field(fr{b});
   sk_p1(i) = skewness(build_distrib(l));
   [a,c] = max(fr{b});
   skd_p1(i) = sk_p1(i) / b;
   fsize_p1(i) = length(l);
   
   %[center, fsize, mfr_s2(i), sfr_s2(i)] = field_properties (C2_s{i}, probav);
   %center_s2(i,:) = center;
   %fsize_s2(i) = max(fsize);
   %lks_s2(i) = sum(sum(probav .* ((C2_s{i} - mfr_s2(i)) ./ sfr_s2(i)) .^ 4)) - 3;
   fr = C2_s{i}; 
   mfr_s2(i) = mean(fr(idx));
   sfr_s2(i) = std(fr(idx));
   lkse_s2(i) = kurtosis(fr(idx)) - 3;
   fr = exp_data.load_neuron(NC2_s(i));
   tfr = firing_rate(fr, exp_data.v_space, size(exp_data.pv_space), true); clear fr;
   %info_s2(i) = mutual_information(tfr(idx), probav(idx));
   infoe_s2(i) = mutual_information(tfr(idx), ones(length(idx),1) / length(idx));
   fr={};
   fr{1} = LG1(find(O1 == NC2_s(i)), :);
   fr{2} = LG2(find(O2 == NC2_s(i)), :);
   fr{3} = LG3(find(O3 == NC2_s(i)), :);
   [a,b]=max([max(fr{1}), max(fr{2}), max(fr{3})]);
   l = cut_field(fr{b});
   sk_s2(i) = skewness(build_distrib(l));
   [a,c] = max(fr{b});
   skd_s2(i) = sk_s2(i) / b;
   fsize_s2(i) = length(l);
   
   %[center, fsize, mfr_v2(i), sfr_v2(i)] = field_properties (C2_v{i}, probav);
   %center_v2(i,:) = center;
   %fsize_v2(i) = max(fsize);
   %lks_v2(i) = sum(sum(probav .* ((C2_v{i} - mfr_v2(i)) ./ sfr_v2(i)) .^ 4)) - 3;
   fr = C2_v{i}; 
   mfr_v2(i) = mean(fr(idx));
   sfr_v2(i) = std(fr(idx));
   lkse_v2(i) = kurtosis(fr(idx)) - 3;
   fr = exp_data.load_neuron(NC2_v(i));
   tfr = firing_rate(fr, exp_data.v_space, size(exp_data.pv_space), true); clear fr;
   %info_v2(i) = mutual_information(tfr(idx), probav(idx));
   infoe_v2(i) = mutual_information(tfr(idx), ones(length(idx),1) / length(idx));
   fr={};
   fr{1} = LG1(find(O1 == NC2_v(i)), :);
   fr{2} = LG2(find(O2 == NC2_v(i)), :);
   fr{3} = LG3(find(O3 == NC2_v(i)), :);
   [a,b]=max([max(fr{1}), max(fr{2}), max(fr{3})]);
   l = cut_field(fr{b});
   sk_v2(i) = skewness(build_distrib(l));
   [a,c] = max(fr{b});
   skd_v2(i) = sk_v2(i) / b;
   fsize_v2(i) = length(l);
   
   %[center, fsize, mfr_p2(i), sfr_p2(i)] = field_properties (C2_p{i}, probav);
   %center_p2(i,:) = center;
   %fsize_p2(i) = max(fsize);   
   %lks_p2(i) = sum(sum(probav .* ((C2_p{i} - mfr_p2(i)) ./ sfr_p2(i)) .^ 4)) - 3;
   fr = C2_p{i};
   mfr_p2(i) = mean(fr(idx));
   sfr_p2(i) = std(fr(idx));
   lkse_p2(i) = kurtosis(fr(idx)) - 3;
   fr = exp_data.load_neuron(NC2_p(i));
   tfr = firing_rate(fr, exp_data.v_space, size(exp_data.pv_space), true); clear fr;
   %info_p2(i) = mutual_information(tfr(idx), probav(idx));
   infoe_p2(i) = mutual_information(tfr(idx), ones(length(idx),1) / length(idx));
   fr={};
   fr{1} = LG1(find(O1 == NC2_p(i)), :);
   fr{2} = LG2(find(O2 == NC2_p(i)), :);
   fr{3} = LG3(find(O3 == NC2_p(i)), :);
   [a,b]=max([max(fr{1}), max(fr{2}), max(fr{3})]);
   l = cut_field(fr{b});
   sk_p2(i) = skewness(build_distrib(l));
   [a,c] = max(fr{b});
   skd_p2(i) = sk_p2(i) / b;  
   fsize_p2(i) = length(l);
end

% figure, hold on
% plot(fsize_s1, '+r');
% plot(fsize_v1, '+g');
% plot(fsize_p1, '+b');
% plot(fsize_s2, '.r');
% plot(fsize_v2, '.g');
% plot(fsize_p2, '.b');
col = ones(1,10);
figure, hold on
plot(col, fsize_s1, '+r');
plot(2*col, fsize_v1, '+g');
plot(3*col, fsize_p1, '+b');
plot(4*col, fsize_s2, '.r');
plot(5*col, fsize_v2, '.g');
plot(6*col, fsize_p2, '.b');

figure, hold on
plot(col, mfr_s1, '+r');
plot(2*col, mfr_v1, '+g');
plot(3*col, mfr_p1, '+b');
plot(4*col, mfr_s2, '.r');
plot(5*col, mfr_v2, '.g');
plot(6*col, mfr_p2, '.b');

figure, hold on
plot(col, sfr_s1, '+r');
plot(2*col, sfr_v1, '+g');
plot(3*col, sfr_p1, '+b');
plot(4*col, sfr_s2, '.r');
plot(5*col, sfr_v2, '.g');
plot(6*col, sfr_p2, '.b');

% figure, hold on
% plot(col, lks_s1, '+r');
% plot(2*col, lks_v1, '+g');
% plot(3*col, lks_p1, '+b');
% plot(4*col, lks_s2, '.r');
% plot(5*col, lks_v2, '.g');
% plot(6*col, lks_p2, '.b');

figure, hold on
plot(col, lkse_s1, '+r');
plot(2*col, lkse_v1, '+g');
plot(3*col, lkse_p1, '+b');
plot(4*col, lkse_s2, '.r');
plot(5*col, lkse_v2, '.g');
plot(6*col, lkse_p2, '.b');

% figure, hold on
% plot(col, [info_s1.info], '+r');
% plot(2*col, [info_v1.info], '+g');
% plot(3*col, [info_p1.info], '+b');
% plot(4*col, [info_s2.info], '.r');
% plot(5*col, [info_v2.info], '.g');
% plot(6*col, [info_p2.info], '.b');

figure, hold on
plot(col, [infoe_s1.info], '+r');
plot(2*col, [infoe_v1.info], '+g');
plot(3*col, [infoe_p1.info], '+b');
plot(4*col, [infoe_s2.info], '.r');
plot(5*col, [infoe_v2.info], '.g');
plot(6*col, [infoe_p2.info], '.b');

figure, hold on
plot(col, sk_s1, '+r');
plot(2*col, sk_v1, '+g');
plot(3*col, sk_p1, '+b');
plot(4*col, sk_s2, '.r');
plot(5*col, sk_v2, '.g');
plot(6*col, sk_p2, '.b');

figure, hold on
plot(col, skd_s1, '+r');
plot(2*col, skd_v1, '+g');
plot(3*col, skd_p1, '+b');
plot(4*col, skd_s2, '.r');
plot(5*col, skd_v2, '.g');
plot(6*col, skd_p2, '.b');

Q = [
    fsize_s1', mfr_s1', sfr_s1', lkse_s1', [infoe_s1.info]', sk_s1', skd_s1';
    fsize_v1', mfr_v1', sfr_v1', lkse_v1', [infoe_v1.info]', sk_v1', skd_v1';
    fsize_p1', mfr_p1', sfr_p1', lkse_p1', [infoe_p1.info]', sk_p1', skd_p1';
    fsize_s2', mfr_s2', sfr_s2', lkse_s2', [infoe_s2.info]', sk_s2', skd_s2';
    fsize_v2', mfr_v2', sfr_v2', lkse_v2', [infoe_v2.info]', sk_v2', skd_v2';
    fsize_p2', mfr_p2', sfr_p2', lkse_p2', [infoe_p2.info]', sk_p2', skd_p2';
    ];
save  ~/Dropbox/figs/neuron_data/karim/Q.mat Q;
v=1:6;
%v=[1,2,3,5,7,8];
Q3=zscore(Q(:,v));
[coeff, score, latent]=princomp(Q3);
figure, hold on;
plot(score(1:10,1), score(1:10,2),'r+')
plot(score(11:20,1), score(11:20,2),'g+')
plot(score(21:30,1), score(21:30,2),'b+')
plot(score(31:40,1), score(31:40,2),'r.')
plot(score(41:50,1), score(41:50,2),'g.')
plot(score(51:60,1), score(51:60,2),'b.')
figure, hold on;
plot(score(1:10,1), score(1:10,3),'r+')
plot(score(11:20,1), score(11:20,3),'g+')
plot(score(21:30,1), score(21:30,3),'b+')
plot(score(31:40,1), score(31:40,3),'r.')
plot(score(41:50,1), score(41:50,3),'g.')
plot(score(51:60,1), score(51:60,3),'b.')
figure, hold on;
plot(score(1:10,3), score(1:10,2),'r+')
plot(score(11:20,3), score(11:20,2),'g+')
plot(score(21:30,3), score(21:30,2),'b+')
plot(score(31:40,3), score(31:40,2),'r.')
plot(score(41:50,3), score(41:50,2),'g.')
plot(score(51:60,3), score(51:60,2),'b.')

figure, hold on;
plot3(score(1:10,1), score(1:10,2), score(1:10,3),'r+')
plot3(score(11:20,1), score(11:20,2), score(11:20,3),'g+')
plot3(score(21:30,1), score(21:30,2), score(21:30,3),'b+')
plot3(score(31:40,1), score(31:40,2), score(31:40,3),'r.')
plot3(score(41:50,1), score(41:50,2), score(41:50,3),'g.')
plot3(score(51:60,1), score(51:60,2), score(51:60,3),'b.')


Q = [
    fsize_s1', mfr_s1', sfr_s1', sfr_s1' ./  mfr_s1', lkse_s1', [infoe_s1.info]', sk_s1', skd_s1';
    fsize_p1', mfr_p1', sfr_p1', sfr_p1' ./  mfr_p1', lkse_p1', [infoe_p1.info]', sk_p1', skd_p1';
    fsize_s2', mfr_s2', sfr_s2', sfr_s2' ./  mfr_s2', lkse_s2', [infoe_s2.info]', sk_s2', skd_s2';
    fsize_p2', mfr_p2', sfr_p2', sfr_p2' ./  mfr_p2', lkse_p2', [infoe_p2.info]', sk_p2', skd_p2';
    ];
v=1:8;
%v=[2,3,9];
Q3=zscore(Q(:,v));
[coeff, score, latent]=princomp(Q3);
figure, hold on;
plot(score(1:10,1), score(1:10,2),'r+')
plot(score(11:20,1), score(11:20,2),'b+')
%plot(score(21:30,1), score(21:30,2),'r.')
%plot(score(31:40,1), score(31:40,2),'b.')
