function [] = build_Q(exp_name, Q_PATH)

load([Q_PATH, 'data_', exp_name, '.mat'], 'exp_data', 'C', 'tfrs');
N = 1:exp_data.nbneurons;
probav = exp_data.pv_space;
probav(127,27:97) = probav(128,27:97);
probav(50:126,27) = probav(50:126,26);
probav = probav / sum(probav(:));
idx = find(probav > 0);
exp_data.linear_graph();
LG1 = exp_data.lin_graph.fr{1};
O1 = exp_data.lin_graph.neurons{1};
LG2 = exp_data.lin_graph.fr{2};
O2 = exp_data.lin_graph.neurons{2};
LG3 = exp_data.lin_graph.fr{3};
O3 = exp_data.lin_graph.neurons{3};
Q = zeros(length(N), 13);
for i = 1:length(N)
    fr = C{i}; 
    mfr = sum(fr(:) .* probav(:));
    sfr = sqrt(sum(fr(:) .^ 2 .* probav(:)) - mfr .^ 2);
    meanFr = mean(fr(idx));
    stdFr = std(fr(idx));
    lks = kurtosis(fr(idx)) - 3;
    info = mutual_information(tfrs(i,:), probav(idx));
    infoEqui = mutual_information(tfrs(i,:), ones(length(idx),1) / length(idx));
    fr={};
    fr{1} = LG1(find(O1 == N(i)), :);
    fr{2} = LG2(find(O2 == N(i)), :);
    fr{3} = LG3(find(O3 == N(i)), :);
    [a,b] = max([max(fr{1}), max(fr{2}), max(fr{3})]);
    sk = skewness(build_distrib(fr{b}));
    l = cut_field(fr{b});
    skcut = skewness(build_distrib(l));
    fsize = length(l);
    map.rate = C{i};
    map.time = probav;
    statInfo = SpatialInfo(map, meanFr);
    spatialInfo = statInfo.info;
    sparsity = statInfo.sparsity;
    specificity = 1/sparsity;
    Q(i, :) = [ fsize, mfr, sfr, meanFr, stdFr, lks, info.info, ...
        infoEqui.info, spatialInfo, sparsity, specificity, sk, skcut ];
end
save([Q_PATH, 'Q_', exp_name, '.mat'], 'Q');

end