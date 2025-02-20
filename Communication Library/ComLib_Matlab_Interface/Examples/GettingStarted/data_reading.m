% Impostare porta seriale 
serialPort = "COM3"; 
baudRate = 115200;   

% Aprertura connessione seriale
s = serialport(serialPort, baudRate);

configureTerminator(s, "LF"); 
flush(s); % Pulisce il buffer iniziale

% Crea la finestra
fig = uifigure('Name', 'Radar Data', 'Position', [500, 500, 300, 200]);

% Crea label per mostrare il numero
lbl = uilabel(fig, 'Text', '---', 'FontSize', 48, ...
              'HorizontalAlignment', 'center', ...
              'Position', [50, 70, 200, 50]);

% Loop per aggiornare il valore in tempo reale
while isvalid(fig)  % Continua finché la finestra è aperta
    try
        % Legge la linea dalla seriale 
        data = readline(s);
        % Converte in numero 
        value = str2double(data);
        % Aggiorna il testo della label
        if ~isnan(value)
            lbl.Text = num2str(value);
        end
    catch
        % Se c'è un errore interrompere
        break;
    end
    pause(0.1); 
end

% Chiudere la connessione alla chiusura della finestra
clear s;
