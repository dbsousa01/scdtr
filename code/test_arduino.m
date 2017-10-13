clear all; close all; clc;


%add = 'COM3';
<<<<<<< HEAD
add = '/dev/ttyS101';

samples = 400;

=======
<<<<<<< bd14ccce57c30e552373f93508f8ef595ea5641a
add = '/dev/ttyACM0';

samples = 500;
=======
add = '/dev/ttyS101';

samples = 50;
>>>>>>> Matlab communication and pwm plots
>>>>>>> 71fbe107862603bd92a1173a10560080a1d00726

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

<<<<<<< HEAD

=======
<<<<<<< bd14ccce57c30e552373f93508f8ef595ea5641a
>>>>>>> 71fbe107862603bd92a1173a10560080a1d00726
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

serie1.ReadAsyncMode = 'continuous';

fopen(serie1); %Abre a porta série
%pause(3); %Espera que o Arduino inicie
flushinput(serie1); %Limpa o buffer da porta série
readasync(serie1);
while(serie1.BytesAvailable <= 0)  %wait until Arduino outputs data 
end
i = 1;
AR = cell(samples,1);
PWM = cell(samples,1);
while (i <= samples)
     
     string = fscanf(serie1);
     [AR(i), PWM(i)] = strsplit(string, ';');
     
%      AR(i) = fscanf(serie1,'%f'); %read sensor
%      PWM(i) = fscanf(serie1,'%f');
     
     flushinput(serie1);
     
     i = i + 1;
    
end

% voltage on LDR

vldr = 5-((AR*5)./1023);

AR = str2double(AR);
PWM = str2double(PWM);
figure
plot(AR);
% 
figure
plot(PWM);

figure
<<<<<<< HEAD
plot(vldr);
% close the serial port!
fclose(serie1);

=======
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
>>>>>>> 71fbe107862603bd92a1173a10560080a1d00726
