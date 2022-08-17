close all

% Check if map and path exist
if exist('map', 'var') == 0 && exist('path', 'var') == 0
    [path, map] = PRM();
end

theta_vec = [0 pi/4];
x_vec = [0 0];
y_vec = [0 0];

theta_noise = [0 pi/4];
x_noise = [0 0];
y_noise = [0 0];

figure(5)
for i = 1:size(path)
    [x_vec, y_vec, theta_vec] = Control(1, 0.1, 10, 50, x_vec(end), y_vec(end), theta_vec(end), path(i, 1), path(i, 2), 0);
    [x_noise, y_noise, theta_noise] = Control(1, 0.1, 10, 50, x_noise(end), y_noise(end), theta_noise(end), path(i, 1), path(i, 2), 0.3); 
    scatter(x_vec, y_vec)
    hold on
    plot(x_noise, y_noise)
    hold on
end
grid on;
xlabel("X position");
ylabel("Y position");
title("Bycicle robot simulation");

% Out-phase X
