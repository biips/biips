function flag = pmmh_rw_check_adapt(rw)

if (rw.pmean==0 || rw.pmean==1)
    flag = false;
    return;
end

dist = abs(log(rw.pmean/(1-rw.pmean)) ...
    - log(rw.targetprob/(1-rw.targetprob) ));

flag = (dist<0.5);