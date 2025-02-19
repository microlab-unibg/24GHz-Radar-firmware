% Imposta la porta seriale (modifica 'COM3' con la tua porta)
serialPort = "COM3"; % Su Mac/Linux potrebbe essere '/dev/ttyUSB0'
baudRate = 115200;   % Imposta il baud rate corretto

% Apri la connessione seriale
s = serialport(serialPort, baudRate);

% Imposta il timeout e la terminazione delle linee se necessario
configureTerminator(s, "LF"); % Cambia secondo il tuo dispositivo
flush(s); % Pulisce il buffer iniziale

% Crea la finestra
fig = uifigure('Name', 'Radar Data', 'Position', [500, 500, 300, 200]);

% Crea un'etichetta per mostrare il numero
lbl = uilabel(fig, 'Text', '---', 'FontSize', 48, ...
              'HorizontalAlignment', 'center', ...
              'Position', [50, 70, 200, 50]);

% Loop per aggiornare il valore in tempo reale
while isvalid(fig)  % Continua finché la finestra è aperta
    try
        % Legge la linea dalla seriale (modifica se il formato è diverso)
        data = readline(s);
        % Converte in numero (se necessario)
        value = str2double(data);
        % Aggiorna il testo della label
        if ~isnan(value)
            lbl.Text = num2str(value);
        end
    catch
        % Se c'è un errore (es. disconnessione) interrompi
        break;
    end
    pause(0.1); % Piccola pausa per non sovraccaricare il processore
end

% Chiudi la connessione alla chiusura della finestra
clear s;
