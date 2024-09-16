#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <algorithm>

using namespace std;

// add
void add( vector<string>& shoppingList, const string item ) { 
  cout << endl;
  if (item.empty()){
    cout << "Please specify an item to add." << endl << endl;
    return;
  }

  // check for duplicates
  if(find(shoppingList.begin(), shoppingList.end(), item) != shoppingList.end()){
    cout << "Item already in the list: " << item << endl;
    return;
  }
  cout << "Adding item - " << item << endl << endl;
  shoppingList.push_back(item);
}

// remove
void remove( vector<string>& shoppingList, const string item) {
  cout << endl;
  // int cnt = count(shoppingList.begin(), shoppingList.end(), item);
  auto it = find(shoppingList.begin(), shoppingList.end(), item);
  if(it != shoppingList.end()){
    cout << "Removing item - " << item << endl << endl;
    shoppingList.erase(it);
  }else{
    cout << "Item not found in list, check spelling :)" << endl << endl;
  }

}

// view
void view( const vector<string>& shoppingList ) {
  cout << endl;
  cout << "------ Shopping List ------" << endl;
  if(shoppingList.empty()){
    cout << "The list is empty." << endl;
  }else{
    int itemNumber = 1;
    for (const string& i : shoppingList){
      cout << itemNumber << ". " << i << endl;
      ++itemNumber;
    }
  }
  cout << endl;
}

// quit
void quit(){
  cout << "Quitting..." << endl << endl;
  exit(0);
}

// help
void help() {
  cout << "See below commands:" << endl;
  cout << "add - adds an item to the list" << endl;
  cout << "help - see list of available commands." << endl;
  cout << "remove - removes an item from the list" << endl;
  cout << "view - see the entire list" << endl;
}

void welcomeMessage(){
  cout << "Welcome to Shopping List Plus Plus!" << endl;
  cout << "What would you like to do? Type help to see commands" << endl;
}

string toLowerCase(const string& str){
  string lowerStr = str;
  transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
  return lowerStr;
}

string trim(const string& str) {
  size_t firstNonSpace = str.find_first_not_of(' ');
  // trim leading
  if(firstNonSpace == string::npos){
    return "";
  }

  // trim trailing whitespace
  size_t lastNonSpace = str.find_last_not_of(' ');
  return str.substr(firstNonSpace, (lastNonSpace - firstNonSpace + 1));
}

void handleCommand(const string& input, unordered_map<string, function<void( const string& )>>& commands){

  if(input.empty()){
    return;
  }

  string trimmedInput = trim(input);

  string command, argument;
  size_t spaceIndex = trimmedInput.find(' ');

  if(spaceIndex != string::npos){
    command = trimmedInput.substr(0, spaceIndex);
    argument = trimmedInput.substr(spaceIndex + 1);
  } else{
    command = trimmedInput;
    argument = "";
  }
  // convert command to lowercase
  command = toLowerCase(command);
  argument = trim(argument);

  if(commands.find(command) != commands.end()){
    commands[command](argument);
  } else {
    cout << "Invalid command: '" << command << "'. Type 'help' to see available commands." << endl;
  }
}

void getCommandInput(unordered_map<string, function<void(const string&)>>& commands) {
  string input;
  while( true ){
    cout << "Enter command:";
    getline(cin, input);

    handleCommand(input, commands);

  }
}

void initPrompt() {
  welcomeMessage();

  vector<string> shoppingList = { "Apples", "Bananas", "Peaches" };
  
  unordered_map<string, function<void(const string&)>> commands = {
    {"add", [&shoppingList](const string& item) { add( shoppingList, item ); }},
    {"help", [](const string&) { help(); }},
    {"remove", [&shoppingList](const string& item) { remove( shoppingList, item ); }},
    {"view", [&shoppingList](const string&) { view( shoppingList ); }},
    {"quit", [](const string&) { quit(); }}
  };
  getCommandInput(commands);
}

int main() {
  initPrompt();
  return 0;
}
