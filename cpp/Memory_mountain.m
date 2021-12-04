clc, clear

data = load('result.txt');

x = data(:, 1); % ������
y = data(:, 2); % ����
z = data(:, 3); % ������

x1 = 1 : 18;
y1 = 1 : 12;
z1 = griddata(x, y, z, x1',y1,'linear');

meshc(x1, y1, z1);