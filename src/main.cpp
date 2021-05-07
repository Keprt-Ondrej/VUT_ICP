#include "shared.h"

#include "hello.cpp"
#include <QApplication>
#include "mainwindow.h"
#include "connectserver.h"

Shared *activeShared;

int main(int argc, char *argv[]) { 
	/* level00
	 *  | topic0
	 *  |
	 *  + level10
	 *  |  | topic2
	 *  |  | topic3
	 *  |
	 *  + level11
	 *  |  + level20
	 *  |  |  | topic4
	 *  |  |
	 *  |  | topic5
	 *  |
	 *  | topic1
	 *  | topic6
	 */
	std::string t0 = "10";
	std::string t1 = "True";
	std::string t2 = "420";
	std::string t3 = "69";
	std::string t4 = "dxfcgvbhjnmkl";
	std::string t5 = "fthn";
	TypedItem topic0("device", TOPIC, t0);
	TypedItem topic1("something_else", TOPIC, t1);
	TypedItem topic2("blazeit", TOPIC, t2);
	TypedItem topic3("nice", TOPIC, t3);
	TypedItem topic4("cghjk", TOPIC, t4);
	TypedItem topic5("jhgytr78uoikl", TOPIC, t5);
	TypedItem topic6("asdfg", TOPIC, t5);

	std::vector<Item*> main_v;
	std::vector<Item*> lvl10;
	std::vector<Item*> lvl11;
	std::vector<Item*> lvl20;

	lvl20.push_back(&topic4);
	TypedItem level20("level2", LEVEL, lvl20);

	lvl11.push_back(&level20);
	lvl11.push_back(&topic5);
	TypedItem level11("other_level1", LEVEL, lvl11);

	lvl10.push_back(&topic2);
	lvl10.push_back(&topic3);
	TypedItem level10("level1", LEVEL, lvl10);

	main_v.push_back(&topic0);
	main_v.push_back(&level10);
	main_v.push_back(&level11);
	main_v.push_back(&topic1);
	TypedItem level00("", LEVEL, main_v);

	std::cout << "==========================\n";
	tree_print_recursive(&level00);
	std::cout << "==========================\n";

	level00.data.push_back(&topic6);

	Shared oof(level00);
	tree_print_recursive(oof.get_topics());
    std::cout << "==========================\n";

	Shared x;
	tree_print_recursive(x.get_topics());
    std::cout << "==========================\n";
	
	QList<QVariant> x1;
    for(int i=0; i<5; i++){
		x1.append((double)i);
	}

    QVariant x2 = x1;

	
	QList<QVariant> karel = x2.toList();
	karel.push_front((double)73);
	x2 = karel;
	
     
    QList<QVariant> x3 = x2.toList();
    for(int i=0; i<x3.size(); i++){
        std::cout << x3.at(i).toDouble() << "\n";
	}


	Shared active(level00);
	activeShared = &active; //potrebuji nejaky shared navazat jinak SIGSEG
	hi();
		
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

  	return a.exec();
}
