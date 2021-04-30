

PROJECT = mqttexp
ZIP = n-xfabom01-xkeprt03.zip
SRC=src/

build: clean
	qmake $(SRC)src.pro -o $(SRC)Makefile
	$(MAKE) -C $(SRC)
	mv $(SRC)$(PROJECT) $(PROJECT)

run: build
	./$(PROJECT)

clean:
	rm -r -f icp $(SRC)*.o $(SRC)Makefile $(SRC)src $(SRC)moc_* $(SRC)ui_mainwindow.h doc/*
	
pack: clean
	zip -r $(ZIP) $(SRC) examples/ doc/ Makefile README.*

doxygen:
	doxygen doc/Doxyfile
	