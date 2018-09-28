% ControlAnalysisManu

%------------------------------------------------------------------------

try
    th;
catch
    
    th=0.055;
    th=0;
end


try
    choixPC;
catch
    
    choixPC=2; % 1: PCA sur mesures, 2: PCA sur neurons

end





%------------------------------------------------------------------------
%------------------------------------------------------------------------
%------------------------------------------------------------------------

cd /Users/karimbenchenane/Documents/MATLAB/PrgMATLAB/New/AnalysesAngelo

load CategorieNeuron

load Q_randomPoisson
Qrd=Q;
load Q_random
Qr=Q;
load Q
load DataExp

clear varia
clear varia2
clear varia3

% if choixPC==1;

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

% else
    

        
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
    
    
% end



%------------------------------------------------------------------------
%------------------------------------------------------------------------
%------------------------------------------------------------------------


M=Qexp(Qexp(:,2)>th*3,:);


Mz=zscore(M);

if choixPC~=1

Mz=Mz';
    
%     Mz=M';
%     Mz=zscore(Mz);

end

[coeff,sc,lat,tsq]=princomp(Mz);

C=corrcoef(Mz);

[BE,idt]=sort(coeff(:,1));
[BE,idt2]=sort(coeff(:,2));




figure('Color',[1 1 1]),

subplot(2,4,1),
imagesc(C(idt,idt)), title(['Data exp, th=',num2str(th)]), ylabel('1st PC')
try
    set(gca,'ytick',[1:length(varia)])
    set(gca,'yticklabel',varia(idt))
    set(gca,'xtick',[1:length(varia)])
    set(gca,'xticklabel',varia(idt))
end
subplot(2,4,5), imagesc(C(idt2,idt2)), ylabel('2nd PC')
try
    set(gca,'ytick',[1:length(varia)])
    set(gca,'yticklabel',varia(idt2))
    set(gca,'xtick',[1:length(varia)])
    set(gca,'xticklabel',varia(idt2))
end

%------------------------------------------------------------------------


M=Q(Q(:,2)>th,:);


Mz=zscore(M);

if choixPC~=1
    
Mz=Mz';    

%     Mz=M';
%     Mz=zscore(Mz);

[coeff,sc,lat,tsq]=princomp(Mz);
[BE,idt]=sort(coeff(:,1));
[BE,idt2]=sort(coeff(:,2));

listOK=find(Q(:,2)>th);
clear varia3;
a=1;
for i=listOK'
varia3{a}=varia2{i};
a=a+1;
end


end


C=corrcoef(Mz);

% figure('Color',[1 1 1]),
subplot(2,4,2),
imagesc(C(idt,idt)), title('Data model')
try
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt))
catch
set(gca,'ytick',[1:length(varia3)])
set(gca,'yticklabel',varia3(idt))
set(gca,'xtick',[1:length(varia3)])
set(gca,'xticklabel',varia3(idt))

end
subplot(2,4,6), imagesc(C(idt2,idt2))
try
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt2))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt2))
catch
set(gca,'ytick',[1:length(varia3)])
set(gca,'yticklabel',varia3(idt2))
set(gca,'xtick',[1:length(varia3)])
set(gca,'xticklabel',varia3(idt2))
end

%------------------------------------------------------------------------


M=Qr(Qr(:,2)>th,:);
Mz=zscore(M);

if choixPC~=1

Mz=Mz';    

%     Mz=M';
%     Mz=zscore(Mz);


[coeff,sc,lat,tsq]=princomp(Mz);
[BE,idt]=sort(coeff(:,1));
[BE,idt2]=sort(coeff(:,2));
end


C=corrcoef(Mz);
% figure('Color',[1 1 1]),
subplot(2,4,3),
imagesc(C(idt,idt)), title('Data Random')
try
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt))
end
subplot(2,4,7), imagesc(C(idt2,idt2))
try
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt2))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt2))
end

%------------------------------------------------------------------------


M=Qrd(Qrd(:,2)>th,:);
Mz=zscore(M);

if choixPC~=1

Mz=Mz';    

%     Mz=M';
%     Mz=zscore(Mz);

[coeff,sc,lat,tsq]=princomp(Mz);
[BE,idt]=sort(coeff(:,1));
[BE,idt2]=sort(coeff(:,2));
end


C=corrcoef(Mz);
% figure('Color',[1 1 1]),
subplot(2,4,4),
imagesc(C(idt,idt)), title('Data Random Poisson')
try
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt))
end
subplot(2,4,8), imagesc(C(idt2,idt2))
try
set(gca,'ytick',[1:length(varia)])
set(gca,'yticklabel',varia(idt2))
set(gca,'xtick',[1:length(varia)])
set(gca,'xticklabel',varia(idt2))
end



%------------------------------------------------------------------------
%------------------------------------------------------------------------
%------------------------------------------------------------------------
%Detail



if 1

    
    M=Q(Q(:,2)>th,:);


    Mz=zscore(M);

    if choixPC~=1

    Mz=Mz';    

    %     Mz=M';
    %     Mz=zscore(Mz);



    listOK=find(Q(:,2)>th);
    clear varia3;
    a=1;
    for i=listOK'
    varia3{a}=varia2{i};
    valu2(a)=valu(i);
    a=a+1;
    end


    end

    [coeff,sc,lat,tsq]=princomp(Mz);
    [BE,idt]=sort(coeff(:,1));
    [BE,idt2]=sort(coeff(:,2));
    
    
    try
    ratio=length(varia3)/length(Q)*100;
    ratio=floor(ratio*10)/10;
    catch
    ratio=100;    
    end
    
    C=corrcoef(Mz);

    figure('Color',[1 1 1]),
    
    imagesc(C(idt,idt)), title(['Data model 1st PC,   th=',num2str(th),',   ratio=',num2str(ratio),'%']), axis xy

        if choixPC==1
    set(gca,'ytick',[1:length(varia)])
    set(gca,'yticklabel',varia(idt))
    set(gca,'xtick',[1:length(varia)])
    set(gca,'xticklabel',varia(idt))

    
        end
        

        if choixPC~=1
    try
        groupneuron{1}='S1';
        groupneuron{2}='D1';
        groupneuron{3}='P1';
        groupneuron{4}='Q1';
        groupneuron{5}='V1';
        groupneuron{6}='S2';
        groupneuron{7}='D2';
        groupneuron{8}='P2';
        groupneuron{9}='Q2';       
        groupneuron{10}='V2';               

    binss=length(Mz)/12;
    hold on, plot(valu2(idt)*binss,'ko-','linewidth',1,'markerFaceColor','k')
    
    set(gca,'ytick',[binss:binss:10*binss])
    set(gca,'yticklabel',groupneuron)
    
    end
        end



end


% clear

