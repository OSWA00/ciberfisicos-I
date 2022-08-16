clear
close all
clc

% Ideal trajectory
x = 0:0.1:10; % Time
y = sin(x);

% Simulation parameters
vec_length = size(x);

% Noise parameters
initial_noise = 0.2;
noise_y = generate_noise(vec_length(2), initial_noise);

% (x, y) with noise
% x_actual = x;
y_actual = y + noise_y;

% Phase change
phase_change = 0.7;

figure(1)
plot(x, y);
hold on;
scatter(x, y_actual);
hold on;
plot(x + phase_change, y);
hold on;
scatter(x + phase_change, y_actual);
legend('Ideal path', 'Noise path', ...
    'Out-phase signal', 'Noise & Out-phase signal')
grid on;
