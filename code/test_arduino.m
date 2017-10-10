clear all; close all; clc;

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end
arduino=serial('COM3', 'BaudRate', 9600); % create serial communication object 
fopen(arduino);

read = fread(arduino);
var = median(read);

fwrite(arduino,var,uint8(var));