parfor i = 1:10
    rng(i)
    % HMM 1D linear gaussian model
    filename = sprintf('hmm_1d_lin_gauss.%.2d.cfg', i);
    run_bench_all(1, [100], 0.5, 10000, {filename});
    movefile(filename, '../benchmarks')

    % HMM 1D non linear gaussian model
    filename = sprintf('hmm_1d_nonlin_gauss.%.2d.cfg', i);
    run_bench_all(2, [100], 0.5, 10000, {filename});
    movefile(filename, '../benchmarks')
end
