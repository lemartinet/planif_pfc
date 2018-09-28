function [nClu,coeff,listVar,cidx,H,B]=PlotCompResAngelo(M,nClu,listVar, save_path)


% ListVar: optionnal liste des variables a prendre en compte dans la PCA
% nClu: nomb re de cluster
% M matrice des donnees
%
%
%
%
% legende
% 
% styl = {'r+' 'm+' 'g+' 'k+' 'b+' 'r.' 'm.' 'g.' 'k.' 'b.'};
% numb = { S1   D1   P1   Q1   V1   S2   D2   P2   Q2   V2};
%
%
%
%----------------------------------------------------------------------
%----------------------------------------------------------------------
%----------------------------------------------------------------------
%
%
% Comparaison r�sults
%
% 
% Pour les histos :
%  * sur la population identifi�e comme "spatiale" dans la PCA :
%    - taille champs r�cepteurs pour voir si on peut voir une
% distribution bimodale (comme C1 vs C2)
%    - slewness pour voir si on peut faire la difference entre des
% neurones "s" et "p"
%  * sur la population "non-spatiale", i.e. les neurones "v" du mod�le :
%    - fr�quence pr�f�r�e de chaque neurone (cf figure ds article)
%  * puisque c facile � faire, autant faire un histo de toutes les
% autres mesures sur toute la population de neurones.


% if nClu>6
%     nClu=6;
% end



%variable names
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
        
try
    listVar;
    Mz=zscore(M(:,listVar));
catch
    Mz=zscore(M);
    listVar=[1:13];
end

disp(' ')

[coeff,sc,lat,tsq]=princomp(Mz);
 

figure('Color',[1 1 1])

subplot(3,1,1),hold on
plot(lat,'-ko','MarkerFaceColor','k','linewidth',2)

subplot(3,1,[2:3]),hold on
plot(coeff(:,1),[1:length(listVar)],'-ko','MarkerFaceColor','k','linewidth',2)
plot(coeff(:,2),[1:length(listVar)],'-ro','MarkerFaceColor','r','linewidth',2)
plot(coeff(:,3),[1:length(listVar)],'-bo','MarkerFaceColor','b','linewidth',2)
set(gca,'ytick',[1:length(listVar)])
set(gca,'yticklabel',varia(listVar))
title('black 1st PC, red 2nd PC, blue 3rd PC')


opts = statset('Display','final');
%[cidx, ctrs,sumS] = kmeans(sc, nClu, 'Distance','sqEuclidean','Replicates',5, 'Options',opts);
[cidx, ctrs,sumS] = kmeans(Mz, nClu, 'Distance','sqEuclidean','Replicates',5, 'Options',opts);

couleur{1}='k';
couleur{2}='r';
couleur{3}='b';
couleur{4}='g';
couleur{5}='c';
couleur{6}='m';
couleur{7}='y';
couleur{8}='k-';
couleur{9}='r-';
couleur{10}='g-';
couleur{11}='b-';

coul{1}=[0 0 0];
coul{2}=[1 0 0];
coul{3}=[0 0 1];
coul{4}=[0 1 0];
coul{5}=[0 1 1];
coul{6}=[1 0 1];
coul{7}=[1 1 0];
coul{8}=[0.5 0.5 0.5];
coul{9}=[0.5 0 0];
coul{10}=[0 0.5 0];
coul{11}=[0 0 0.5];

for i=1:nClu
listCluster(i)=length(find(cidx==i));
end

[BE,id]=sort(listCluster,'descend');


figure('Color',[1 1 1]), hold on

for i=1:nClu

    plot3(sc(cidx==id(i),1),sc(cidx==id(i),2),sc(cidx==id(i),3),[couleur{i},'.'])
    
    ra=sumS(id(i))/length(find(cidx==id(i)))/5;
    %[x,y,z] = ellipsoid(ctrs(id(i),1),ctrs(id(i),2),ctrs(id(i),3),ra,ra,ra,20);
    %surfl(x, y, z)
%shading interp
    %colormap([0.7 0.7 0.7])
    %alpha(0.2)

end

plot3(ctrs(:,1),ctrs(:,2),ctrs(:,3),'kx');
xlabel('C1')
ylabel('C2')
zlabel('C3')


title(['Separation ',num2str(sum(sumS))])
if nargin == 4
    saveas(gcf, [save_path, 'clusters.fig'], 'fig');  
    saveas(gcf, [save_path, 'clusters.pdf'], 'pdf');
end

%----------------------------------------------------------------------
%----------------------------------------------------------------------
%----------------------------------------------------------------------

% ratio=5;
ratio=1;

figure('Color',[1 1 1]), hold on

a=1;

for w=1:13

        [ht,b]=hist(M(:,w),20);

        subplot(3,5,w), hold on
        plot(b,ht/ratio,'k');

        
        for j=1:nClu


                [h{j},bb]=hist(M(find(cidx==id(j)),w),b);
                
                H{w,j}=h{j};
                B{w,j}=bb;
                                
                hold on, A{a}=area(b,h{j});

                set(A{a},'FaceColor',coul{j})
                set(A{a},'LineStyle',':')
                alpha(0.2)
                a=a+1;
        end
        
        
        title(varia{w})
                
                
end
if nargin == 4
    saveas(gcf, [save_path, 'all_measures.fig'], 'fig');    
    saveas(gcf, [save_path, 'all_measures.pdf'], 'pdf');    
end




%----------------------------------------------------------------------
%----------------------------------------------------------------------
%----------------------------------------------------------------------

if 0
    
C=corrcoef(M);
[BE,idt]=sort(coeff(:,1));
[BE,idt2]=sort(coeff(:,2));

figure('Color',[1 1 1]),
subplot(2,1,1),
imagesc(C(idt,idt)), title('1st PC')
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt))

subplot(2,1,2), imagesc(C(idt2,idt2)), title('2nd PC')
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt2))

end

if 1

a=1;

for w=1:13

        [ht,b]=hist(M(:,w),20);

        figure('Color',[1 1 1]), hold on
        %plot(b,ht/max(ht),'k-.','linewidth',2);
        htm=max(ht);
        plot(b,ht/htm,'k-.','linewidth',2);

        
        for j=1:nClu


                [h{j},bb]=hist(M(find(cidx==id(j)),w),b);
                

                
                %hold on, plot(b,h{j}/max(h{j}),'Color',couleur{j},'linewidth',2);
                %hold on, plot(b,h{j},'Color',couleur{j},'linewidth',2);
                hold on, A{a}=area(b,h{j}/htm);

                set(A{a},'FaceColor',coul{j})
                set(A{a},'LineStyle',':')
                %alpha(0.2)
                a=a+1;
                

        end
        if nargin == 4
            saveas(gcf, [save_path, varia{w}, '_hist.fig'], 'fig');    
            saveas(gcf, [save_path, varia{w}, '_hist.pdf'], 'pdf');    
        end 
        
        figure; hold on;
        for j=1:nClu

                A{a}=bar(j, mean(M(find(cidx==id(j)),w)),'FaceColor',coul{j});
                errorbar(j, mean(M(find(cidx==id(j)),w)), std(M(find(cidx==id(j)),w))/sqrt(length(M(find(cidx==id(j)),w))),'k');
                %set(A{a},'LineStyle','k');

        end
        
        
        title(varia{w})
        if nargin == 4
            saveas(gcf, [save_path, varia{w}, '.fig'], 'fig');    
            saveas(gcf, [save_path, varia{w}, '.pdf'], 'pdf');    
        end        
                
end


end

% for i=1:nClu
%     cidx(i)=id(i);
% end
