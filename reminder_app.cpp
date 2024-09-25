#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <algorithm>
#include <fstream>

// mysql headers
#include <mysqlx/xdevapi.h>

using namespace std;
using namespace mysqlx;

// Function to load configuration file into a map
map<std::string, std::string> loadConfig(const std::string& filename) {
    ifstream file(filename);
    map<std::string, std::string> config;
    std::string line;

    while (getline(file, line)) {
        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);
            config[key] = value;
        }
    }

    return config;
}

// Establish the session and return a Session object
Session* connectToDatabase() {
    try {
        // Load the configuration file
        map<std::string, std::string> config = loadConfig("db_config.txt");

        // Establish connection using the loaded config
        Session* session = new Session(config["host"], stoi(config["port"]), config["user"], config["password"]);
        session->sql("USE " + config["database"]).execute();  // Use the correct database

        return session;

    } catch (const Error &err) {
        cerr << "Connection error: " << err << endl;
        exit(1);
    }
    return nullptr;
}

// add
void add( Session* session, const std::string item ) { 
  cout << endl;

  if (item.empty()){
    cout << "Please specify an item to add." << endl << endl;
    return;
  }

  try{
    // Prepare the SQL statement
    session->sql("INSERT INTO reminders (reminder_text) VALUES (?)")
      .bind(item)
      .execute();

    cout << "Added reminder: " << item << endl;
  } catch (const Error &err) {
    cerr << "SQL Error: " << err << endl;
  }
  cout << endl;
}

// remove
void remove(Session* session, const int id) {
    cout << endl;

    if(id == -1){
        cout << "Please specify a valid ID to remove." << endl << endl;
        return;
    }

    try {
        // Execute the delete query
        mysqlx::SqlResult result = session->sql("DELETE FROM reminders WHERE id = ?")
                                .bind(id)
                                .execute();

        if (result.getAffectedItemsCount() > 0) {
            cout << "Removed reminder with id " << id << endl;
        } else {
            cout << "No reminder found with id " << id << endl;
        }

    } catch (const Error &err) {
        cerr << "SQL Error: " << err << endl;
    }
    cout << endl;
}

// view
void view(Session* session) {
    try {
        RowResult res = session->sql("SELECT id, reminder_text, due_date FROM reminders").execute();

        if (!res.count()) {
            cout << "No reminders found." << endl;
        } else {
            cout << endl << "------ Reminders ------" << endl;
            for (Row row : res) {
                int id = row[0];
                std::string reminder_text = row[1].get<std::string>();
                cout << "Reminder: " << reminder_text << " | id: " << id << endl;
            }
        }

    } catch (const Error &err) {
        cerr << "SQL Error: " << err << endl;
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

std::string toLowerCase(const std::string& str){
  std::string lowerStr = str;
  transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
  return lowerStr;
}

std::string trim(const std::string& str) {
  size_t firstNonSpace = str.find_first_not_of(' ');
  // trim leading
  if(firstNonSpace == std::string::npos){
    return "";
  }

  // trim trailing whitespace
  size_t lastNonSpace = str.find_last_not_of(' ');
  return str.substr(firstNonSpace, (lastNonSpace - firstNonSpace + 1));
}

void handleCommand(const std::string& input, unordered_map<std::string, function<void( const std::string& )>>& commands){

  if(input.empty()){
    return;
  }

  std::string trimmedInput = trim(input);

  std::string command, argument;
  size_t spaceIndex = trimmedInput.find(' ');

  if(spaceIndex != std::string::npos){
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

void getCommandInput(unordered_map<std::string, function<void(const std::string&)>>& commands) {
  std::string input;
  while( true ){
    cout << "Enter command:";
    getline(cin, input);

    handleCommand(input, commands);

  }
}

void initPrompt() {
  Session* session = connectToDatabase();
  welcomeMessage();

  vector<std::string> shoppingList = { "Apples", "Bananas", "Peaches" };
  
  unordered_map<std::string, function<void(const std::string&)>> commands = {
    {"add", [&session](const std::string& item) { add( session, item ); }},
    {"help", [](const std::string&) { help(); }},
    {"remove", [&session](const std::string& id_str) { 
        try {
            int id = stoi(id_str);  // Convert the std::string to an int
            remove(session, id); 
        } catch (const invalid_argument&) {
            cout << "Invalid ID format. Please enter a valid number." << endl;
        }
    }},
    {"view", [&session](const std::string&) { view( session ); }},
    {"quit", [](const std::string&) { quit(); }}
  };
  getCommandInput(commands);

  delete session;
  cout << "Database connection closed." << endl;
}

int main() {
  initPrompt();

  // try {
  //     // Simplified connection
  //     Session sess("104.248.226.236", 33060, "reminder_user", "Secure_Password123!!");
  //     cout << "Connected successfully!" << endl;
  //     sess.close();
  // } catch (const mysqlx::Error &err) {
  //     cerr << "Connection error: " << err.what() << endl;
  // } catch (std::exception &ex) {
  //     cerr << "STD Exception: " << ex.what() << endl;
  // } catch (...) {
  //     cerr << "Unknown exception caught!" << endl;
  // }
  return 0;
}
