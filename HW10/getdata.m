
data1=filters('COM14');
data=cell2mat(data1);
plot(data(2,:))
hold on
plot(data(3,:))
plot(data(4,:))
plot(data(5,:))
legend('raw','MAF','IIR','FIR')
ylabel('z-acc');
xlabel('Sample times');  