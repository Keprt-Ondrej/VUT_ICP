#ifndef SHARED_H
#define SHARED_H

#include "mainwindow.h"
#include <iostream>
#include <vector>
#include <QtCore>
#include <QtGui>

enum type_t{LEVEL, TOPIC};
enum cmd_t{POSLAT, ODEBRAT}; //enum cmd_t{PUBLISH, SUBSCRIBE}; 
enum data_type_t{JSON,STRING,BIN};

/*
 * QStandardItem:
 * vsechny datove polozky jsou typu QVariant
 * data[0] = topic name
 * data[3] = bool data nebo topic/level
 * data[4] = zdroj 
 * data[5] = data_type_t
 * data[6] = QList<QVariant> data
 * data[7] = fullpath
*/

class Item{
	public:
		Item(const std::string& item_name, const type_t item_type)
			: name(item_name), type(item_type){}

		const std::string&	get_name();
		type_t 				get_type();

	protected:
		const std::string name;
		const type_t type;
};

template<typename T>
class TypedItem : public Item{
	public:
		TypedItem(const std::string& item_name, const type_t item_type, T& item_data)
			: Item(item_name, item_type), data(item_data){}
		TypedItem(const std::string& item_name, const type_t item_type)
			: Item(item_name, item_type){}

		T data;
};

class Shared{
	public:
		Shared(TypedItem<std::vector<Item*>>& topics_item)
			: topics(topics_item.get_name(), LEVEL, topics_item.data){}
		Shared()
			: topics("", LEVEL){}
		~Shared();

		void cmd_append(cmd_t cmd);
		void data_out_append(const std::string& data);
		void data_recv_append(const std::string& data);
		TypedItem<std::vector<Item*>>* get_topics();
        void setConnectionInfo(std::string protocol,std::string host,int port,std::string username,std::string password);
		void modelSetup();
		void insertTopic(std::string path,bool received,data_type_t dataType,QVariant data);
	protected:
		QStandardItemModel* dataModel;
		std::string protocol = "mqt:://";
		std::string host = "test.mosquitto.org";
		int port = 1883;
		std::string username = "";
		std::string password = "";	
	
		std::vector<cmd_t> command;
		std::vector<std::string> data_out;
		std::vector<std::string> data_recv;
		TypedItem<std::vector<Item*>> topics;
		// pridat mutexy ked sa implementuju thready
};

void tree_print_recursive(Item* item);
extern Shared *activeShared;
#endif // SHARED_H
