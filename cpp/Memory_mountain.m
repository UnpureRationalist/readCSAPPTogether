clc, clear

data = load('result.txt');

x = data(:, 1); % 数据量
y = data(:, 2); % 步长
z = data(:, 3); % 吞吐量

x1 = 1 : 18;
y1 = 1 : 12;
z1 = griddata(x, y, z, x1',y1,'linear');

meshc(x1, y1, z1);