


%生成中心为0，离均差为1.8205的离散半高斯分布
% x = 0:1:18;
% pd2 = makedist('HalfNormal','mu',0,'sigma',1.8205);
% y0 = pdf(pd2,x);  
% y0 = y0/sum(y0);
% plot(x,y0,'r');
% legend('HalfNormalDistribution with (mu,sigma)=(0,1.8205)');



%生成中心为0，离均差为1.7的离散高斯分布
x = -18:1:18;
y0 = normpdf(x,0,1.7);  
y0 = y0/sum(y0);
plot(x,y0,'r');
ylim([0,0.3])
legend('NormalDistribution with (mu,sigma)=(0,1.7)');


