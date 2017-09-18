%Créditos: Francisco Marcolino Rodrigues Filho
%Disciplina: Controle Digital 2017
%Data: 09 de maio de 2017
%Professor: Otacílio da Mota Almeida
%CÓDIGO MATLAB PARA REALIZAR O CONTROLE DA TENSÃO DE SAÍDA DO CONVERSOR BOOST
%Limpar o Workspace
clc;
%Deletar todas as variáveis no Workspace
clear;
%Fechar todas as Janelas
close all;


%###########################################################################
%criar objeto porta serial
%Verificar em Gerenciador de Dispositivos
s = serial('COM22','BaudRate',19200);
%Ajustar o caractere final para '\n'
s.terminator = 'LF';

%Verificar as portas abertas pelo MATLAB
out = instrfind(s);
if(out.status == 'closed')
    fclose(instrfind);
    %Abrir a comunicação Serial
    fopen(s);  
end
%############################################################################


%Ajustar o período de amostragem
Ts = 0.1;

% 0 para a Enviar DUTY (Entre 0 e 255)
% D para somente enviar DUTY (Entre 0 e 255)
% Z para somente zerar Duty
% T para somente ler tensão (max de 1023*53.76 mV)
% P para desligar relé da fonte
% L ativar relé da fonte

flushinput(s);
flushoutput(s);
%Zerar pwm
fprintf(s,  sprintf( 'Z' ));
%Ligar relé da fonte
fprintf(s,  sprintf( 'L' ));
Tempo = [];
saida = [];
entrada = [];
%delay 'Diferente' com etime() (mais preciso que pause())
tt = clock;
while etime(clock, tt) < 5
%não fazer nada enquanto o não passar de 5 segundos (pode deletar, se quiser
end

%FECHANDO A MALHA
DUTY = 40; %70 por cento de duty
REF = 20000;
Tempo_grafico = 10; %Tempo do gráfico em Segundos
figure(1);
t0 = clock;
k = 1;
d_max =  80/100*(255); % 80 % duty pwm max
d_min =  10/100*(255); % 10 % duty pwm min

entrada(k) = d_max; % Cerca de 58,8% da largura do Pulso.
eps = 10;

%LOOP CONTROLE PLANTA BOOST
%AQUI PODE-SE APLICAR DIVERSOS TIPOS DE CONTROLE.
while etime(clock, t0)< Tempo_grafico
    tt = clock;
    %configurar pwm
    flushinput(s);
    flushoutput(s);
	
	% Entrada do processo (Aqui você pode controlar a largura do pulso no instante k)
	% Duty mínimo -> 0		(0%)
	% Duty máximo -> 255	(100%)
	
    %SATURAÇÃO (DUTY máximo de 80 %)
    if(entrada(k) > 80/100*(255))
        entrada(k) =  80/100*(255);
    else
        if(entrada(k)<0)
           entrada(k) = 0; 
        end
    end
	%Comandos para enviar o valor do duty para o microcontrolador (3 bytes)
    fwrite(s,'0','uint8');
    fwrite(s,uint8(entrada(k)),'uint8'); %Envia valor de Duty para o Motor (entre 0 e 255)
    fwrite(s,10,'uint8');  %Código quebra de linha é necessário
    resposta = fread(s,2); %
    %Saída do processo
    saida(k) = double(bitor(bitshift(uint16(resposta(1)),8),uint16(resposta(2))));
    erro(k) = REF - saida(k);
    
    %Construção de um relé
    if(abs(erro(k)) > eps)
        %mudar o status do rele
        if(erro(k) >= 0)
           % erro positivo - ref maior que a saída
           entrada(k+1) = d_max;
        else
           % erro negativo - saída maior que ref 
           entrada(k+1) = d_min;
        end
    else
        %Caso contrário, continuar com a mesma entrada
        entrada(k+1) = entrada(k);
    end
    
    plot(saida,'LineWidth',2);
    hold on;
    plot(entrada,'r','LineWidth',2);
    drawnow
    while etime(clock, tt) < Ts
    %não fazer nada enquanto o tempo de amostragem não terminar
    end
    k = k + 1;
end
 
%Zerar pwm
fprintf(s,  sprintf( 'Z' ));
%Desligar relé da fonte
fprintf(s,  sprintf( 'P' ));

Tempo = [1:(k-1)]*Ts;
plot(saida,'LineWidth',2);
hold on;
plot(entrada,'r','LineWidth',2);
drawnow
xlabel('Tempo (s)');
ylabel('Tensão (mV)');

fclose(s);


%save('pqfile.mat', 'ref', 'saida');

