clear;
clc;
clf;

% Setup remote connection
sim=remApi('remoteApi');
sim.simxFinish(-1);
clientID = sim.simxStart('127.0.0.1', 19998, true, true, 5000, 5);

if (clientID>-1)
    disp('Coppelia connected');

    [~, target] = sim.simxGetFloatSignal(clientID,'target',sim.simx_opmode_streaming);

    disp('Copellia connection closed');
    sim.simxFinish(clientID);
end
disp("Success")
sim.delete(); 
