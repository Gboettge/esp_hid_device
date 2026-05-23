# 1. Indledning


# 2. Ursprunglig projektplan
## ___Projektbeskrivning Wireless Snake game___
### Bakgrund
Uppgiften görs i primärt syfte för att kunna etablera trådlös kontakt mellan olika typer av IoT-enheter. Där en eller två ESP32:or ska agera trådlösa handkontroller till en Raspberry Pi som har spelet Snake.

Jag har valt detta projekt för att jag tror att det dels kommer att vara roligt att genomföra men också bra att kunna visa upp i framtida arbetsintervjuer.
### Syfte/mål
Målet är att kunna skicka två olika typer av inputs från ESP32 som ska kunna tolkas av en Rasberry Pi. Kommunikationsprotokollen som jag har tänkt att använda är följande.
Wifi (Enklast)
Bluetooth (Svårast)
Resultatet ska vara lite likt en Nintendo Switch men fungerande trådlösa handkontroller.

### Avgränsningar och fokus
Eftersom fokus ligger på att ha kontakt och fungerande handkontroller så blir den potentiella avgränsningen själva snake spelet. Om jag inte hinner med att programmera spelet så kommer jag att ladda ner det till min Raspberry Pi så att jag iallafall kan påvisa kontakten mellan enheterna.
   

# 3. Tidsplan
 
# 4. Valda metoder
## *Material:*
```
3x knappar
1x led-lampa
1x ESP32c6
1x Raspberry Pi
```
## Vilken kod gör vad
***Kod skriven i tidigare projekt*** Dessa har en enkel och tydlig stuktur som passar bra med [RTOS](Real time operating system) av olika slag.
- Binary_LED.c & Binary_LED.h 
- Button.c & button.h 
## *Utvecklingsmiljö och kodspråk*
```
Kod bluetooth controll(esp32): C
Vscode viritual enviroment och rasberry pi: BASH commands
Programmerings språk Raspberry Pi: Python
```
## Enhet som ska söka efter handkontrollen **BASH-commands**
#### *bluetooth kommandon:*
- __bluetoothctl__    - Öppnar bluetooth configuration på i terminalen
- __scan on__ - Listar bluetooth enheter och dess ***MAC-adress***
- __pair MAC__ - Paraihop enhet
- __info MAC__ - Visa device egenskaper så som ex. keyboard, name mm.
- __remove MAC__ - Ta bort parad enhet
- __connect MAC__ - Koppla till parad enhet
- __disconnect MAC__ - Koppla bort från parad enhet
- __quit__ - gå ur bluetooth menyn

#### *kommandon för att hitta rådata samt inputs från device*
- __ls /dev/input__ - visar alla typer av enheter som finns kopplade ex. event1-X eller mouse 0-x osv
- __cat  /dev/input/eventX__ - visar rådata eller input av vald enhet
- __sudo btmon__ - visar bluetooth inputs från olika enheter. bra snabbcheck


# 5. Hur fungerar enheten.

# 6. Arbetsprocess / Faktisk tidslinje

# 7. Förbättrningsmöjligheter











