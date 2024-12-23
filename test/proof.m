% Número de simulações
num_simulacoes = 1000000;

size = 100;

% Gerar variáveis aleatórias entre 1 e size
variavel1 = randi([1, size], size, num_simulacoes);
variavel2 = randi([1, size], size, num_simulacoes);

% Somar as variáveis e obter o valor por linhas de runs
somas = sum(variavel1 + variavel2, 1) ./ size;

% Plot
%%
f1 = figure(1);

histogram(somas, "EdgeColor","green");
title('média = 100.9');
xlabel('Soma');
ylabel('Frequência');


% Coisas extras para exportar o gráfico
%%
set(f1, "Position",[0 0 400 300])

%%
exportgraphics(f1, "figures/out.pdf", "Append", true)
