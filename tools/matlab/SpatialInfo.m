function stat=SpatialInfo(map,meanrate)


% si pas de meanrate : probabilit� d'occupation uniforme

%-------------------------------------------------------------------
%-------------------------------------------------------------------



%proba=map.time/sum(sum(map.time);

MR=map.rate;
MT=map.time;
list=find(MT > 0);            
proba=zeros(size(map.time)); % pour etre insensible a l'occupation ch Jung et al. J Neurosci 1994
proba(list)=1/length(list);


try 
    meanrate;
catch
    
    meanrate=sum(sum(MR.*proba));
end


%-------------------------------------------------------------------
%-------------------------------------------------------------------



spaceinfo=(map.rate/meanrate).*log2(map.rate/meanrate).*proba;


spaceinfo(isnan(spaceinfo))=0;
stat.info=sum(sum(spaceinfo));



%-------------------------------------------------------------------

temp=sum(sum(map.rate.*(proba)));
temp2=map.rate;

sparsity=(temp.*temp)./sum(sum((temp2.*temp2.*proba)));
% sparsity(isnan(sparsity))=0;
% 
% stat.sparsity=sum(sum(sparsity));
stat.sparsity=sparsity;


%-------------------------------------------------------------------


%figure, imagesc(spaceinfo), axis xy
%title(['spatial info: ',num2str(stat.info),'   sparsity: ',num2str(sparsity)])


