# Testfälle DrehgeberPolling

Zu prüfen:

1. Funktionen schreiben LEDs
    - Zähler LEDs gehen an bei Vorwärtsrichtung :white_check_mark:
    - Richtungs LEDs Wechseln bei Richtungswechsel :white_check_mark:
    - Bei Error Leuchtet nur Error :white_check_mark:
2. Grenzfälle prüfen
    - Bei LED Werten über 8 Bits kommt es zu keinem Nebeneffekt  :white_check_mark:
    - Der Overflow/Underflow von Grad ist gelöst. :x:
    - Bei zu ungültigen Drehungen kommt es zu einem Fehler :white_check_mark:
    - Der Timer Overflow für 32bit ist gelöst :white_check_mark:
3. Zeitanalye
    - Eine Standardloop ohne Display Ausgabe dauert im Schnitt eine micro Sekunde
    - Die Expirmentell Ermittelte Geschwindigkeit ist 250 Grad/s