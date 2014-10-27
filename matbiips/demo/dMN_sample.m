function x = dMN_sample( mu, Sig )
% DMN_SAMPLE Draw a sample of distribution dMN
x = mu(:) + chol(Sig, 'lower') * randn(numel(mu), 1);
