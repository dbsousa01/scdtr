clear all; close all; clc;


%add = 'COM3';
add = '/dev/ttyS101';

samples = 400;


if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end


if exist('serie1') %Fecha descritores da porta série anteriores
    fclose(serie1);
    delete(serie1);
    clear serie1;
end


%Inicializa a porta série
serie1 = serial(add);
set(serie1, 'BaudRate', 115200);

serie1.ReadAsyncMode = 'continuous';

fopen(serie1); %Abre a porta série
%pause(3); %Espera que o Arduino inicie
flushinput(serie1); %Limpa o buffer da porta série
readasync(serie1);
while(serie1.BytesAvailable <= 0)  %wait until Arduino outputs data 
end
i = 1;



while (i <= samples)
     
     %string(i) = cellstr(fscanf(serie1));
     
     
%      AR(i) = fscanf(serie1,'%f'); %read sensor
%      PWM(i) = fscanf(serie1,'%f');
     
     Voltage(i) = readDigitalPin(serie1,'D10');
     flushinput(serie1);
     
     i = i + 1;
    
end

% voltage on LDR

% vldr = 5-((AR*5)./1023);
% 
% AR = str2double(AR);
% PWM = str2double(PWM);
% figure
% plot(AR);
% % 
% figure
% plot(PWM);
% 
% figure
% plot(vldr);
% % close the serial port!
figure
plot(Voltage);
fclose(serie1);
% 
