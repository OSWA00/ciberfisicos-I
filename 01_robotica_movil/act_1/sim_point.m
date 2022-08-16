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
min = -0.4;
max = 0.4;
% Acumulated noise
noise_y = generate_noise(vec_length(2), initial_noise);
% noise_y = min + (max + max) * rand(vec_length);

% (x, y) with noise
y_actual = y + noise_y;

% Phase change
phase_change = 1.2;

figure(1)
plot(x, y);
hold on;
plot(x, y_actual);
legend('Ideal signal', 'Noisy signal')
grid on;

figure(2)
plot(x, y);
hold on;
plot(x + phase_change, y)
legend('Ideal signal', 'Out-phased signal');
grid on;

figure(3);
plot(x, y);
hold on;
plot(x + phase_change, y_actual);
legend('Ideal signal', 'Out-phased & noisy signal')
grid on;
