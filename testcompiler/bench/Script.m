for x ='a':'j'
    run_bench_all(1, [100, 500, 1000], 0.5, 100, {sprintf('hmm_1d_lin_gauss.%s.cfg', x)});
    run_bench_all(2, [100, 500, 1000], 0.5, 100, {sprintf('hmm_1d_non_lin_gauss.%s.cfg', x)});
end