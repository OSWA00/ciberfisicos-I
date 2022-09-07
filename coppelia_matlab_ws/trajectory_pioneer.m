clear;
clc;
clf;

% Setup remote connection
sim=remApi('remoteApi');
sim.simxFinish(-1);
clientID=sim.simxStart('127.0.0.1', 19999, true, true, 5000, 5);

if (clientID>-1)
    disp('Coppelia connected');

    % Coppelia handlers
    [~, left_motor]=sim.simxGetObjectHandle(clientID,...
    'Pioneer_p3dx_leftMotor',sim.simx_opmode_blocking);

    [~, right_motor]=sim.simxGetObjectHandle(clientID,...
    'Pioneer_p3dx_rightMotor',sim.simx_opmode_blocking);

    [~, pioneer_block]=sim.simxGetObjectHandle(clientID,...
    'Pioneer_p3dx',sim.simx_opmode_blocking);
    
    [~, target_block]=sim.simxGetObjectHandle(clientID,...
    'Cuboid0',sim.simx_opmode_blocking);

    % Robot pose %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    [~, rotational_pose] = sim.simxGetObjectOrientation( ...
        clientID,pioneer_block, -1, sim.simx_opmode_streaming);

    [~, traslational_pose] = sim.simxGetObjectPosition(clientID, ...
        pioneer_block,-1, sim.simx_opmode_streaming);

    % Robot kinematic parameters
    len = 0.19;
    
    % Simulation parameters
    time = 0;
    total_time = 2;
    delta_time = 0.1;

    % Controller gains
    kt = 10;
    kr = 20;

    % Set initial velocities to 0
    [~] = sim.simxSetJointTargetVelocity(clientID,left_motor,0.0,...
    sim.simx_opmode_blocking);

    [~] = sim.simxSetJointTargetVelocity(clientID,right_motor,0.0,...
    sim.simx_opmode_blocking);

    % Initial kinematics
    x = traslational_pose(1);
    y = traslational_pose(2);
    theta = rotational_pose(3);

    % Target
    i = 1;
    x_desired = sin(x:0.1:2);
    y_desired = sin(y:0.1:2);
    path_size = size(x_desired);

    x_sim = [x];
    y_sim = [y];
    theta_sim = [theta];

    % Error
    x_error = x - x_desired(i);
    y_error = y - y_desired(i);
    theta_desired = atan2(y_desired(i) - y, x_desired(i) - x);
    theta_error = theta  - theta_desired;

    
    % Simulation
    while time < total_time
        
        % Control
        v = kt * sqrt(x_error ^ 2 + y_error ^ 2);
        omega = -kt * theta_error;

        % Saturation
        if v > 1
            v = 1;
        end

        if omega > pi / 2
            omega = pi / 2;
        end

        if omega < -pi / 2
            omega = -pi / 2;
        end

        vel_right = v + 0.5 * len * omega;
        vel_left = v + 0.5 * len * omega;

        [~] = sim.simxSetJointTargetVelocity(clientID, ...
            left_motor,vel_left,... 
            sim.simx_opmode_blocking);

        [~] = sim.simxSetJointTargetVelocity(clientID, ...
            right_motor,vel_right, ...
            sim.simx_opmode_blocking);
        
        % Feedback robot pose
        [~, rotational_pose] = sim.simxGetObjectOrientation( ...
            clientID,pioneer_block, -1, sim.simx_opmode_streaming);
    
        [~, traslational_pose] = sim.simxGetObjectPosition(clientID, ...
            pioneer_block,-1, sim.simx_opmode_streaming);

        x = traslational_pose(1);
        y = traslational_pose(2);
        theta = rotational_pose(3);

        x_sim(end + 1) = x;
        y_sim(end + 1) = x;
        theta_sim(end + 1) = x;

        % Calculate error
        if i < path_size(2)
            i = i + 1;
        end
        

        x_error = x - x_desired(i);
        y_error = y - y_desired(i);
        theta_desired = atan2(y_desired(i) - y, x_desired(i) - x);
        theta_error = theta  - theta_desired;

        time = time + delta_time;
        pause(0.1);

    end

    [returnCode] = sim.simxSetJointTargetVelocity(clientID,left_motor,0,...
    sim.simx_opmode_blocking);

    [returnCode] = sim.simxSetJointTargetVelocity(clientID,right_motor,0,...
    sim.simx_opmode_blocking);
    disp('Copellia connection closed');
    sim.simxFinish(clientID);
end
sim.delete(); 

figure(1)
scatter(x_sim, y_sim);
hold on;
scatter(x_desired, y_desired, 'filled');
