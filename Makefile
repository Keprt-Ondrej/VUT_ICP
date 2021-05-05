

PROJECT = mqttexp
ZIP = 1-xfabom01-xkeprt03.zip
SRC=src/

build:
	qmake $(SRC)src.pro -o $(SRC)Makefile
	$(MAKE) -C $(SRC)
	mv $(SRC)$(PROJECT) $(PROJECT)

run: build
	./$(PROJECT)

clean:
	rm -r -f $(SRC)*.o $(SRC)Makefile $(SRC)moc_* $(SRC)ui_mainwindow.h doc/* $(PROJECT) src/src.pro.user.*
	
pack: clean
	zip -r $(ZIP) $(SRC) examples/ doc/ Makefile README.txt Doxyfile

doxygen:
	doxygen 
	
