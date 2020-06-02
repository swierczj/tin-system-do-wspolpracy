# tin-system-do-wspolpracy
Repozytorium projektu Techniki Internetowe realizowanego w semestrze letnim 2019/20

## Zespół
-Michał Makoś
-Jan Prugarewicz
-Jakub Świerczyński
-Bartłomiej Wolski

## Moduły - etap 1
Na potrzeby tego etapu zostały stworzone trzy podstawowe aplikacje klienckie i jedna serwerowa. Zapewniają one jednynie połączenie klient-serwer i przesył losowych wiadomości.


#### Klient desktopowy
Aplikacja napisana w Javie. Działa na 3 oddzielnych wątkach: reader, writer i keep-alive.<br>
* reader odpowiada za asynchroniczne czytanie danych z socketu.<br>
* writer czyta strumień wejścia z klawiatury i wysyła dane przez socket<br>
* keep-alive co 15 sekund wysyła wiadomość "Am I alive?" i jeśli w przeciągu 5 sekund nie uzyska odpowiedzi to informuje o zerwanym połączeniu. <br><br>

Argumenty wywołania:
1. Ip serwera (opcjonalny - domyślnie localhost)
2. Port (opcjonalny - domyślnie 54000)

#### Klient webowy 
NIe został ukończony
#### Klient administratorski
Jest to prosty skrypt w Pythonie uruchamiany poleceniem:
```
 Python3 client_main.py
```
Wymagany jest Python3.#. Jako, że jest to etap jeszcze testowania to część rzeczy jest zahardkodowana czy nawet momentami bez sensu. 

#### Serwer
Serwer otrzymuje dane od użytkowników i przesyła dalej innym użytkownikom połączonym z serwerem. Działa na zasadzie pokoju rozmów. Można uruchomić poprzez napisany Makefile, właściwe pliki znajdują się w folderze server na branchu o tej samej nazwie.




