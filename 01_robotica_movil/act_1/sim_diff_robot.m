clear
close all
clc

% Simulation parameters
time = 0;
time_final = 5;
time_delta = 0.01;

iters = (time_final - time) / time_delta;

trajectory = NaN(iters, 3);

% Robot parameters
robot_length = 0.2;

% Proportional control parameters
kt = 10;
kr = 10;

% Pose (x, y, theta)
pose = [0, 0 , 0];

% Pose desired (x, y, theta)
pose_desired = [1, 0.7, 0];

for i = 1:iters

    % Save current pose
    trajectory(i,:) = pose;

    % Update error (x, y, theta)
    pose(3) = atan2(pose_desired(2) - pose(2), pose_desired(1) - pose_desired(1));
    error = pose - pose_desired;

    % Control
    velocity = kt * sqrt(error(1)^2 + error(2)^2);
    omega = -kr * error(3);

    % Control saturation
    if velocity >= 1
        v = 1;
    end

    if omega >= pi/2
        omega = pi / 2;
    end

    if omega <= -pi/2
        omega = -pi/2;
    end

    % Kinematics
    velocity_right = velocity + 0.5 * robot_length * omega;
    velocity_left = velocity - 0.5 * robot_length * omega;

    velocity = (velocity_right + velocity_left) / 2;
    omega = (velocity_right - velocity_left) / robot_length;

    % Pose dt
    pose_dot = [...
        velocity * cos(pose(3)), ...
        velocity * sin(pose(3)), ...
        omega];

    % Update pose
    pose = pose + pose_dot * time_delta;
end

i = 1;
figure(i);
scatter(trajectory(:, 1), trajectory(:, 2));
