%% Reading file
file = fopen("out.txt","r");
if (file == -1)
    disp("Couldn't open file")
else
    data = textscan(file, '%d %d %d %d');
    fclose(file);
end
% Data processing
IDENTIFIERS = double(data{1});
PIXELS = unique(double(data{2}));
PIXMEM = double(data{3}); 
BOOLOPS = double(data{4});

PIXMEM_AND = PIXMEM(IDENTIFIERS == 1);
PIXMEM_AND2 = PIXMEM(IDENTIFIERS == 2);

%% Plot
f1 = figure(1);
plot(PIXELS,PIXMEM_AND, "-")
hold on
plot(PIXELS,PIXMEM_AND2, "-")
hold off
title("COMPUTACIONAL")
legend("AND","AND2", Location="northwest")%% Data process
xlabel("SIZE")
ylabel("PIXMEM")

set(f1, "Position",[0 0 1000 300])

%% Exporting
exportgraphics(f1, 'figures/out.pdf');

%%
quit
