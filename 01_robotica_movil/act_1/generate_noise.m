function noise = generate_noise(vec_size, seed)
%GENERATE_NOISE Summary of this function goes here
%   Detailed explanation goes here
noise = zeros(1, vec_size);
noise(:, 1) = seed;
min = -0.5;
max = 0.5;
for i=2:vec_size
    r = min + (max + max) * rand();
    noise(:, i) = noise(:, i - 1) + r; 
end

