#include "shared.h"
#include "mainwindow.h"

std::pair<std::string,std::string> getPath(std::string path){    
    std::string delimiter = "/";
    std::string name = path.substr(0, path.find(delimiter)); // name is "scott"
    path.erase(0, path.find(delimiter) + delimiter.length());
    return std::make_pair(name, path);
}

void Shared::modelSetup(){
	dataModel = new QStandardItemModel();
}

void Shared::insertTopic(std::string path,bool received,data_type_t dataType,QVariant data){
	return;
}

const std::string& Item::get_name(){
	return name;
}

type_t Item::get_type(){
	return type;
}


Shared::~Shared(){
}

void Shared::cmd_append(cmd_t cmd){
	command.push_back(cmd);
}

void Shared::data_out_append(const std::string& data){
	data_out.push_back(data);
}

void Shared::data_recv_append(const std::string& data){
	data_recv.push_back(data);
}


void Shared::setConnectionInfo(std::string protocol,std::string host,int port,std::string username,std::string password){
	this->protocol = protocol;
	this->host = host;
	this->port = port;
	this->username = username;
	this->password = password;	
}

TypedItem<std::vector<Item*>>* Shared::get_topics(){
	return &topics;
}

void tree_print_recursive(Item* item){
	if(item == NULL) return;

	static std::string path = "";

	if(item->get_type() == TOPIC){
		TypedItem<std::string>* tmp = static_cast<TypedItem<std::string>*>(item);
		std::cout << path << tmp->get_name() << " = " << tmp->data << std::endl;
	}
	else if(item->get_type() == LEVEL){
		path += item->get_name() + "/";
		std::vector<Item*> tmp_list = static_cast<TypedItem<std::vector<Item*>>*>(item)->data;
		for(size_t i = 0; i < tmp_list.size(); i++){
			tree_print_recursive(tmp_list[i]);
		}
		path.erase(path.end()-item->get_name().length()-1, path.end());

	}
	else{
		std::cout << "Unrecognized item " << item->get_name() << std::endl;
	}
}

void test() {
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

	Shared a(level00);
	tree_print_recursive(a.get_topics());
	std::cout << "==========================\n";

	Shared x;
	tree_print_recursive(x.get_topics());
	std::cout << "==========================\n";
}

