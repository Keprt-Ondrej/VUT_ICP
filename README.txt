Projekt do předmětu Programování v jazyce C++

MQTT explorer/dashboard

zadání: zadani.txt nebo https://wis.fit.vutbr.cz/FIT/st/cwk.php.cs?title=Projekt_20202021&csid=724883&id=13964 

Autoři:
Matúš Fabo (xfabom01@stud.fit.vutbr.cz)
Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)

Spustitelný soubor se jmenuje mqttexp


Implementace:

Program nebere žádné argumenty a neukládá žádné soubory.

MQTT Klient je udělán ručně a když je příliš mnoho příchozích paketů, tak spadne (například když se odobírají všechny topicy na testovacích servrech)(když se ale spustí přes valgrind, pak nepadá)

Simulátor je zakomponovaný do normálního klienta a jenom posíla zprávy s výjimkou obrázků (obrázky ale lze posílat)

Dashboard se nám nepodařil automaticky obnovovat, tak se musí ručně obnovit tlačítkem update.

Vše ostatní by mělo jít podle zadání.
