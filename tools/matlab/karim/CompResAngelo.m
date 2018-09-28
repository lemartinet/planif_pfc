% function [clusterRepro,clusterReproM,clusterReproMr,DistanceCluster,listN,repart]=CompResAngelo




% legende
% 
% styl = {'r+' 'm+' 'g+' 'k+' 'b+' 'r.' 'm.' 'g.' 'k.' 'b.'};
% numb = { S1   D1   P1   Q1   V1   S2   D2   P2   Q2   V2};



%----------------------------------------------------------------------
%----------------------------------------------------------------------
%----------------------------------------------------------------------


% Comparaison résults

% 
% Pour les histos :
%  * sur la population identifiée comme "spatiale" dans la PCA :
%    - taille champs récepteurs pour voir si on peut voir une
% distribution bimodale (comme C1 vs C2)
%    - slewness pour voir si on peut faire la difference entre des
% neurones "s" et "p"
%  * sur la population "non-spatiale", i.e. les neurones "v" du modèle :
%    - fréquence préférée de chaque neurone (cf figure ds article)
%  * puisque c facile à faire, autant faire un histo de toutes les
% autres mesures sur toute la population de neurones.


%-------------------------------------------------------------------
%choix des conditions
%-------------------------------------------------------------------

% 
% disp('  ')
% 
% % choix=1; disp('clustering manuel')   % selection manuelle     
% choix=0; disp('clustering kmeans')   % selection kmeans     


close all

disp('  ')

% fieldsize=1; disp('without field size')  %en ne prenant pas le field size dans la PCA
fieldsize=0; disp('with field size')     %en prenant le field size dans la PCA

disp('  ')

% detail=0;
detail=1; % plot les détails des composantes et la matrice de correlation


%-------------------------------------------------------------------

% selection en fonction du taux de decharge

seuilD=0;  %valeur par default: 1.7
seuilU=100;


%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------






        cd /Users/karimbenchenane/Documents/MATLAB/PrgMATLAB/New/AnalysesAngelo

        load VAngelo181014 V
        V2=V;
        load VAngelo181020 V
        V3=[V2;V];
        load VAngelo181021 V
        V3=[V3;V];
        load VAngelo181024 V
        V3=[V3;V];
        load VAngelo181011 V
        V3=[V3;V];
        load VAngelo181012 V
        V3=[V3;V];
        load VAngelo181025 V
        V3=[V3;V];
        load VAngelo181026 V
        V3=[V3;V];
        load VAngelo150628 V
        V3=[V3;V];


        %-------------------------------------------------------------------
        %-------------------------------------------------------------------
        %-------------------------------------------------------------------



        % V4=V3(:,[[3:7],8]);
        % V4=V3(:,[[3:7],9]);


        variaM{1}='fsize'; %3
        variaM{2}='mfr'; %4
        variaM{3}='sfr'; %5
        variaM{4}='lks'; % 6 combien de fois le neurone repond au dessus de la moyenne plus standard
        variaM{5}='info'; %7
        variaM{6}='sk'; %8
        variaM{7}='autreSk'; %9
        
                %     V(i,1)=numFile;
        %     V(i,2)=i;

        
if fieldsize==1;

        varia{1}='mfr';     %4
        varia{2}='sfr';     %5
        varia{3}='meanFr';  %6    occupation eguale
        varia{4}='stdFr';   %7   occupation eguale
        varia{5}='lks';     %8
        varia{6}='info';    %9
        varia{7}='infoEqui';%10
        varia{8}='spatialInfo';     %11
        varia{9}='sparsity';       %12
        varia{10}='specificity';    %13
        varia{11}='sk';     %14
        varia{12}='skcut';  %15

        
        variat{1}='fsize';   %3
        variat{2}='mfr';     %4
        variat{3}='sfr';     %5
        variat{4}='meanFr';  %6    occupation eguale
        variat{5}='stdFr';   %7   occupation eguale
        variat{6}='lks';     %8
        variat{7}='info';    %9
        variat{8}='infoEqui';%10
        variat{9}='spatialInfo';     %11
        variat{10}='sparsity';       %12
        variat{11}='specificity';    %13
        variat{12}='sk';     %14
        variat{13}='skcut';  %15
        
else
        varia{1}='fsize';   %3
        varia{2}='mfr';     %4
        varia{3}='sfr';     %5
        varia{4}='meanFr';  %6    occupation eguale
        varia{5}='stdFr';   %7   occupation eguale
        varia{6}='lks';     %8
        varia{7}='info';    %9
        varia{8}='infoEqui';%10
        varia{9}='spatialInfo';     %11
        varia{10}='sparsity';       %12
        varia{11}='specificity';    %13
        varia{12}='sk';     %14
        varia{13}='skcut';  %15
        
        variat=varia;
end

        variaM=varia;
        

%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------

disp('  ')

        load('/Users/karimbenchenane/Documents/MATLAB/PrgMATLAB/New/AnalysesAngelo/Q_random.mat'), disp('  Noise')
%         load('/Users/karimbenchenane/Documents/MATLAB/PrgMATLAB/New/AnalysesAngelo/Q_randomPoisson.mat'), disp('  Noise Poisson')

disp('  ')
        
        QMr=Q;



%-------------------------------------------------------------------

        load('/Users/karimbenchenane/Documents/MATLAB/PrgMATLAB/New/AnalysesAngelo/Q.mat')

        QM=Q;
       
        % v=2:6;  % sans fsize
        % v=1:6;  % avec fsize

        %v=[1,2,3,5,7,8];


%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------


        


if fieldsize==1

    V4=V3(:,[4:end]);  %disp('sans fsize')   % sans fsize
%     V4=V3(:,[2,6,9,13]+2);  % truc de cons de Manu
    
    
    V5=V3(:,3:end);
    
    [hh,bb]=hist(V5(:,2),50);
    
    
    V4=V4(find(V5(:,2)>seuilD&V5(:,2)<seuilU),:);
    V5=V5(find(V5(:,2)>seuilD&V5(:,2)<seuilU),:);
    

    [hh2,bb2]=hist(V5(:,2),bb);

    figure('color',[1 1 1])
    A1=area(bb,hh);
    hold on, A2=area(bb,hh2);
    set(A1,'FaceColor',[0.3 0.3 0.3])
    set(A2,'FaceColor',[1 0 0])
    set(A1,'LineStyle',':')
    set(A2,'LineStyle',':')
    alpha(0.5)
        
    
    V4=zscore(V4);

    [coeff,sc,lat,tsq]=princomp(V4);

 
    
    
%     v=2:6;  % sans fsize
    v=2:13;  % sans fsize
%     v=[2,6,9,13]; % truc de cons de Manu
    
    Q3=zscore(QM(:,v));
    [coeffM, scM, latM,tsqM]=princomp(Q3);
    
    %v=[1,2,3,5,7,8];
    Q4=zscore(QMr(:,v));
    [coeffMr, scMr, latMr,tsqMr]=princomp(Q4);

else

    V4=V3(:,[3:end]); %disp('avec fsize')   % avec fsize
    V5=V3(:,3:end);

    
    [hh,bb]=hist(V5(:,2),50);    
    
    V4=V4(find(V5(:,2)>seuilD&V5(:,2)<seuilU),:);
    V5=V5(find(V5(:,2)>seuilD&V5(:,2)<seuilU),:);
    
    
    [hh2,bb2]=hist(V5(:,2),bb);

    figure('color',[1 1 1])
    A1=area(bb,hh);
    hold on, A2=area(bb,hh2);
    set(A1,'FaceColor',[0.3 0.3 0.3])
    set(A2,'FaceColor',[1 0 0])
    set(A1,'LineStyle',':')
    set(A2,'LineStyle',':')
    alpha(0.5)
    title('Selection according to the firing rate')
    
    
    V4=zscore(V4);

    [coeff,sc,lat,tsq]=princomp(V4);

    
    %     v=1:6;  % avec fsize
    v=1:13;  % avec fsize
    
    Q3=zscore(QM(:,v));
    [coeffM, scM, latM,tsqM]=princomp(Q3);

    %v=[1,2,3,5,7,8];
    Q4=zscore(QMr(:,v));
    [coeffMr, scMr, latMr,tsqMr]=princomp(Q4);

end




%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------



C=corrcoef(V4);
[BE,idt]=sort(coeff(:,1));
[BE,idt2]=sort(coeff(:,2));

Cb=corrcoef(Q3);
[BE,idtb]=sort(coeffM(:,1));
[BE,idt2b]=sort(coeffM(:,2));

Cc=corrcoef(Q4);
[BE,idtc]=sort(coeffMr(:,1));
[BE,idt2c]=sort(coeffMr(:,2));


%-------------------------------------------------------------------
%-------------------------------------------------------------------
% figures
%-------------------------------------------------------------------
%-------------------------------------------------------------------


% sc(:,1)=sc(:,1);
% sc(:,2)=-sc(:,2);
% sc(:,3)=sc(:,3);



%-------------------------------------------------------------------
% les pods des composantes principales et matrice de corrélation ordonnées
%-------------------------------------------------------------------




if detail==1

% figure, plot(lat,'-ko','MarkerFaceColor','k','linewidth',2)

figure,
subplot(2,1,1),
imagesc(C(idt,idt)), title('exp')
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt))

subplot(2,1,2), imagesc(C(idt2,idt2))
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt2))


figure,
subplot(2,1,1),
imagesc(Cb(idtb,idtb)),, title('model')
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idtb))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idtb))

subplot(2,1,2), imagesc(Cb(idt2b,idt2b))
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt2b))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt2b))

figure,
subplot(2,1,1),
imagesc(Cc(idtc,idtc)), title('random')
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idtc))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idtc))

subplot(2,1,2), imagesc(Cc(idt2c,idt2c))
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt2c))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt2c))



%-------------------------------------------------------------------
% les coposantes en detail
%-------------------------------------------------------------------

% figure,
% subplot(2,1,1)
% scatter(sc(:,1),sc(:,2),40,sc(:,3),'filled')
% colormap(pink)
% hold on, plot(sc(:,1),sc(:,2),'ko')
% subplot(2,1,2)
% hold on, plot(coeff(:,1),'r')
% hold on, plot(coeff(:,2),'k')
% 
% set(gca,'xtick',[1:length(varia)])
% set(gca,'xticklabel',varia)

end


%-------------------------------------------------------------------
%-------------------------------------------------------------------

% figure, plot(sc(:,1),sc(:,2),'ko','MarkerFaceColor','k')

%-------------------------------------------------------------------
%-------------------------------------------------------------------

% xlim([-6 4])
% ylim([-6 10])



%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------
%Classification Kmeans

% X=sc;
opts = statset('Display','final');
[cidxSC, ctrsSC,sumSC] = kmeansKB(sc, 3, 'Distance','sqEuclidean','Replicates',5, 'Options',opts);
% figure('Color',[1 1 1])
%         plot(sc(cidxSC==1,1),sc(cidxSC==1,2),'k.', ...
%              sc(cidxSC==2,1),sc(cidxSC==2,2),'g.', ...
%              sc(cidxSC==3,1),sc(cidxSC==3,2),'b.',ctrsSC(:,1),ctrsSC(:,2),'kx');title(['Exp Data,  separation ',num2str(sum(sumSC))])
%     

% X=scM;
[cidxSCM, ctrsSCM,sumSCM] = kmeansKB(scM, 6, 'Distance','sqEuclidean','Replicates',5, 'Options',opts);
figure('Color',[1 1 1])
        plot(scM(cidxSCM==1,1),scM(cidxSCM==1,2),'k.', ...
             scM(cidxSCM==2,1),scM(cidxSCM==2,2),'g.', ...
             scM(cidxSCM==3,1),scM(cidxSCM==3,2),'b.', ...
             scM(cidxSCM==4,1),scM(cidxSCM==4,2),'c.', ...
             scM(cidxSCM==5,1),scM(cidxSCM==5,2),'m.', ...
             scM(cidxSCM==6,1),scM(cidxSCM==6,2),'y.', ctrsSCM(:,1),ctrsSCM(:,2),'kx');title(['Model Data,  separation ',num2str(sum(sumSCM))])

[cidxSCM, ctrsSCM,sumSCM] = kmeansKB(scM, 3, 'Distance','sqEuclidean','Replicates',5, 'Options',opts);

% figure('Color',[1 1 1])
%         plot(scM(cidxSCM==1,1),scM(cidxSCM==1,2),'k.', ...
%              scM(cidxSCM==2,1),scM(cidxSCM==2,2),'g.', ...
%              scM(cidxSCM==3,1),scM(cidxSCM==3,2),'b.',ctrsSCM(:,1),ctrsSCM(:,2),'kx');title(['Model Data,  separation ',num2str(sum(sumSCM))])
% figure('Color',[1 1 1])
%         plot(scM(cidxSCM==1,2),scM(cidxSCM==1,3),'k.', ...
%              scM(cidxSCM==2,2),scM(cidxSCM==2,3),'g.', ...
%              scM(cidxSCM==3,2),scM(cidxSCM==3,3),'b.',ctrsSCM(:,1),ctrsSCM(:,2),'kx');title(['Model Data,  separation ',num2str(sum(sumSCM))])
         

% X=scMr;
[cidxSCMr, ctrsSCMr,sumSCMr] = kmeansKB(scMr, 3, 'Distance','sqEuclidean','Replicates',5, 'Options',opts);
% figure('Color',[1 1 1])
%         plot(scMr(cidxSCMr==1,1),scMr(cidxSCMr==1,2),'k.', ...
%              scMr(cidxSCMr==2,1),scMr(cidxSCMr==2,2),'g.', ...
%              scMr(cidxSCMr==3,1),scMr(cidxSCMr==3,2),'b.',ctrsSCMr(:,1),ctrsSCMr(:,2),'kx');title(['Random Data,  separation ',num2str(sum(sumSCMr))])
% % % figure('Color',[1 1 1])
% % %         plot(X(cidx==1,2),X(cidx==1,3),'r.', ...
% % %              X(cidx==2,2),X(cidx==2,3),'b.', ...
% % %              X(cidx==3,2),X(cidx==3,3),'g.',ctrs(:,1),ctrs(:,2),'kx');
         
     
%-------------------------------------------------------------------

% pts=find(cidxSC==1);
% ptv=find(cidxSC==2);
% 
% 
% ptsM=find(cidxSCM==1);
% ptvM=find(cidxSCM==2);


%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------


clusterRepro(1)=length(find(cidxSC==1));
clusterRepro(2)=length(find(cidxSC==2));
clusterRepro(3)=length(find(cidxSC==3));

clusterReproM(1)=length(find(cidxSCM==1));
clusterReproM(2)=length(find(cidxSCM==2));
clusterReproM(3)=length(find(cidxSCM==3));

clusterReproMr(1)=length(find(cidxSCMr==1));
clusterReproMr(2)=length(find(cidxSCMr==2));
clusterReproMr(3)=length(find(cidxSCMr==3));


DistanceCluster(1)=sum(sumSC);
DistanceCluster(2)=sum(sumSCM);
DistanceCluster(3)=sum(sumSCMr);


%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------

listCluster(1)=length(find(cidxSC==1));
listCluster(2)=length(find(cidxSC==2));
listCluster(3)=length(find(cidxSC==3));    
[BE,id]=sort(listCluster);

pts=find(cidxSC==id(1));%b
ptv=find(cidxSC==id(2));%g
list=find(cidxSC==id(3));


listCluster(1)=length(find(cidxSCM==1));
listCluster(2)=length(find(cidxSCM==2));
listCluster(3)=length(find(cidxSCM==3));    
[BE,id]=sort(listCluster);

ptvM=find(cidxSCM==id(1)); %g
ptsM=find(cidxSCM==id(2)); %b
listM=find(cidxSCM==id(3));


listCluster(1)=length(find(cidxSCMr==1));
listCluster(2)=length(find(cidxSCMr==2));
listCluster(3)=length(find(cidxSCMr==3));    
[BE,id]=sort(listCluster);

ptsMr=find(cidxSCMr==id(1)); %b
ptvMr=find(cidxSCMr==id(2)); %g
listMr=find(cidxSCMr==id(3));

%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------

% if choix==0&fieldsize==1&detail==0
% 
%     try
%         cd /Users/karimbenchenane/Documents/MATLAB/PrgMATLAB/New/AnalysesAngelo
%         load DataPourFiguresManuAngelo
%     end
% 
% end

%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------




% try
%     lin;
% catch
% lin=1;
% end

% figure('Color',[1 1 1])
% V5=V3(:,3:end);
% for w=1:13
% [hi,bi]=hist(V5(:,w),20);
% [hiv,bi]=hist(V5(ptv,w),bi);
% [his,bi]=hist(V5(pts,w),bi);
% subplot(3,5,w), plot(bi,hi,'k','linewidth',lin)
% hold on, plot(bi,his,'r','linewidth',lin)
% hold on, plot(bi,hiv,'g','linewidth',lin)
% title(varia{w})
% end



figure('Color',[1 1 1])


% list=[1:size(V5,1)];
% [tf,loc] = ismember(list,pts);
% list=find(loc==0);
% [tf,loc] = ismember(list,ptv);
% list=find(loc==0);


sc2=sc;
sc2([pts;ptv],:)=[];

g=0;
co=1;
al=0.2;

% ratio=5;
ratio=1;

for w=1:13
[hit,bi]=hist(V5(:,w),20);
[hi,bi]=hist(V5(list,w),bi);
[hiv,bi]=hist(V5(ptv,w),bi);
[his,bi]=hist(V5(pts,w),bi);

subplot(3,5,w), 
A1=area(bi,hi/ratio);
hold on, As=area(bi,his);
hold on, Av=area(bi,hiv);
hold on, plot(bi,hit/ratio,'k');
set(A1,'FaceColor',[g g g])
set(As,'FaceColor',[0 0 co])
set(Av,'FaceColor',[0 co 0])
set(A1,'LineStyle',':')
set(As,'LineStyle',':')
set(Av,'LineStyle',':')
alpha(al)

title(variat{w})
end


% figure('Color',[1 1 1])
% 
% for w=1:13
% [hi,bi]=hist(V5(:,w),20);
% [hiv,bi]=hist(V5(ptv,w),bi);
% [his,bi]=hist(V5(pts,w),bi);
% subplot(3,5,w), plot(bi,hi/5,'k','linewidth',lin)
% hold on, plot(bi,his,'b','linewidth',lin)
% hold on, plot(bi,hiv,'g','linewidth',lin)
% title(varia{w})
% end





% figure('Color',[1 1 1])
% 
% V5=V3(:,3:end);
% for w=1:13
% [hi,bi]=hist(V5(:,w),20);
% [hiv,bi]=hist(V5(ptv,w),bi);
% [his,bi]=hist(V5(pts,w),bi);
% subplot(3,5,w), plot(bi,hi/max(hi),'k','linewidth',lin)
% hold on, plot(bi,his/max(his),'r','linewidth',lin)
% hold on, plot(bi,hiv/max(hiv),'g','linewidth',lin)
% title(varia{w})
% end


% uiopen('/Users/karimbenchenane/Dropbox/karim/neuron_data/1-2-3.fig',1)


%-------------------------------------------------------------------
%-------------------------------------------------------------------
%nuage de points des données exp

% figure('Color',[1 1 1]), hold on
% plot3(sc2(:,1),sc2(:,2),sc2(:,3),'ko','MarkerFaceColor','k')
% plot3(sc(pts,1),sc(pts,2),sc(pts,3),'ko','MarkerFaceColor','b')
% plot3(sc(ptv,1),sc(ptv,2),sc(ptv,3),'ko','MarkerFaceColor','g')
% xlabel('C1')
% ylabel('C2')
% zlabel('C3')
% title('exp data')


%-------------------------------------------------------------------
%-------------------------------------------------------------------



% figure('Color',[1 1 1]), hold on
% plot(sc(:,1),sc(:,2),'ko','MarkerFaceColor','k')
% plot(sc(ptv,1),sc(ptv,2),'ko','MarkerFaceColor','g')
% plot(sc(pts,1),sc(pts,2),'ko','MarkerFaceColor','r')




%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------
%nuage de points des données Model

% figure('Color',[1 1 1]), hold on
% plot(scM(:,2),scM(:,3),'ko','MarkerFaceColor','k')
% plot(scM(ptvM,2),scM(ptvM,3),'ko','MarkerFaceColor','g')
% plot(scM(ptsM,2),scM(ptsM,3),'ko','MarkerFaceColor','b')
% title('model data')

%-------------------------------------------------------------------
%-------------------------------------------------------------------

ratio=2;
ratio=1;
% 
% listM=[1:size(QM,1)];
% [tf,loc] = ismember(listM,ptsM);
% listM=find(loc==0);
% [tf,loc] = ismember(listM,ptvM);
% listM=find(loc==0);


figure('Color',[1 1 1])

for w=1:13

    
[hitM,biM]=hist(QM(:,w),20);
[hiM,biM]=hist(QM(listM,w),biM);
[hivM,biM]=hist(QM(ptvM,w),biM);
[hisM,biM]=hist(QM(ptsM,w),biM);

subplot(3,5,w), 
A1=area(biM,hiM/ratio);
hold on, As=area(biM,hisM);
hold on, Av=area(biM,hivM);
hold on, plot(biM,hitM/ratio,'k');
set(A1,'FaceColor',[g g g])
set(As,'FaceColor',[0 0 co])
set(Av,'FaceColor',[0 co 0])
set(A1,'LineStyle',':')
set(As,'LineStyle',':')
set(Av,'LineStyle',':')
alpha(al)

% subplot(4,2,w), plot(biM,hiM/2,'k','linewidth',lin)
% hold on, plot(biM,hisM,'b','linewidth',lin)
% hold on, plot(biM,hivM,'g','linewidth',lin)

title(variat{w})
end

%-------------------------------------------------------------------
%-------------------------------------------------------------------

% listMr=[1:size(QMr,1)];
% [tf,loc] = ismember(listMr,ptsMr);
% listMr=find(loc==0);
% [tf,loc] = ismember(listMr,ptvMr);
% listMr=find(loc==0);

%-------------------------------------------------------------------
%-------------------------------------------------------------------

% figure('Color',[1 1 1]), hold on
% plot(scMr(:,2),scMr(:,3),'ko','MarkerFaceColor','k')
% % plot(scMr(ptvM,2),scMr(ptvM,3),'ko','MarkerFaceColor','g')
% % plot(scMr(ptsM,2),scMr(ptsM,3),'ko','MarkerFaceColor','b')
% title('random data')


%-------------------------------------------------------------------
%-------------------------------------------------------------------


% figure, plot(sc(:,1),sc(:,2),'ko','MarkerFaceColor','k'), title('Exp')
% hold on, plot(scM(:,1),scM(:,2),'ro','MarkerFaceColor','r'), title('Model')


%-------------------------------------------------------------------
%-------------------------------------------------------------------



% subplot(3,2,2),scatter(sc(:,x1),sc(:,x2),30,sc(:,x3),'filled')
% subplot(3,2,4),scatter(-scM(:,x1),-scM(:,x2),30,scM(:,x3),'filled')
% subplot(3,2,6),scatter(-scMr(:,x1),-scMr(:,x2),30,scMr(:,x3),'filled')

         
%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------


[BE,idc]=sort(coeff(:,1));
[BE,idcM]=sort(coeffM(:,1));
[BE,idcMr]=sort(coeffMr(:,1));
idcM=idc;
idcMr=idc;

%-------------------------------------------------------------------

figure('Color',[1 1 1])
subplot(3,3,1),hold on
plot(lat,'-ko','MarkerFaceColor','k','linewidth',2)
subplot(3,3,[2:3]),hold on
plot(coeff(idc,1),'-ko','MarkerFaceColor','k','linewidth',2)
plot(coeff(idc,2),'-bo','MarkerFaceColor','b','linewidth',2)
plot(coeff(idc,3),'-ro','MarkerFaceColor','r','linewidth',2)
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idc))

subplot(3,3,4),hold on
plot(latM,'-ko','MarkerFaceColor','k','linewidth',2)
subplot(3,3,[5:6]),hold on
plot(coeffM(idcM,1),'-ko','MarkerFaceColor','k','linewidth',2)
plot(coeffM(idcM,2),'-bo','MarkerFaceColor','b','linewidth',2)
plot(coeffM(idcM,3),'-ro','MarkerFaceColor','r','linewidth',2)
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',variaM(idcM))

subplot(3,3,7),hold on
plot(latMr,'-ko','MarkerFaceColor','k','linewidth',2)
subplot(3,3,[8:9]),hold on
plot(coeffMr(idcMr,1),'-ko','MarkerFaceColor','k','linewidth',2)
plot(coeffMr(idcMr,2),'-bo','MarkerFaceColor','b','linewidth',2)
plot(coeffMr(idcMr,3),'-ro','MarkerFaceColor','r','linewidth',2)
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',variaM(idcMr))


%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------



couleur{1}='k';
couleur{2}='r';
couleur{3}='b';

figure('Color',[1 1 1]), 

subplot(3,1,1), hold on, 
plot(coeff(:,1),coeffM(:,1),'ko','MarkerFaceColor','k')
plot(coeff(:,1),coeffM(:,2),'ro','MarkerFaceColor','r')
plot(coeff(:,1),coeffM(:,3),'bo','MarkerFaceColor','b')

i1=1;
probabi=1;
for i2=1:3
    [r,pro]=corrcoef(coeff(:,i1),coeffM(:,i2));
    if pro(1,2)<0.05
%     [robustbeta,stats] = robustfit(coeff(:,i1),coeffM(:,i2));
% %     figure, plot(coeff(:,i1),coeffM(:,i2),'o')
%     hold on, plot(coeff(:,i1),robustbeta(1)+robustbeta(2)*coeff(:,i1),couleur{i2})
    probabi=min(probabi,pro(1,2));
    robustbeta=polyfit(coeff(:,i1),coeffM(:,i2),1);
    % figure, plot(coeff(:,1),coeffM(:,3),'o')
    hold on, plot(coeff(:,i1),robustbeta(2)+robustbeta(1)*coeff(:,i1),couleur{i2})
    end
end
title(['First principal component vs model p=',num2str(probabi)])

subplot(3,1,2), hold on, 
plot(coeff(:,2),coeffM(:,1),'ko','MarkerFaceColor','k')
plot(coeff(:,2),coeffM(:,2),'ro','MarkerFaceColor','r')
plot(coeff(:,2),coeffM(:,3),'bo','MarkerFaceColor','b')

i1=2;
probabi=1;
for i2=1:3
    [r,pro]=corrcoef(coeff(:,i1),coeffM(:,i2));
    if pro(1,2)<0.05
%     [robustbeta,stats] = robustfit(coeff(:,i1),coeffM(:,i2));
% %     figure, plot(coeff(:,i1),coeffM(:,i2),'o')
%     hold on, plot(coeff(:,i1),robustbeta(1)+robustbeta(2)*coeff(:,i1),couleur{i2})
    probabi=min(probabi,pro(1,2));
    robustbeta=polyfit(coeff(:,i1),coeffM(:,i2),1);
    % figure, plot(coeff(:,1),coeffM(:,3),'o')
    hold on, plot(coeff(:,i1),robustbeta(2)+robustbeta(1)*coeff(:,i1),couleur{i2})
    end
end
title(['Second principal component vs model p=',num2str(probabi)])

subplot(3,1,3), hold on, 
plot(coeff(:,3),coeffM(:,1),'ko','MarkerFaceColor','k')
plot(coeff(:,3),coeffM(:,2),'ro','MarkerFaceColor','r')
plot(coeff(:,3),coeffM(:,3),'bo','MarkerFaceColor','b')

i1=3;
probabi=1;
for i2=1:3
    [r,pro]=corrcoef(coeff(:,i1),coeffM(:,i2));
    if pro(1,2)<0.05
%     [robustbeta,stats] = robustfit(coeff(:,i1),coeffM(:,i2));
% %     figure, plot(coeff(:,i1),coeffM(:,i2),'o')
%     hold on, plot(coeff(:,i1),robustbeta(1)+robustbeta(2)*coeff(:,i1),couleur{i2})
    probabi=min(probabi,pro(1,2));
    robustbeta=polyfit(coeff(:,i1),coeffM(:,i2),1);
    % figure, plot(coeff(:,1),coeffM(:,3),'o')
    hold on, plot(coeff(:,i1),robustbeta(2)+robustbeta(1)*coeff(:,i1),couleur{i2})
    end
end
title(['Third principal component vs model p=',num2str(probabi)])


%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------



couleur{1}='k';
couleur{2}='r';
couleur{3}='b';

figure('Color',[1 1 1]), 

subplot(3,1,1), hold on, 
plot(coeff(:,1),coeffMr(:,1),'ko','MarkerFaceColor','k')
plot(coeff(:,1),coeffMr(:,2),'ro','MarkerFaceColor','r')
plot(coeff(:,1),coeffMr(:,3),'bo','MarkerFaceColor','b')

i1=1;
probabi=1;
for i2=1:3
    [r,pro]=corrcoef(coeff(:,i1),coeffMr(:,i2));
    if pro(1,2)<0.05
%     [robustbeta,stats] = robustfit(coeff(:,i1),coeffM(:,i2));
% %     figure, plot(coeff(:,i1),coeffM(:,i2),'o')
%     hold on, plot(coeff(:,i1),robustbeta(1)+robustbeta(2)*coeff(:,i1),couleur{i2})
    probabi=min(probabi,pro(1,2));
    robustbeta=polyfit(coeff(:,i1),coeffMr(:,i2),1);
    % figure, plot(coeff(:,1),coeffM(:,3),'o')
    hold on, plot(coeff(:,i1),robustbeta(2)+robustbeta(1)*coeff(:,i1),couleur{i2})
    end
end
title(['First principal component vs random p=',num2str(probabi)])


subplot(3,1,2), hold on, 
plot(coeff(:,2),coeffMr(:,1),'ko','MarkerFaceColor','k')
plot(coeff(:,2),coeffMr(:,2),'ro','MarkerFaceColor','r')
plot(coeff(:,2),coeffMr(:,3),'bo','MarkerFaceColor','b')

i1=2;
probabi=1;
for i2=1:3
    [r,pro]=corrcoef(coeff(:,i1),coeffMr(:,i2));
    if pro(1,2)<0.05
%     [robustbeta,stats] = robustfit(coeff(:,i1),coeffM(:,i2));
% %     figure, plot(coeff(:,i1),coeffM(:,i2),'o')
%     hold on, plot(coeff(:,i1),robustbeta(1)+robustbeta(2)*coeff(:,i1),couleur{i2})
    probabi=min(probabi,pro(1,2));
    robustbeta=polyfit(coeff(:,i1),coeffMr(:,i2),1);
    % figure, plot(coeff(:,1),coeffM(:,3),'o')
    hold on, plot(coeff(:,i1),robustbeta(2)+robustbeta(1)*coeff(:,i1),couleur{i2})
    end
end
title(['Second principal component vs random p=',num2str(probabi)])

subplot(3,1,3), hold on, 
plot(coeff(:,3),coeffMr(:,1),'ko','MarkerFaceColor','k')
plot(coeff(:,3),coeffMr(:,2),'ro','MarkerFaceColor','r')
plot(coeff(:,3),coeffMr(:,3),'bo','MarkerFaceColor','b'), title('Third principal component')

i1=3;
probabi=1;
for i2=1:3
    [r,pro]=corrcoef(coeff(:,i1),coeffMr(:,i2));
    if pro(1,2)<0.05
%     [robustbeta,stats] = robustfit(coeff(:,i1),coeffM(:,i2));
% %     figure, plot(coeff(:,i1),coeffM(:,i2),'o')
%     hold on, plot(coeff(:,i1),robustbeta(1)+robustbeta(2)*coeff(:,i1),couleur{i2})
    probabi=min(probabi,pro(1,2));
    robustbeta=polyfit(coeff(:,i1),coeffMr(:,i2),1);
    % figure, plot(coeff(:,1),coeffM(:,3),'o')
    hold on, plot(coeff(:,i1),robustbeta(2)+robustbeta(1)*coeff(:,i1),couleur{i2})
    end
end
title(['Third principal component vs random p=',num2str(probabi)])


%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------

% figure('Color',[1 1 1])
% subplot(3,1,1),scatter(sc(:,x1),sc(:,x2),30,sc(:,x3),'filled')
% subplot(3,1,2),scatter(-scM(:,x1),-scM(:,x2),30,scM(:,x3),'filled')
% subplot(3,1,3),scatter(-scMr(:,x1),-scMr(:,x2),30,scMr(:,x3),'filled')

scM2=scM;
scM2([ptsM;ptvM],:)=[];

scMr2=scMr;
scMr2([ptsMr;ptvMr],:)=[];

x1=1;
x2=2;
x3=3;

figure('Color',[1 1 1])

subplot(3,1,1),hold on
plot3(sc2(:,x1),sc2(:,x2),sc2(:,x3),'k.')
plot3(sc(ptv,x1),sc(ptv,x2),sc(ptv,x3),'g.')
plot3(sc(pts,x1),sc(pts,x2),sc(pts,x3),'b.')
xlabel('C1')
ylabel('C2')
zlabel('C3')
title('exp data')

subplot(3,1,2),,hold on
plot3(scM2(:,x1),scM2(:,x2),scM2(:,x3),'k.')
plot3(scM(ptvM,x1),scM(ptvM,x2),scM(ptvM,x3),'g.')
plot3(scM(ptsM,x1),scM(ptsM,x2),scM(ptsM,x3),'b.')
xlabel('C1')
ylabel('C2')
zlabel('C3')
title('model data')

subplot(3,1,3), hold on
plot3(scMr2(:,x1),scMr2(:,x2),scMr2(:,x3),'k.')
plot3(scMr(ptvMr,x1),scMr(ptvMr,x2),scMr(ptvMr,x3),'g.')
plot3(scMr(ptsMr,x1),scMr(ptsMr,x2),scMr(ptsMr,x3),'b.')
xlabel('C1')
ylabel('C2')
zlabel('C3')
title('random data')




%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------
% figure avec les poids des composanes du model

% figure
% try
% plot(V4*coeffM(:,1),V4*coeffM(:,2),'ko','MarkerFaceColor','k'), title('Exp weight from the Model')
% hold on, plot(scM(:,1),scM(:,2),'ro','MarkerFaceColor','r'), title('Model')
% catch
% close
% end
% 
% figure, 
% % try
%     subplot(1,2,1), hold on
%     plot(V4*coeffM(:,1),V4*coeffM(:,2),'ko','MarkerFaceColor','k')
%     plot(V4(ptv,:)*coeffM(:,1),V4(ptv,:)*coeffM(:,2),'ko','MarkerFaceColor','g')
%     plot(V4(pts,:)*coeffM(:,1),V4(pts,:)*coeffM(:,2),'ko','MarkerFaceColor','b'), title('Exp weight from the Model')
%     
%     subplot(1,2,2),  hold on
%     plot(scM(:,1),scM(:,2),'ko','MarkerFaceColor','k')
%     plot(scM(ptvM,1),scM(ptvM,2),'ko','MarkerFaceColor','g')
%     plot(scM(ptsM,1),scM(ptsM,2),'ko','MarkerFaceColor','b'), title('Model')
    
% catch
%     close
% end


V4b1=V4*coeffM(:,1);
V4b1([pts;ptv],:)=[];

V4b2=V4*coeffM(:,2);
V4b2([pts;ptv],:)=[];

V4b3=V4*coeffM(:,3);
V4b3([pts;ptv],:)=[];

    figure('Color',[1 1 1]), 

    subplot(1,3,1),  hold on
    plot3(sc2(:,1),sc2(:,2),sc2(:,3),'k.')
    plot3(sc(ptv,1),sc(ptv,2),sc(ptv,3),'g.')
    plot3(sc(pts,1),sc(pts,2),sc(pts,3),'b.'), title('Exp')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')

    subplot(1,3,2), hold on
    plot3(V4b1,V4b2,V4b3,'k.')
    plot3(V4(ptv,:)*coeffM(:,1),V4(ptv,:)*coeffM(:,2),V4(ptv,:)*coeffM(:,3),'g.')
    plot3(V4(pts,:)*coeffM(:,1),V4(pts,:)*coeffM(:,2),V4(pts,:)*coeffM(:,3),'b.'), title('Exp weight from the Model')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')

    subplot(1,3,3),  hold on
    plot3(scM2(:,1),scM2(:,2),scM2(:,3),'k.')
    plot3(scM(ptvM,1),scM(ptvM,2),scM(ptvM,3),'g.')
    plot3(scM(ptsM,1),scM(ptsM,2),scM(ptsM,3),'b.'), title('Model')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')

    

% 
% 
% figure, 
% subplot(1,3,1), scatter(scM(:,1),scM(:,2),30,scM(:,3),'filled'), title('Model')
% % xlim([-6 4])
% % ylim([-4 10])
% try
%     subplot(1,3,2), scatter(V4*coeffM(:,1),V4*coeffM(:,2),30,V4*coeffM(:,3),'filled'), title('Exp weight from the Model')
% % xlim([-6 4])
% % ylim([-4 10])
% 
% subplot(1,3,3), scatter(sc(:,1),sc(:,2),30,sc(:,3),'filled'), title('Exp')
% % xlim([-6 4])
% % ylim([-4 10])
% catch
%     close
% end


V4b1=V4*coeffM(:,1);
V4b1([pts;ptv],:)=[];

V4b2=V4*coeffM(:,2);
V4b2([pts;ptv],:)=[];

V4b3=V4*coeffM(:,3);
V4b3([pts;ptv],:)=[];

    figure('Color',[1 1 1]), 

    subplot(1,3,1),  hold on
    plot3(sc2(:,1),sc2(:,2),sc2(:,3),'k.')
    plot3(sc(ptv,1),sc(ptv,2),sc(ptv,3),'g.')
    plot3(sc(pts,1),sc(pts,2),sc(pts,3),'b.'), title('Exp')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')

    subplot(1,3,2), hold on
    plot3(V4b1,V4b2,V4b3,'k.')
    plot3(V4(ptv,:)*coeffMr(:,1),V4(ptv,:)*coeffMr(:,2),V4(ptv,:)*coeffMr(:,3),'g.')
    plot3(V4(pts,:)*coeffMr(:,1),V4(pts,:)*coeffMr(:,2),V4(pts,:)*coeffMr(:,3),'b.'), title('Exp weight from the Random')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')

    subplot(1,3,3),  hold on
    plot3(scMr2(:,1),scMr2(:,2),scMr2(:,3),'.k')
    plot3(scMr(ptvMr,1),scMr(ptvMr,2),scMr(ptvMr,3),'g.')
    plot3(scMr(ptsMr,1),scMr(ptsMr,2),scMr(ptsMr,3),'b.'), title('Random')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')
    
  
    
    
    
Q3b1=Q3*coeffM(:,1);
Q3b1([ptsM;ptvM],:)=[];

Q3b2=Q3*coeffM(:,2);
Q3b2([ptsM;ptvM],:)=[];

Q3b3=Q3*coeffM(:,3);
Q3b3([ptsM;ptvM],:)=[];    


    figure('Color',[1 1 1]), 

    subplot(1,3,1),  hold on
    plot3(scM2(:,1),scM2(:,2),scM2(:,3),'k.')
    plot3(scM(ptvM,1),scM(ptvM,2),scM(ptvM,3),'g.')
    plot3(scM(ptsM,1),scM(ptsM,2),scM(ptsM,3),'b.'), title('Model')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')

    subplot(1,3,2), hold on
    plot3(Q3b1,Q3b2,Q3b3,'k.')
    plot3(Q3(ptvM,:)*coeffMr(:,1),Q3(ptvM,:)*coeffMr(:,2),Q3(ptvM,:)*coeffMr(:,3),'g.')
    plot3(Q3(ptsM,:)*coeffMr(:,1),Q3(ptsM,:)*coeffMr(:,2),Q3(ptsM,:)*coeffMr(:,3),'b.'), title('Model weight from the Random')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')

    subplot(1,3,3),  hold on
    plot3(scMr2(:,1),scMr2(:,2),scMr2(:,3),'k.')
    plot3(scMr(ptvMr,1),scMr(ptvMr,2),scMr(ptvMr,3),'g.')
    plot3(scMr(ptsMr,1),scMr(ptsMr,2),scMr(ptsMr,3),'b.'), title('Random')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')

    
    
    
Q3b1=Q3*coeff(:,1);
Q3b1([ptsM;ptvM],:)=[];

Q3b2=Q3*coeff(:,2);
Q3b2([ptsM;ptvM],:)=[];

Q3b3=Q3*coeff(:,3);
Q3b3([ptsM;ptvM],:)=[];    


    figure('Color',[1 1 1]), 

    subplot(1,3,1),  hold on
    plot3(scM2(:,1),scM2(:,2),scM2(:,3),'k.')
    plot3(scM(ptvM,1),scM(ptvM,2),scM(ptvM,3),'g.')
    plot3(scM(ptsM,1),scM(ptsM,2),scM(ptsM,3),'b.'), title('Model')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')

    subplot(1,3,2), hold on
    plot3(Q3b1,Q3b2,Q3b3,'k.')
    plot3(Q3(ptvM,:)*coeff(:,1),Q3(ptvM,:)*coeff(:,2),Q3(ptvM,:)*coeff(:,3),'g.')
    plot3(Q3(ptsM,:)*coeff(:,1),Q3(ptsM,:)*coeff(:,2),Q3(ptsM,:)*coeff(:,3),'b.'), title('Model weight from the Exp')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')

    subplot(1,3,3),  hold on
    plot3(sc2(:,1),sc2(:,2),sc2(:,3),'k.')
    plot3(sc(ptv,1),sc(ptv,2),sc(ptv,3),'g.')
    plot3(sc(pts,1),sc(pts,2),sc(pts,3),'b.'), title('Exp')
    xlabel('C1')
    ylabel('C2')
    zlabel('C3')    
    
    
%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------


%    uiopen('/Users/karimbenchenane/Dropbox/karim/neuron_data/pca.fig',1)
%    uiopen('/Users/karimbenchenane/Dropbox/karim/neuron_data/pca_withrandompoisson.fig',1)
%    uiopen('/Users/karimbenchenane/Dropbox/karim/neuron_data/pca_random.fig',1)
uiopen('/Users/karimbenchenane/Documents/MATLAB/PrgMATLAB/New/AnalysesAngelo/pca_randompoisson_mycoeff_withmodel_commonQZ.fig',1)

%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------

x1=1;
x2=2;
x3=3;

figure('Color',[1 1 1])
subplot(3,1,1), hold on,
% subplot(3,2,1), hold on
plot(sc(:,x1),sc(:,x2),'k.'), title('Exp')
plot(sc(ptv,x1),sc(ptv,x2),'g.')
plot(sc(pts,x1),sc(pts,x2),'b.')
% xlim([-6 4])
% ylim([-6 10])

subplot(3,1,2), hold on,
% subplot(3,2,3), hold on,
plot(scM(:,x1),scM(:,x2),'k.'), title('Model')
plot(scM(ptvM,x1),scM(ptvM,x2),'g.')
plot(scM(ptsM,x1),scM(ptsM,x2),'b.')
% xlim([-6 4])
% ylim([-6 10])

subplot(3,1,3), hold on
% subplot(3,2,3), hold on,
plot(scMr(:,x1),scMr(:,x2),'k.'), title('Random')
plot(scMr(ptvMr,x1),scMr(ptvMr,x2),'g.')
plot(scMr(ptsMr,x1),scMr(ptsMr,x2),'b.')


%-------------------------------------------------------------------
%-------------------------------------------------------------------
%-------------------------------------------------------------------


Nb(1)=length(pts);
Nb(2)=length(ptv);
Nb(3)=size(V4,1)-length(pts)-length(ptv);

Nb(4)=length(ptsM);
Nb(5)=length(ptvM);
Nb(6)=size(Q3,1)-length(ptsM)-length(ptvM);

Nb(7)=length(ptsMr);
Nb(8)=length(ptvMr);
Nb(9)=size(Q4,1)-length(ptsMr)-length(ptvMr);    


Nb;
clusterRepro=sort(clusterRepro)
clusterReproM=sort(clusterReproM)
clusterReproMr=sort(clusterReproMr)
% DistanceCluster

listN{1}=list;
listN{2}=ptv;
listN{3}=pts;

listN{4}=listM;
listN{5}=ptvM;
listN{6}=ptsM;

listN{7}=listMr;
listN{8}=ptvMr;
listN{9}=ptsMr;

load CategorieNeuron

% k=1;
% 
% for m=4:6
%     
% repart2(k,1)=length(find(ismember(NC1_d,N(listN{m})))==1)/length(listN{m})*100;
% repart2(k,2)=length(find(ismember(NC1_p,N(listN{m})))==1)/length(listN{m})*100;
% repart2(k,3)=length(find(ismember(NC1_q,N(listN{m})))==1)/length(listN{m})*100;
% repart2(k,4)=length(find(ismember(NC1_s,N(listN{m})))==1)/length(listN{m})*100;
% repart2(k,5)=length(find(ismember(NC1_v,N(listN{m})))==1)/length(listN{m})*100;
% 
% repart2(k,7)=length(find(ismember(NC2_p,N(listN{m})))==1)/length(listN{m})*100;
% repart2(k,8)=length(find(ismember(NC2_q,N(listN{m})))==1)/length(listN{m})*100;
% repart2(k,9)=length(find(ismember(NC2_s,N(listN{m})))==1)/length(listN{m})*100;
% repart2(k,10)=length(find(ismember(NC2_v,N(listN{m})))==1)/length(listN{m})*100;
% 
% k=k+1;
% end

k=1;

for m=4:6
    
repart(k,1)=length(find(ismember(NC1_s,N(listN{m})))==1)/length(NC1_s)*100;
repart(k,2)=length(find(ismember(NC1_d,N(listN{m})))==1)/length(NC1_d)*100;
repart(k,3)=length(find(ismember(NC1_p,N(listN{m})))==1)/length(NC1_p)*100;
repart(k,4)=length(find(ismember(NC1_q,N(listN{m})))==1)/length(NC1_q)*100;
repart(k,5)=length(find(ismember(NC1_v,N(listN{m})))==1)/length(NC1_v)*100;

repart(k,6)=length(find(ismember(NC2_s,N(listN{m})))==1)/length(NC2_s)*100;
repart(k,7)=length(find(ismember(NC2_d,N(listN{m})))==1)/length(NC2_d)*100;
repart(k,8)=length(find(ismember(NC2_p,N(listN{m})))==1)/length(NC2_p)*100;
repart(k,9)=length(find(ismember(NC2_q,N(listN{m})))==1)/length(NC2_q)*100;
repart(k,10)=length(find(ismember(NC2_v,N(listN{m})))==1)/length(NC2_v)*100;

k=k+1;
end


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


% styl = {'r+' 'm+' 'g+' 'k+' 'b+' 'r.' 'm.' 'g.' 'k.' 'b.'};
% numb = { S1   D1   P1   Q1   V1   S2   D2   P2   Q2   V2};



% figure('Color',[1 1 1])
% % subplot(2,1,1),bar(repart)
% % subplot(2,1,2),bar(repart2)
% bar(repart2,1)
% xlabel('Legend: S1 D1 P1 Q1 V1 S2 D2 P2 Q2 V2')


figure('Color',[1 1 1])
bar(repart','stack')
xlabel('         S1      D1       P1       Q1       V1       S2       D2       P2       Q2       V2                    ')
ylabel('bleu/black, green/green, red/bleu')

