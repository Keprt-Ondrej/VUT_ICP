Projekt do předmětu Programování v jazyce C++

MQTT explorer/dashboard

zadání: https://wis.fit.vutbr.cz/FIT/st/cwk.php.cs?title=Projekt_20202021&csid=724883&id=13964

Autoři:
Matúš Fabo (xfabom01@stud.fit.vutbr.cz)
Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)

Spustitelný soubor se jmenuje mqttexp


Implementace:

Program nebere zadny argumenty a neuklada/nenacte zadny soubory (krome souboru, ktere se posilaji brokeru)

MQTT Klient je delan rucne a kdyz je prilis mnoho prichozich paketu, tak spadne (napriklad kdyz se odoberaji vsechny topicy na testovacich servrech)(kdyz se ale spusti pres valgrind, pak nepada)

Simulator je zakomponovany do normalniho klienta a jenom posila spravy s vynimkou obrazku (obrazky ale lze posilat)

Dashboard se nam nepodaril automaticky obnovovat, tak se musi rucne obnovit tlacidlem update.

Vse ostatni by melo jit podle zadani.