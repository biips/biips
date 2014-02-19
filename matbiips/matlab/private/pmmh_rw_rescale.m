function rw = pmmh_rw_rescale(rw, p)

p = min(1.0, p);
rw.pmean = rw.pmean + 1*(p-rw.pmean)/rw.niter;

if ~rw.rescale
    return;
end
rw.lstep = rw.lstep + rw.alpha^(rw.niter)*(rw.pmean-rw.targetprob);


% 
% % OLD CODE: REMOVED THE PLUMMER UPDATE
% switch(rw.rescale_type)
%     case 'dureau'
%         rw.lstep = rw.lstep + rw.alpha^(rw.niter)*(rw.pmean-rw.targetprob)        
%         
%     case 'plummer'
%         % The step size is adapted to achieve the target acceptance rate
%         % using a noisy gradient algorithm
%         rw.lstep = rw.lstep + (p - rw.targetprob)/rw.ncrosstarget;
%     otherwise
%         error('Unkown rescaling technique')
%         
% end
% 
% if ((p>rw.targetprob) ~= rw.povertarget)
%     rw.povertarget = ~rw.povertarget;
%     rw.ncrosstarget = rw.ncrosstarget + 1;
% end
