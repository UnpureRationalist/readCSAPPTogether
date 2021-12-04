clc, clear

n = 1000; % n 次测试
m = 150; % 每次分别测试 m 个元素

data = load('result5.txt'); 

CPE_total = zeros(m, n);
CPE_mean = zeros(m, 1);
CPE_sort = zeros(m, n);

for i = 1 : m
    CPE_total(i, :) = data(i : m : m * n, 2);
end

for i = 1 : m
    CPE_sort(i, :) = sort(CPE_total(i, :));
    CPE_mean(i) = sum(CPE_sort(i, 1 : n - 50)) / (n - 50); % 默认在 1000 次测试中，去掉结果最大的 50 次
end

x = data(1, 1) : m + data(1, 1) - 1;
cftool(x, CPE_mean)