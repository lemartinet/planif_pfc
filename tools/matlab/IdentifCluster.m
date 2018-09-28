function [repart,list]=IdentifCluster(Q,nClu,cidx,listVar, save_path)

DATA_PATH = '~/Dropbox/figs/karim/pca/data/';
load([DATA_PATH, 'CategorieNeuron.mat']);

S1 = find(ismember(N, NC1_s));
D1 =  find(ismember(N, NC1_d));
P1 =  find(ismember(N, NC1_p));
Q1 =  find(ismember(N, NC1_q));
V1 =  find(ismember(N, NC1_v));
S2 =  find(ismember(N, NC2_s));
D2 =  find(ismember(N, NC2_d));
P2 =  find(ismember(N, NC2_p));
Q2 =  find(ismember(N, NC2_q));
V2 =  find(ismember(N, NC2_v));

valu=[];
    
    for i=S1
        varia2{i}='S 1';
        valu(i)=1;
    end
    for i=D1
        varia2{i}='D 1';
        valu(i)=2;
    end
    for i=P1
        varia2{i}='P 1';
        valu(i)=3;
    end
    for i=Q1
        varia2{i}='Q 1';
        valu(i)=4;
    end
    for i=V1
        varia2{i}='V 1';
        valu(i)=5;
    end

    for i=S2
        varia2{i}='S   2';
        valu(i)=6;
    end
    for i=D2
        varia2{i}='D   2';
        valu(i)=7;
    end
    for i=P2
        varia2{i}='P   2';
        valu(i)=8;
    end
    for i=Q2
        varia2{i}='Q   2';
        valu(i)=9;
    end
    for i=V2
        varia2{i}='V   2';
        valu(i)=10;
    end
    
    
    
%--------------------------------------------------------------------
%--------------------------------------------------------------------
%--------------------------------------------------------------------
 
try
    listVar;
    Mz=zscore(Q(:,listVar));
catch
    Mz=zscore(Q);
    listVar=[1:13];
end

disp(' ')

[coeff,sc,lat,tsq]=princomp(Mz);



% opts = statset('Display','final');
% [cidx, ctrs,sumS] = kmeansKB(sc, nClu, 'Distance','sqEuclidean','Replicates',5, 'Options',opts);

couleur{1}='k';
couleur{2}='r';
couleur{3}='b';
couleur{4}='g';
couleur{5}='c';
couleur{6}='m';

coul{1}=[0 0 0];
coul{2}=[1 0 0];
coul{3}=[0 0 1];
coul{4}=[0 1 0];
coul{5}=[0 0.8 1];
coul{6}=[1 1 0.5];

for i=1:nClu
listCluster(i)=length(find(cidx==i));
end

[BE,id]=sort(listCluster,'descend');


for i=1:nClu
    list{i}=find(cidx==id(i));
%     list{i}=cidx(i);
end

% for i=1:nClu
%     list{i}=find(cidx==i);
% end



%--------------------------------------------------------------------
%--------------------------------------------------------------------
%--------------------------------------------------------------------





%--------------------------------------------------------------------
%--------------------------------------------------------------------
%--------------------------------------------------------------------







k=1;

for m=1:nClu
    
repart(k,1)=length(find(ismember(S1,(list{m})))==1)/length(S1)*100;
repart(k,2)=length(find(ismember(D1,(list{m})))==1)/length(D1)*100;
repart(k,3)=length(find(ismember(P1,(list{m})))==1)/length(P1)*100;
repart(k,4)=length(find(ismember(Q1,(list{m})))==1)/length(Q1)*100;
repart(k,5)=length(find(ismember(V1,(list{m})))==1)/length(V1)*100;

repart(k,6)=length(find(ismember(S2,(list{m})))==1)/length(S2)*100;
repart(k,7)=length(find(ismember(D2,(list{m})))==1)/length(D2)*100;
repart(k,8)=length(find(ismember(P2,(list{m})))==1)/length(P2)*100;
repart(k,9)=length(find(ismember(Q2,(list{m})))==1)/length(Q2)*100;
repart(k,10)=length(find(ismember(V2,(list{m})))==1)/length(V2)*100;

repart2(k,1)=length(find(ismember(S1,(list{m})))==1);
repart2(k,2)=length(find(ismember(D1,(list{m})))==1);
repart2(k,3)=length(find(ismember(P1,(list{m})))==1);
repart2(k,4)=length(find(ismember(Q1,(list{m})))==1);
repart2(k,5)=length(find(ismember(V1,(list{m})))==1);

repart2(k,6)=length(find(ismember(S2,(list{m})))==1);
repart2(k,7)=length(find(ismember(D2,(list{m})))==1);
repart2(k,8)=length(find(ismember(P2,(list{m})))==1);
repart2(k,9)=length(find(ismember(Q2,(list{m})))==1);
repart2(k,10)=length(find(ismember(V2,(list{m})))==1);


k=k+1;
end




%--------------------------------------------------------------------
%--------------------------------------------------------------------
%--------------------------------------------------------------------    




figure('Color',[1 1 1]) 
subplot(2,1,1)
bar(repart','stack'),caxis([1, 6])
xlabel('         S1      D1       P1       Q1       V1       S2       D2       P2       Q2       V2                    ')
% ylabel('bleu/black, green/green, red/bleu')
subplot(2,1,2)
repart2
bar(repart2','stacked'),caxis([1, 6])
xlabel('         S1      D1       P1       Q1       V1       S2       D2       P2       Q2       V2                    ')


Coul=zeros(64,3);

Coul(1,:)=[0 0 0];
Coul(floor(64/5),:)=[1 0 0];
Coul(floor(2*64/5),:)=[0 0 1];
Coul(floor(3*64/5),:)=[0 1 0];
Coul(floor(4*64/5),:)=[0 0.8 1];
Coul(floor(5*64/5),:)=[1 0 0.5];

Coul2=SmoothDec(Coul,[1,0.1]);
Coul2(1,:)=[0 0 0];
% Coul2(end,:)=[1 0.8 0];

for i=1:64
Coul2(i,:)=Coul2(i,:)/max(Coul2(i,:));
end
Coul2(isnan(Coul2))=0;
colormap(Coul2)
caxis([1, 6])

if nargin == 5
    saveas(gcf, [save_path, 'repartition.fig'], 'fig');  
    saveas(gcf, [save_path, 'repartition.pdf'], 'pdf');
end

if 0
    uiopen('/Users/karimbenchenane/Documents/MATLAB/PrgMATLAB/New/AnalysesAngelo/pca_randompoisson_mycoeff_withmodel_commonQZ.fig',1)
    disp(' ')
    disp(' Legend')
    disp(' ')
    disp(' r+   m+   g+   k+   b+   r.   m.   g.   k.   b.')
    disp(' S1   D1   P1   Q1   V1   S2   D2   P2   Q2   V2')
    disp(' ')
end

end
