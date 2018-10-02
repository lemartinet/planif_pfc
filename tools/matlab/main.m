% compute sur differents vars et output

% population responses

% plot_fr_all
close all
for i=1:24;
fr=zeros(size(pv23));fr2=zeros(size(pv23)); fr3=zeros(size(pv23));
fr(id)=frs_2(i,:);fr2(id)=frs_1(i,:);fr3(id)=frs_3(i,:);
figure,
subplot(3,1,1),contourf(fr);
subplot(3,1,2),contourf(fr2);
subplot(3,1,3),contourf(fr3);
end;



