cd workspace/planif_pfc/tools/matlab/
load ~/Dropbox/figs/neuron_data/normal/1_exp_data_spatial.mat
exp_data
exp_data.spatial_1
exp_data.spatial_1.active
v=exp_data.spatial_1.active;
N=exp_data.from_type(1);
N(v)
N
v=exp_data.spatial_2.active;
N=exp_data.from_type(2);
N
N(v)
v=v(1:19);
N(v)
exp_data.neuron(N(v))
exp_data.neuron(N(v)).fr
[exp_data.neuron(N(v)).fr]
exp_data.neuron(N(v)).fr
[exp_data.neuron(N(v)).fr]
M=[exp_data.neuron(N(v)).fr];
MS=sum(M);
MS=sum(M,2);
pv=exp_data.pv_space;
pv(pv>0)=MS;
imagesc(pv)
v
N(v)
M=[exp_data.neuron(N(v(8:19)).fr];
M=[exp_data.neuron(N(v(8:19))).fr];
MS=sum(M,2);
pv=exp_data.pv_space;
pv(pv>0)=MS;
imagesc(pv)
M=[exp_data.neuron(N(v(10:19))).fr];
MS=sum(M,2);
pv=exp_data.pv_space;
pv(pv>0)=MS;
imagesc(pv)
imagesc(pv-min(mv(:)))
imagesc(pv-min(pv(:)))
min(pv(:))
min(pv(pv>0))
imagesc(pv-min(pv(pv>0)))
M=[exp_data.neuron(N(v(10:19))).fr];
MS=sum(M,2);
pv=exp_data.pv_space;
pv(pv>0)=MS;
imagesc(pv-min(pv(pv>0)))
exp_data.linear_graph_plot()
exp_data.linear_graph_plot(3)
exp_data.linear_graph()
exp_data.linear_graph_plot(3)
exp_data.linear_graph_plot(3,N(v))
L=exp_data.linear_graph_plot(3,N(v));
imagesc(L)
imagesc(L(L>0.3))
LL=L;
LL(LL<0.3)=0;
imagesc(LL)
imagesc(L)
LL=L(4:16,:);
imagesc(LL)
LLS=sum(LL,2);
LLS=sum(LL,1);
figure, plot(LLS)
figure, plot(LLS(10:end))
N=exp_data.from_type(1);
exp_data.linear_graph_plot(3,exp_data.from_type(1))
L=exp_data.linear_graph_plot(3,exp_data.from_type(1));
LL=L(12:30, 12:end);
imagesc(LL);
LLS=sum(LL,1);
figure, plot(LLS)
L=exp_data.linear_graph_plot(3,N(v));
LL=L(4:16,:);
imagesc(LL)
figure, plot(LLS(10:end))
figure, plot(smooth(LLS(10:end)))
imagesc(LL)
figure, plot(smooth(LLS(10:end)))
figure, plot(LLS(1:end))
mult=repmat(1:75,13,1);
sum(LL.*mult,2);
res=sum(LL.*mult,2)./sum(LL);
res=sum(LL.*mult,2)./sum(LL,2);
res
figure,imagesc(LL)
n1=LL(1,:);
sum(n1.*1:75)
sum(n1)
sum(n1.*1:75)/sum(n1)
n1.*1:75
n1
n1.*1:75
n1(end)*75
n1(:) .* 1:75
n1(:) * 1:75
n1(:) * [1:75]
n1(:) .* [1:75]
n1(:) .* [1:75]'
n1 .* [1:75]
sum(n1 .* [1:75])/sum(n1)
n1(n1<0.1)=0;
sum(n1 .* [1:75])/sum(n1)
LL2=LL;LL2(LL2<0.1)=0;res=sum(LL2.*mult,2)./sum(LL2,2);
res
figure,imagesc(LL)
length(find(res<37))
length(find(res>37))
LL2=LL;LL2(LL2<0.2)=0;res=sum(LL2.*mult,2)./sum(LL2,2);
length(find(res<37))/length(res)
LL2=LL;LL2(LL2<0.3)=0;res=sum(LL2.*mult,2)./sum(LL2,2);
length(find(res<37))/length(res)
res