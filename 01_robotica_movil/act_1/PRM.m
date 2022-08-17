function [path, map] = PRM()
    figure()
    BinaryMap = makemap(1000);
    map = binaryOccupancyMap(rot90(transpose(BinaryMap)), 1000/10);
    show(map)

    nodes = 100;
    tic
    planner = mobileRobotPRM(map,nodes);
    toc
    figure()
    show(planner)

    startLocation = [0.0 0.0];
    endLocation = [10, 10];
    tic
    path = findpath(planner, startLocation, endLocation);
    toc
    figure()
    show(planner)
end