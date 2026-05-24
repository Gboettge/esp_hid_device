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
   

# 3. Tidsplan (plannering)
### Vecka 1:
- Planera för hur etableringen av uppkoppling ska gå till
- Val av utvecklings miljö ***Arduino IDE*** eller ***Espressif IDE*** via VScode?
- Läsa dokummentation på bluetooth uppkoppling.                      
### Vecka 2:
- Koppla samman all hårvara för esp32, 
- Lyckas etablera en bluetooth kontakt
### Vecka 3:
- Koda ***snake spel*** 
- Testa enheten i alla dessa states så garanterat resultat kan leveraras.
 
# 4. Valda metoder
## *Material:*
```
3x knappar
1x led-lampa
1x ESP32c6
Broadboard & sladdar för koppling av enhet


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
Bluetooth kontrollen har tre knappar, en knapp som står för reset vid disconnect.
Två knappar som skickar data i form at hexadecimala tal ex. 0x05 & 0x06 (som tolkas som "a" respektive "b"). En binär-ledlampa som reagerar på bluetooth connection status genom att hämta värdet via en funktionen **ble_state_t ble_hid_get_state(void);**

ESP32:an är flashad med en main som inkuderar 3 "komponenter" av c & h filer som gör föjande:
- __Button__ - Sköter logik vid knapptryck, så som att ta bort debounce mm. samt ha färdiga callback funktioner som kan kallas från ex. main. Skickar hexdecimala strängar med värde vid knapptryck samt skickar tomma strängar vid knappsläpp för att bluetooth enheten ska få rätt beteende.
- __Binary_LED__ - ställer in ledlampas beteende via funktioner som b_led_blink & b_led_setled(b, BL_LIGHT_ON). Samt implementerar ändringar genom funktionen b_led_update.


# 6. Arbetsprocess
Första halvan av projekts tiden gick år primärt att läsa både dokummentation och fakta blad via Espressif IDF (***ESP32 officiella hemsida för hur man kodar på ESP32:or (ESP-IDF)***). 

Då jag väl kom igång med projektet så valde jag att jobba med **vscode** och därmed **ESP-IDF**. Detta valde jag istället för att jobba med Arduiono IDE att använda mig av då jag dels har erfarenhet av den samt en tanke av att använda tidigare kodkomponenter som jag gjorde i ett tidigare kodprojekt vilket var mina .c och .h filer för ***button*** och ***binary_led***. 

Detta val gav mig möjligheten att gå in helhjärtat på det jag inte gjort förut vilket var att skapa en bluetooth HID (***Human Interface Device***). 

Det som satte mest käppar i hjulet för mig var då jag fick för mig att man kunde använda sig av en sk. "bluedroid pheriferal server" för att kunna etablera kontakt och kännas igen som en klassisk bluetooth.

Detta sidospår kostade mig ca 4 arbetsdagar att jobba med bluetooth server, detta för att en bluetooth server kan när det är fungerande och etablerad kännas igen som en bluetooth enhet men kunde dock **inte** efterlikna en gamepad eller keyboard för live inputs.

Valet att övergå till en HID enhet hade kunnat spara mig mycket tid, och jag bara hade kommit i kontakt med rätt källa eller tolkat dokummentationen bättre.

### Hur jag har kodat projektet. 
***Main.c*** egenskriven C kod med hjälpfunktioner från **Button, binary_led & bluetooth_hid.**

**Binary_led & Button** egenskriven C kod från första skolåret med logik som kan köras i en minimal main med hjälp av att kalla på update funktioner i en loop.

**Bluetooth_HID** Kodad i C och ***delvist "vibe-coded"*** för att fixa logik som att jag råkade försöka skicka data tillbaka till avsändaren istället för mottagaren. Detta hann jag tyvärr inte lösa helt utan hjälp av AI eftersom det gick åt så mycket tid åt snestegen med min **bluetooth pheriferal server**.

### På Rasberry Pi (vscode):
***

# 7. Analys & Slutsats
Arbetsproccessen skiljde sig mer än vad jag tänkte mig i planneringen (***3. Tidsplan (plannering***). Detta var primärt för att bl

# 8. Förbättringsmöljigheter slutprodukt











