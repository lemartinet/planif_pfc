function fig_propagation()

%load /media/storage/martinet/huge_pop2/data/data_raw/1/exp_data.mat

c = exp_data.get_centers(exp_data.from_type(0));
c = c(exp_data.spatial_0.pop1, :);
figure, plot(c(:,1), c(:,2), 'o');
figure, xlim([0 140]); ylim([0 140]);
list = [44 : length(exp_data.spatial_0.pop1), 19 : 23]; 
plot(c(list,1), c(list,2), 'o');

neurons = exp_data.neuron(exp_data.from_type(4));
neurons = neurons(exp_data.spatial_0.pop1);
neurons = neurons(list);
fr = [];
for i = 1 : length(neurons)
fr(i, :) = [mean(neurons(i).day(15).fr), std(neurons(i).day(15).fr)];
end

c2 = c(list, :);
d = sum((c2(2 : end, :) - c2(1 : end - 1, :)) .^ 2, 2);
d = [0; d];
for j = 1 : length(d)
    d2(j) = sum(d(1 : j));
end
d2 = abs(d2 - d2(end)); % on inverse l'ordre des distances

figure, plot(d2, fr(:,1), '+');
figure, plot(d2, fr(:,1), '+'), errorbar(d2, fr(:,1), fr(:,2))

%save ../../../figs/propagation/propagation_pop2.mat

end