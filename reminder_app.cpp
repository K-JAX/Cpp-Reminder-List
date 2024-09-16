#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <algorithm>
#include <fstream>

// mysql headers
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

using namespace std;

// Function to load configuration file into a map
map<string, string> loadConfig(const string& filename) {
    ifstream file(filename);
    map<string, string> config;
    string line;

    while (getline(file, line)) {
        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != string::npos) {
            string key = line.substr(0, delimiter_pos);
            string value = line.substr(delimiter_pos + 1);
            config[key] = value;
        }
    }

    return config;
}

sql::Connection* connectToDatabase() {
  try {
      // Load the configuration file
      map<string, string> config = loadConfig("db_config.txt");

      sql::mysql::MySQL_Driver *driver;
      sql::Connection *con;

      // Establish connection using the loaded config
      driver = sql::mysql::get_mysql_driver_instance();
      con = driver->connect("tcp://" + config["host"], config["user"], config["password"]);
      con->setSchema(config["database"]);

      return con;

  } catch (sql::SQLException &e) {
      cerr << "SQL Error: " << e.what() << endl;
      exit(1);
  }
  return nullptr;
}


// add
void add( sql::Connection* con, const string item ) { 
  cout << endl;

  if (item.empty()){
    cout << "Please specify an item to add." << endl << endl;
    return;
  }

  try{
    // Prepare the SQL statement
    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement("INSERT INTO reminders (reminder_text) VALUES (?)");

    pstmt->setString(1, item);

    pstmt->executeUpdate();

    delete pstmt;
  } catch (sql::SQLException &e) {
    cerr << "SQL Error: " << e.what() << endl;
  }
  cout << endl;
}

// remove
void remove( sql::Connection* con, const int id = -1 ) { 
  cout << endl;

  // if id is not provided
  if(id == -1){
    cout << "Please specify an id to remove." << endl << endl;
    return;
  }
  try{
    sql::PreparedStatement *pstmt;
    // this is my attempt just from researching
    pstmt = con->prepareStatement("DELETE FROM reminders WHERE id = ?");

    pstmt->setInt(1, id);

    int affectedRows = pstmt->executeUpdate();
    if(affectedRows > 0){
      cout << "Reminder with id " << id << " was successfully removed." << endl;
    } else {
      cout << "No reminder found with id " << id << endl;
    }

    delete pstmt;

  }catch(sql::SQLException &e){
    cerr << "SQL Error: " << e.what() << endl;
  } 
  cout << endl;
}

// view
void view( sql::Connection* con ) {
  // Prepare the SQL statement
  sql::PreparedStatement *pstmt;
  pstmt = con->prepareStatement("SELECT id, reminder_text, due_date FROM reminders");

  sql::ResultSet* res = pstmt->executeQuery();

  if(!res->next()){
    cout << "No reminders found." << endl;
  } else{
    cout << endl;
    cout << "------ Reminders ------" << endl;
    do{

      int id = res->getInt("id");
      string reminder_text = res->getString("reminder_text");
      string due_date = res->getString("due_date");
      cout << "Reminder: " << reminder_text << " | id: " << id << endl;

    } while(res->next());
  }

  // clean up resources
  delete res;
  delete pstmt;

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
  sql::Connection* con = connectToDatabase();
  welcomeMessage();

  vector<string> shoppingList = { "Apples", "Bananas", "Peaches" };
  
  unordered_map<string, function<void(const string&)>> commands = {
    {"add", [&con](const string& item) { add( con, item ); }},
    {"help", [](const string&) { help(); }},
    {"remove", [&con](const string& id_str) { 
        try {
            int id = stoi(id_str);  // Convert the string to an int
            remove(con, id); 
        } catch (const invalid_argument&) {
            cout << "Invalid ID format. Please enter a valid number." << endl;
        }
    }},
    {"view", [&con](const string&) { view( con ); }},
    {"quit", [](const string&) { quit(); }}
  };
  getCommandInput(commands);

  if(con != nullptr){
    delete con;
    cout << "Database connection closed." << endl;
  }
}

int main() {
  initPrompt();
  return 0;
}
