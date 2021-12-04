clc, clear

n = 1000; % n �β���
m = 150; % ÿ�ηֱ���� m ��Ԫ��

data = load('result5.txt'); 

CPE_total = zeros(m, n);
CPE_mean = zeros(m, 1);
CPE_sort = zeros(m, n);

for i = 1 : m
    CPE_total(i, :) = data(i : m : m * n, 2);
end

for i = 1 : m
    CPE_sort(i, :) = sort(CPE_total(i, :));
    CPE_mean(i) = sum(CPE_sort(i, 1 : n - 50)) / (n - 50); % Ĭ���� 1000 �β����У�ȥ��������� 50 ��
end

x = data(1, 1) : m + data(1, 1) - 1;
cftool(x, CPE_mean)