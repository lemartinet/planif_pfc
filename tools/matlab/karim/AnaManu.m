pwd
cd /media/disk/newJune09
load Q
figure, imagesc(Q)
figure, imagesc(Q), axis xy
caxis([0 0.2])
Q=Q';
C = corrcoef(Q(:,1:5000));
C(isnan(C))=0;
C = corrcoef(Q);
C(isnan(C))=0;
[V,L] = pcacov(C);
figure, plot(L,'-o')
figure, plot(log(L),'-o')
figure, plot(V(:,1),'-o')
figure, plot(V(:,2),'-ko')
figure, plot(V(:,3),'-ro')
figure('Color',[1,1,1])
subplot(3,3,1)
plot(V(:,1),'-o')
subplot(3,3,2)
plot(V(:,2),'-ro')
subplot(3,3,3)
plot(V(:,2),'-ko')
subplot(3,3,4)
plot(V(:,4),'-co')
subplot(3,3,5)
plot(V(:,5),'-mo')
subplot(3,3,6)
plot(V(:,6),'-ko')
subplot(3,3,7)
plot(V(:,7),'-o')
subplot(3,3,8)
plot(V(:,8),'-o')
subplot(3,3,9)
plot(V(:,9),'-ko')
subplot(3,3,3)
plot(V(:,3),'-ko')
for i=1:nbBins
pv = Q(i,:);
r(i) = pv*proj*pv';
end
nbBins = size(Q,1);
r = zeros(nbBins,1);
pc1 = V(:,2);
proj = pc1*pc1';
for i=1:nbBins
pv = Q(i,:);
r(i) = pv*proj*pv';
end
figure, plot(r)
figure, imagesc(Q'), axis xy
figure('Color',[1,1,1])
subplot(3,1,[1,2])
imagesc(Q'),axis xy
subplot(3,1,3)
plot(r)
plot(r,'k')
figure('Color',[1,1,1])
imagesc(Q(find(abs(pc1)>1;1),:)'),axis xy
imagesc(Q(find(abs(pc1)>1),:)'),axis xy
figure('Color',[1,1,1])
imagesc(Q(find(abs(pc1)>1),:)'),axis xy
imagesc(Q(:,find(abs(pc1)>1))'),axis xy
size(Q)
figure, imagesc(Q(:,find(abs(pc1)>1))'),axis xy
figure, hist(pc1,50)
figure, hist(log(pc1),50)
length(find(abs(pc1)>1))
figure, imagesc(Q(:,find(abs(pc1)>0.1))'),axis xy
figure('Color',[1,1,1])
subplot(3,1,[1,2])
imagesc(Q(:,find(abs(pc1)>0.1))'),axis xy
subplot(3,1,3)
plot(r,'k')
[A,id]=sort(pc1);
figure('Color',[1,1,1])
imagesc(Q(:,id)'),axis xy
caxis([0 0.2])
figure('Color',[1,1,1])
imagesc(Q(:,id)'),axis xy
caxis([0 0.2])
imagesc(Q(:,find(abs(pc1)>0.05))'),axis xy
imagesc(Q(:,find(abs(pc1)>0.01))'),axis xy
caxis([0 0.2])
figure, hist(abs(pc1),100)
length(find(abs(pc1)>0.01))
length(find(abs(pc1)>0.1))
(find(abs(pc1)>0.1))
(find(abs(pc1)>0.1))'
Pos=load('/media/disk/newJune09/vars.txt');
size(Pos)
size(Q,1)
X=Pos(end-size(Q,1):end,4);
Y=Pos(end-size(Q,1):end,4);
(find(abs(pc1)>0.1))'
size(Q)
size(X)
X=Pos(end+1-size(Q,1):end,4);
Y=Pos(end+1-size(Q,1):end,4);
Y=Pos(end+1-size(Q,1):end,5);
pc2=pc1;
proj2=proj;
save Data Q C V L X Y r nbBins pc2 proj2
id2=id;
save Data Q C V L X Y r nbBins pc2 proj2 id2
binnedR=Bin((Tsa1t),[min(Tsa1t) max(Tsa1t)],siz);
binnedC=Bin((Tsa2t),[min(Tsa2t) max(Tsa2t)],siz);
accumulated = Accumulate([binnedC binnedR]);
bx=Bin((X),[min(X) max(X)],50);
by=Bin((Y),[min(Y) max(Y)],50);
accumulated = Accumulate([bx by]);
meanMap([[1:length(Q)],X,Y],Q(1,:),'nbins',[50, 50],'smooth',1);
size(X)
meanMap([[1:length(Q)]',X,Y],Q(1,:),'nbins',[50, 50],'smooth',1);
MeanMap([[1:length(Q)]',X,Y],Q(1,:),'nbins',[50, 50],'smooth',1);
edit Accumaulte
edit Accumulate
fmT = MeanMap([tp,X,Y],Data(Restrict(tsa,mazeEpoch)),'smooth',smoo);
figure, subplot(221),PlotColorMap(fmT.count,fmT.time, 'bar', 'on','cutoffs',[a, b])
title(['Total ',date(1:2),' ',sup,' Rule ',num2str(ru(1))])
tp=[1:length(Q)]';
Xn=X/max(X);
Yn=Y/max(Y);
fmT = MeanMap([tp,Xn,Yn],Q(1,:),'smooth',1);
size(Xn)
size(Q(1,:))
size(Q(:,1))
fmT = MeanMap([tp,Xn,Yn],Q(:,1),'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],Q(:,1),'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],Q(:,100),'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
figure, plot(X,Y)
figure, plot(Xn,Yn)
figure, plot(X,Y)
X=X+0.7;
Y=Y+0.7;
figure, plot(X,Y)
Y=Y-0.1;
X=X-0.1;
figure, plot(X,Y)
X=X+0.1;
Y=Y+0.1;
figure, plot(X,Y)
Xn=X/max(X);
Yn=Y/max(Y);
figure, plot(Xn,Yn)
X=X-0.1;
Y=Y-0.1;
Xn=X/max(X);
Yn=Y/max(Y);
figure, plot(Xn,Yn)
fmT = MeanMap([tp,Xn,Yn],Q(:,100),'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
hold on, plot(Xn,Yn)
figure, plot(Xn,Yn)
size(Xn)
Xn=[0, Xn, 1];
Xn=[0; Xn; 1];
Yn=[0; Yn; 1];
fmT = MeanMap([tp,Xn,Yn],Q(:,100),'smooth',1);
Yn=[0; Yn; 1]';
Xn=[0; Xn; 1]';
fmT = MeanMap([tp,Xn,Yn],Q(:,100),'smooth',1);
fmT = MeanMap([tp',Xn,Yn],Q(:,100),'smooth',1);
tp=[0; tp; tp(end)+1]';
fmT = MeanMap([tp,Xn,Yn],Q(:,100),'smooth',1);
tp=[1; tp; tp(end)+1]';
tp=[1:length(Q)]';
tp=[1; tp; tp(end)+1]';
fmT = MeanMap([tp,Xn,Yn],Q(:,100),'smooth',1);
fmT = MeanMap([tp',Xn,Yn],Q(:,100),'smooth',1);
size(tp)
size(Xn)
X=Pos(end+1-size(Q,1):end,4);
Y=Pos(end+1-size(Q,1):end,5);
figure, plot(X,Y)
X=X+0.6;
Y=Y+0.6;
figure, plot(X,Y)
Xn=X/max(X);
Yn=Y/max(Y);
size(Xn)
Xn=[Xn,0, 1];
Xn=[Xn;0; 1];
Yn=[Yn;0; 1];
size(tp)
tp=[tp; tp(end)+1; tp(end)+2]';
tp=[tp, tp(end)+1, tp(end)+2]';
size(tp)
size(Xn)
tp=[1:length(Q)]';
size(tp)
tp=[tp; tp(end)+1; tp(end)+2];
size(tp)
fmT = MeanMap([tp,Xn,Yn],Q(:,100),'smooth',1);
fmT = MeanMap([tp,Xn,Yn],[Q(:,100); mean(Q(:,100)); mean(Q(:,100))],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
%Xn=[Xn;0; 1];
%Yn=[Yn;0; 1];
Xn(end)=0;
fmT = MeanMap([tp,Xn,Yn],[Q(:,100); mean(Q(:,100)); mean(Q(:,100))],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
Xn(end)=1;
Yn(end)=0;
fmT = MeanMap([tp,Xn,Yn],[Q(:,100); mean(Q(:,100)); mean(Q(:,100))],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
figure, plot(Xn,Yn)
Y=Y+2.6;
figure, plot(X,Y)
Y=Y-1;
figure, plot(X,Y)
Y=Y-0.4;
figure, plot(X,Y)
Y=Y-0.2;
figure, plot(X,Y)
Xn=X/max(X);
Yn=Y/max(Y);
Xn=[Xn;0; 1];
Yn=[Yn;0; 0];
fmT = MeanMap([tp,Xn,Yn],[Q(:,100); mean(Q(:,100)); mean(Q(:,100))],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[Q(:,i); mean(Q(:,i)); mean(Q(:,i))],'smooth',1);
i=pc2(1);
fmT = MeanMap([tp,Xn,Yn],[Q(:,i); mean(Q(:,i)); mean(Q(:,i))],'smooth',1);
i=pc2(1)
i=id2(1)
fmT = MeanMap([tp,Xn,Yn],[Q(:,i); mean(Q(:,i)); mean(Q(:,i))],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
i=id2(2)
fmT = MeanMap([tp,Xn,Yn],[Q(:,i); mean(Q(:,i)); mean(Q(:,i))],'smooth',1);
i=id2(2)
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[r; mean(r); mean(r)],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[mean(Q(:,id(1:10))); mean(mean(Q(:,id(1:10)))); mean(mean(Q(:,id(1:10))))],'smooth',1);
size(mean(Q(:,id(1:10))))
fmT = MeanMap([tp,Xn,Yn],[mean(Q(:,id(1:10))')'; mean(mean(Q(:,id(1:10))')'); mean(mean(Q(:,id(1:10))')')],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[mean(Q(:,id(1:20))')'; mean(mean(Q(:,id(1:20))')'); mean(mean(Q(:,id(1:20))')')],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[mean(Q(:,id(1:50))')'; mean(mean(Q(:,id(1:50))')'); mean(mean(Q(:,id(1:50))')')],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[mean(Q(:,id(end-50:end))')'; mean(mean(Q(:,id(end-50:end))')'); mean(mean(Q(:,id(end-50:end))')')],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[mean(Q(:,id(end-5:end))')'; mean(mean(Q(:,id(end-5:end))')'); mean(mean(Q(:,id(end-5:end))')')],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[mean(Q(:,id(end-5:end))')'; 0;0],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[mean(Q(:,id(1:5))')'; 0;0],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
id(1:5)
id(end-5:end)
Typ=load('/media/disk/newJune09/type.txt');
size(Type)
size(Typ)
size(Q)
figure, imagesc(Q(:,250))
figure, imagesc(Q(:,250)')
figure, imagesc(Q(:,248:255)')
figure, imagesc(Q(:,240:255)')
figure, imagesc(Q(:,240:260)'), axis xy
figure, imagesc(Q(:,300:360)'), axis xy
figure, imagesc(Q(:,305:360)'), axis xy
figure, imagesc(Q(:,310:360)'), axis xy
figure, imagesc(Q(:,300:360)'), axis xy
figure, imagesc(Q(:,305:360)'), axis xy
figure, imagesc(Q(:,304:360)'), axis xy
figure, imagesc(Q(:,305:360)'), axis xy
Type(id(end-5:end),2)
Typ(id(end-5:end),2)
[id(end-5:end) Typ(id(end-5:end),2)]
[id(1:5) Typ(id(1:5),2)]
fmT = MeanMap([tp,Xn,Yn],[(Q(:,id(end-5))')'; 0;0],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[(Q(:,309)')'; 0;0],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[(Q(:,319)')'; 0;0],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[(Q(:,358)')'; 0;0],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[(Q(:,348)')'; 0;0],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[(Q(:,148)')'; 0;0],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
fmT = MeanMap([tp,Xn,Yn],[(Q(:,591)')'; 0;0],'smooth',1);
figure, PlotColorMap(fmT.count,fmT.time, 'bar', 'on')
map=PlotColorMap(fmT.count,fmT.time, 'bar', 'on');
map=PlotColorMapKB(fmT.count,fmT.time, 'bar', 'on');
map=PlotColorMapKB(fmT.count,fmT.time, 100,[50 50],'bar', 'on');
map=PlotColorMapKB(fmT.count, 100,[50 50],'bar', 'on');
map=PlotColorMapKB(fmT.count,fmT.time, 100,[1 1 1 1],'bar', 'on');
map=PlotColorMapKB(fmT.count,fmT.time, 100,[100 100 100 100],'bar', 'on');
edit PlotColorMap
map=PlotColorMap(fmT.count,fmT.time, 'bar', 'on');
figure, imagesc(map)
figure, imagesc(map), axis xy
caxis
caxis([0.7 1])
figure, imshow(map), axis xy
which imshow
figure, imagesc(map(:,:,1), axis xy
figure, imagesc(map(:,:,1)), axis xy
figure, imagesc(map(:,:,2)), axis xy
figure, imagesc(map(:,:,3)), axis xy
help image
MM=unit8(map);
MM=uint8(map);
figure, imagesc(map)
figure, imagesc(MM)
caxis([0.7 50])
MM=uint8(round(map*255));
figure, imagesc(MM)
size(MM)
figure, imagesc(MM(:,:,1))
MM=double(MM)
MM=double(MM);
figure, imagesc(MM(:,:,1))
figure, imagesc(MM)
MM= .2989*map(:,:,1)+.5870*map(:,:,2) +.1140*rgb_img(:,:,3);
MM= .2989*map(:,:,1)+.5870*map(:,:,2) +.1140*map(:,:,3);
min(MM(:))
max(MM(:))
figure, imagesc(MM)
colormap(gray)
colormap(gray(256))
MM= map(:,:,1)+map(:,:,2) +map(:,:,3);
figure, imagesc(MM)
figure, imagesc(map)
figure, imagesc(map(:,:,1))
figure, imagesc(map(:,:,2))
figure, imagesc(map(:,:,3))
MM= .7989*map(:,:,1)+.5870*map(:,:,2) +.1140*map(:,:,3);
figure, imagesc(MM)
MM= 1*map(:,:,1)+0.5*map(:,:,2) +0.1*map(:,:,3);
figure, imagesc(MM)
MM= 1*map(:,:,1)+0.2*map(:,:,2) +0.1*map(:,:,3);
figure, imagesc(MM)
figure, imagesc(map)
figure, imagesc(map(:,:,1))
figure, imagesc(map(:,:,2))
figure, imagesc(map)
figure, imagesc(map(:,:,3))
figure, imagesc(map)
map=PlotColorMap(fmT.count,fmT.time, 'bar', 'on');
MM= 0.35*map(:,:,1)+0.2*map(:,:,2) +0.05*map(:,:,3);
figure, imagesc(MM)
MM=max(map(:,:,1),map(:,:,2),map(:,:,1));
MM=max(map(:,:,1),map(:,:,2));
MM=max(MM,map(:,:,3));
size(MM)
figure, plot(MM)
figure, imagesc(MM)
MM=max(0.35*map(:,:,1),0.2*map(:,:,2));
MM=max(MM,0.05*map(:,:,3));
figure, imagesc(MM)
caxis([0.7 1])
caxis([0 1])
caxis([0 0.5])
figure, hist(map(:,:,1))
MM= 0.35*map(:,:,1)-0.2*map(:,:,2) -0.05*map(:,:,3);
figure, imagesc(MM)
figure, imagesc(MM+min(min(MM)))
figure, imagesc(MM+abs(min(min(MM))))
figure, imagesc(fmT.count)
figure, imagesc(fmT.rate)
fmt
fmT
figure, imagesc(fmT.time)
fmT = MeanMap([tp,Xn,Yn],[(Q(:,591)')'; 0;0],'smooth',1);
figure, imagesc(fmT.rate)
fmT = MeanMap([tp,Xn,Yn],[(Q(:,591)')'; 0;0],'smooth',2);
figure, imagesc(fmT.rate)
figure, imagesc(fmT.count)
figure, imagesc(fmT.time)
fmT = MeanMap([tp,Xn,Yn],[(Q(:,id(end))')'; 0;0],'smooth',2);
figure, imagesc(fmT.time)
figure, imagesc(fmT.count)
fmT = MeanMap([tp,Xn,Yn],[(Q(:,id(end-10))')'; 0;0],'smooth',2);
figure, imagesc(fmT.count)
Typ(id(end-10),2)
fmT = MeanMap([tp,Xn,Yn],[(Q(:,id(10))')'; 0;0],'smooth',2);
Typ(id(10),2)
figure, imagesc(fmT.count)
fmT = MeanMap([tp,Xn,Yn],[(Q(:,id(1))')'; 0;0],'smooth',2);
Typ(id(1),2)
figure, imagesc(fmT.count)






Weight=load('/media/disk/newJune09/weights/175.txt');
Weight=Weight(1:end-1,1:end-1);

Weight=Weight-diag(diag(Weight));
proj2=proj2-diag(diag(proj2));

c1=weight;
c2=proj2;
N = size(c1,1);

d = 0.5*(log(det(c2)/det(c1))+trace(inv(c2)*c1) - N);



