cd '/home/adrien-inria/workspace/biips/trunk/BiipsTest'

data_file_name =  'data.dat';


t_max = 10;
mu_x0 = 0;
var_x0 = 1;
a_x = 1;
b_x = 0;
var_x = 1;
a_y = 1;
var_y = 1;
rng_seed = 0;
nb_particles = 1000;

results_file_name = 'results.dat';
plot_file_name = 'plot.pdf';
gnuplot_file_name = 'gnuplot.plt';


f_data = mopen(data_file_name, 'w');
mfprintf(f_data, '%d\n', t_max);
mfprintf(f_data, '%f\n', mu_x0);
mfprintf(f_data, '%f\n', var_x0);
mfprintf(f_data, '%f\n', a_x);
mfprintf(f_data, '%f\n', b_x);
mfprintf(f_data, '%f\n', var_x);
mfprintf(f_data, '%f\n', a_y);
mfprintf(f_data, '%f\n', var_y);
mfprintf(f_data, '%d\n', rng_seed);
mfprintf(f_data, '%d\n', nb_particles);
mfprintf(f_data, '%s\n', results_file_name);
mfprintf(f_data, '%s\n', plot_file_name);
mfprintf(f_data, '%s\n', gnuplot_file_name);
mclose(f_data);


unix('Debug/BiipsTest 3 ' + data_file_name + ' -noprompt -noplot');


t = zeros(t_max+1,1);
x_gen = zeros(t_max+1,1);
y_obs = zeros(t_max+1,1);
x_est_KF = zeros(t_max+1,1);
x_est_PF = zeros(t_max+1,1);
x_est_PS = zeros(t_max+1,1);


f_results = mopen(results_file_name, 'r');
[n_results_data, legends] = mfscanf(7, f_results, '%s');
legends = legends(2:$);
for i=0:t_max
  [n_results_data, data] = mfscanf(6, f_results, '%f');
  t(i+1) = data(1);
  x_gen(i+1) = data(2);
  y_obs(i+1) = data(3);
  x_est_KF(i+1) = data(4);
  x_est_PF(i+1) = data(5);
  x_est_PS(i+1) = data(6);
end
file('close', f_results);

plot(t, x_gen, 'k--', t, x_est_KF, 'g', t, x_est_PF, 'b', t, x_est_PS, 'm');
xlabel(legends(1))
legend(legends(2), legends(3), legends(4), legends(5));

