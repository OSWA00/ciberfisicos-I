function [x_vec, y_vec, theta_vec] = Control(L, dt, kt, kr, x, y, theta, x_desired, y_desired, noise)
    x_error = x - x_desired;
    y_error = y - y_desired;    

    delta = 0.01;

    x_vec = [x];
    y_vec = [y];
    theta_vec = [theta];

    while (x_error < -0.6 || x_error > 0.6) || (y_error < -0.6 || y_error > 0.6) 
        
        % Update errors
        x_error = x - x_desired;
        y_error = y - y_desired;
        theta_desired = atan2(y_desired - y, x_desired - x);
        theta_error = theta - theta_desired;
        delta_error = delta + theta_error;

        v = kt * sqrt(x_error ^ 2 + y_error ^ 2);
        phi = - kr * delta_error;

        % Saturation
        if v > 0.5
            v = 0.5;
        end

        if phi > pi/16
            phi = pi/16;
        end

        if phi < -pi/16
            phi = -pi/16;
        end

        x_noise = -noise + (noise + noise) * rand();


        % Kinematic model
        x_dot = v * cos(theta);
        y_dot = v * sin(theta);

        delta_dot = phi;
        theta_dot = v * tan(delta) / L;

        % Update states
        x = x + x_dot * dt + x_noise;
        y = y + y_dot * dt;
        delta = delta + delta_dot * dt;
        theta = theta + theta_dot * dt;

        x_vec(end + 1) = x;
        y_vec(end + 1) = y;
        theta_vec(end + 1) = theta;
    end
end