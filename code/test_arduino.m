clear all; close all; clc;


%add = 'COM3';
<<<<<<< bd14ccce57c30e552373f93508f8ef595ea5641a
add = '/dev/ttyACM0';

samples = 500;
=======
add = '/dev/ttyS101';

samples = 50;
>>>>>>> Matlab communication and pwm plots

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

<<<<<<< bd14ccce57c30e552373f93508f8ef595ea5641a
if exist('serie1') %Fecha descritores da porta série anteriores
    fclose(serie1);
    delete(serie1);
    clear serie1;
end
=======
% if exist('serie1') %Fecha descritores da porta série anteriores
%     fclose(serie1);
%     delete(serie1);
%     clear serie1;
% end
>>>>>>> Matlab communication and pwm plots

%Inicializa a porta série
serie1 = serial(add);
set(serie1, 'BaudRate', 115200);
<<<<<<< bd14ccce57c30e552373f93508f8ef595ea5641a

fopen(serie1); %Abre a porta série
pause(3); %Espera que o Arduino inicie
flushinput(serie1); %Limpa o buffer da porta série

AR = zeros(1,samples);
PWM = zeros(1,samples);

for i = 1 : samples
    
    AR(i) = fread(serie1);
    PWM(i) = fread(serie1);

end

figure
plot(AR);

figure
plot(PWM);
=======
serie1.ReadAsyncMode = 'continuous';

fopen(serie1); %Abre a porta série
%pause(3); %Espera que o Arduino inicie
flushinput(serie1); %Limpa o buffer da porta série
readasync(serie1);
while(serie1.BytesAvailable <= 0)  %wait until Arduino outputs data 
end
i = 1;
AR = zeros(samples,1);
PWM = zeros(samples,1);
while (i <= samples)
     
     AR(i) = fscanf(serie1,'%f'); %read sensor
     PWM(i) = fscanf(serie1,'%f');
     
     flushinput(serie1);
     
     i = i + 1;
    
end

% voltage on LDR

vldr = 5-((AR*5)./1023);

figure
plot(AR);
% 
figure
plot(PWM);

figure
plot(vldr);
% close the serial port!
fclose(serie1);
>>>>>>> Matlab communication and pwm plots
