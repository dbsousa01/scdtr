clear all; close all; clc;


%add = 'COM3';
add = '/dev/ttyACM0';

samples = 500;

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