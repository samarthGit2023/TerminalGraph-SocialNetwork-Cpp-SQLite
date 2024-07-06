#include<bits/stdc++.h>
#include"sqlite3.h"  
using namespace std;


string curr_username="";
string curr_password="";


const char* dbFile = "social_network.db";

const char* createUsersTableSQL = 
    "CREATE TABLE IF NOT EXISTS users ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT NOT NULL,"
    "email TEXT,"
    "age INTEGER,"
    "bio TEXT,"
    "username TEXT UNIQUE NOT NULL,"
    "password TEXT NOT NULL"
    ");";

const char* createConnectionsTableSQL = 
    "CREATE TABLE IF NOT EXISTS connections ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "username_1 TEXT NOT NULL,"
    "username_2 TEXT NOT NULL,"
    "FOREIGN KEY (username_1) REFERENCES users(username),"
    "FOREIGN KEY (username_2) REFERENCES users(username)"
    ");";



class User {
private:
    string name;
    string email;
    int age;
    string bio;
    string username;
    string password;

public:
    // Constructor
    User(const string& name, const string& email, int age, const string& bio,
         const string& username, const string& password)
        : name(name), email(email), age(age), bio(bio), username(username), password(password) {}

    // Methods
    //void createAccount();
    bool authenticate(const string& username, const string& password);
    void viewProfile();
    void updateProfile();
    void showAllUsers();
    void searchUserByUsername();
    void showFollowers();
    void showFollowing();
};


void createTablesIfNeeded() {
    sqlite3* db;
    char* errMsg = nullptr;
    int rc;

    rc = sqlite3_open(dbFile, &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // Execute SQL statements to create tables
    rc = sqlite3_exec(db, createUsersTableSQL, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (users table): " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    rc = sqlite3_exec(db, createConnectionsTableSQL, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (connections table): " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}


int callback(void* data, int argc, char** argv, char** azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << endl;
    return 0;
}


bool credentials(const string& username, const string& password) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    const char* sql = "SELECT * FROM users WHERE username=? AND password=?;";
    rc = sqlite3_open(dbFile, &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return false;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return true; // Username and password match found
    } else {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false; // Username and password match not found
    }
}


int createAccount(){
    string name, email, bio, username, password;
    int age;
    cout << "Enter name: ";
    cin>>name;
    cout << "Enter email: ";
    cin>>email;
    cout << "Enter age: ";
    cin >> age;
    cout << "Enter bio: ";
    cin>>bio;
    cout << "Enter username: ";
    cin>>username;
    cout << "Enter password: ";
    cin>>password;

    // SQLite operations to insert user into database
    sqlite3* db;
    char* errMsg = 0;
    string sql;

    int rc = sqlite3_open(dbFile, &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return 0;
    }

    // Create table if not exists
    sql = "CREATE TABLE IF NOT EXISTS users ("
          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
          "name TEXT NOT NULL,"
          "email TEXT NOT NULL,"
          "age INTEGER NOT NULL,"
          "bio TEXT,"
          "username TEXT UNIQUE NOT NULL,"
          "password TEXT NOT NULL"
          ");";

    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (creating table): " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    sql = "INSERT INTO users (name, email, age, bio, username, password) VALUES ('" +
          name + "', '" + email + "', " + to_string(age) + ", '" + bio + "', '" + username + "', '" + password + "');";

    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);
    int f=0;
    if (rc != SQLITE_OK) {
        cerr << "SQL error (inserting data): " << errMsg << endl;
        sqlite3_free(errMsg);
    } 
    else {
        cout << "Account created successfully!" << endl;
        f=1;
    }
    

    sqlite3_close(db);
    return f;
}


int Authenticate(){
    cout<<"Enter username : ";
    string usrnm;
    cin>>usrnm;
    cout<<"Enter password : ";
    string passwrd;
    cin>>passwrd;
    if(credentials(usrnm, passwrd)==true){
        curr_username=usrnm;
        curr_password=passwrd;
        return 0;
    }
    else{
        return 4;
    }
}


void viewProfile() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    // SQL query to select user profile details based on username and password
    const char* sql = "SELECT * FROM users WHERE username=? AND password=?;";

    rc = sqlite3_open(dbFile, &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, curr_username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, curr_password.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Fetch and display user profile details
        cout << "Profile Details:" << endl;
        cout << "----------------" << endl;
        cout << "Name: " << sqlite3_column_text(stmt, 1) << endl; // Assuming column indices
        cout << "Email: " << sqlite3_column_text(stmt, 2) << endl;
        cout << "Age: " << sqlite3_column_int(stmt, 3) << endl;
        cout << "Bio: " << sqlite3_column_text(stmt, 4) << endl;
        // Add more fields as needed

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    } else {
        cout << "Profile not found for current credentials." << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
}


void updateProfile() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    // Variables to store user input
    string field;
    string newValue;

    // Prompt user for field to update
    cout << "Choose field to update (name, age, email, bio): ";
    cin >> field;

    // Validate input and prompt for new value
    if (field == "name" || field == "age" || field == "email" || field == "bio") {
        cout << "Enter new value for " << field << ": ";
        cin.ignore(); // Ignore newline character
        getline(cin, newValue);
    } else {
        cout << "Invalid field choice." << endl;
        return;
    }

    // Construct SQL query with the chosen field
    string updateSql = "UPDATE users SET " + field + "=? WHERE username=? AND password=?;";
    const char* updateStmt = updateSql.c_str();

    rc = sqlite3_open(dbFile, &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    rc = sqlite3_prepare_v2(db, updateStmt, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare update statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // Bind parameters to the prepared statement
    sqlite3_bind_text(stmt, 1, newValue.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, curr_username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, curr_password.c_str(), -1, SQLITE_STATIC);

    // Execute the update statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Error updating profile: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Profile updated successfully!" << endl;
    }

    // Finalize statement and close database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}


void showAllUsers() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    // SQL query to select all users
    const char* sql = "SELECT name, age, email, bio FROM users;";

    rc = sqlite3_open(dbFile, &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // Execute the query and fetch results
    cout << "All Users:" << endl;
    cout << "----------" << endl;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int age = sqlite3_column_int(stmt, 1);
        const char* email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* bio = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        // Print user details
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "Email: " << email << endl;
        cout << "Bio: " << bio << endl;
        cout << "----------------" << endl;
    }

    if (rc != SQLITE_DONE) {
        cerr << "Error retrieving data: " << sqlite3_errmsg(db) << endl;
    }

    // Finalize statement and close database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}


void followUser(sqlite3* db, const string& userToFollow) {
    //sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    const char* checkSql = "SELECT COUNT(*) FROM connections WHERE username_1=? AND username_2=?;";
    rc = sqlite3_prepare_v2(db, checkSql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare check statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // Bind parameters to the prepared statement
    sqlite3_bind_text(stmt, 1, curr_username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, userToFollow.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        cerr << "Failed to check existing connection: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    if (count > 0) {
        cout << "You are already following user: " << userToFollow << endl;
        return;
    }


    const char* sql = "INSERT INTO connections (username_1, username_2) VALUES (?, ?);";

    // rc = sqlite3_open(dbFile, &db);
    // if (rc) {
    //     cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
    //     sqlite3_close(db);
    //     return;
    // }
    // SQL query to insert into connections table
    

    
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        //sqlite3_close(db);
        return;
    }

    // Bind parameters to the prepared statement
    sqlite3_bind_text(stmt, 1, curr_username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, userToFollow.c_str(), -1, SQLITE_STATIC);

    // Execute the insert statement
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        cout << "You are now following user: " << userToFollow << endl;
    } 
    else {
        cerr << "Error following user: " << sqlite3_errmsg(db) << endl;
        
    }

    // Finalize statement and close database connection
    sqlite3_finalize(stmt);
    //sqlite3_close(db);
}


void searchUser() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    // Variables to store user input
    string searchUsername;
    string choice;

    // Prompt user to enter username to search
    cout << "Enter username to search: ";
    cin >> searchUsername;

    // SQL query to select user details based on username
    const char* sql = "SELECT name, age, email, bio FROM users WHERE username=?;";

    rc = sqlite3_open(dbFile, &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // Bind parameter to the prepared statement
    sqlite3_bind_text(stmt, 1, searchUsername.c_str(), -1, SQLITE_STATIC);

    // Execute the query and fetch results
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Fetch and display user details
        cout << "User Details:" << endl;
        cout << "-------------" << endl;
        cout << "Name: " << sqlite3_column_text(stmt, 0) << endl;
        cout << "Age: " << sqlite3_column_int(stmt, 1) << endl;
        cout << "Email: " << sqlite3_column_text(stmt, 2) << endl;
        cout << "Bio: " << sqlite3_column_text(stmt, 3) << endl;

        // Prompt user for action
        cout << "Do you want to follow this user? (yes/no): ";
        cin >> choice;

        if (choice == "yes") {
            followUser(db, searchUsername);
        } 
        else if (choice == "no") {
            // Do nothing, continue with search options
        } 
        else {
            cout << "Invalid choice. Exiting search." << endl;
        }
    } 
    else {
        cout << "User not found." << endl;
    }

    // Finalize statement and close database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}


void userFollowers() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    // Vector to store usernames of followers
    vector<string> followers;

    // SQL query to select followers of curr_username from connections table
    const char* sql = "SELECT u.name, u.age, u.email, u.bio "
                      "FROM connections c "
                      "JOIN users u ON c.username_1 = u.username "
                      "WHERE c.username_2 = ?;";

    rc = sqlite3_open(dbFile, &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // Bind parameter to the prepared statement
    sqlite3_bind_text(stmt, 1, curr_username.c_str(), -1, SQLITE_STATIC);
    cout << "--------------------------" << endl;
    // Execute the query and fetch results
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int age = sqlite3_column_int(stmt, 1);
        const char* email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* bio = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        // Display details of the follower
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "Email: " << email << endl;
        cout << "Bio: " << bio << endl;
        cout << "--------------------------" << endl;
    }

    if (rc != SQLITE_DONE) {
        cerr << "Error retrieving data: " << sqlite3_errmsg(db) << endl;
    }

    // Finalize statement and close database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}


void userFollowing() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    // Vector to store usernames of users whom curr_username follows
    vector<string> following;

    // SQL query to select users whom curr_username follows from connections table
    const char* sql = "SELECT u.name, u.age, u.email, u.bio "
                      "FROM connections c "
                      "JOIN users u ON c.username_2 = u.username "
                      "WHERE c.username_1 = ?;";

    rc = sqlite3_open(dbFile, &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // Bind parameter to the prepared statement
    sqlite3_bind_text(stmt, 1, curr_username.c_str(), -1, SQLITE_STATIC);
    cout << "--------------------------" << endl;
    // Execute the query and fetch results
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int age = sqlite3_column_int(stmt, 1);
        const char* email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* bio = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        // Display details of the user whom curr_username follows
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "Email: " << email << endl;
        cout << "Bio: " << bio << endl;
        cout << "--------------------------" << endl;
    }

    if (rc != SQLITE_DONE) {
        cerr << "Error retrieving data: " << sqlite3_errmsg(db) << endl;
    }

    // Finalize statement and close database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}


void viewMutualFriends() {
    sqlite3* db;
    string otherUser;
    cin>>otherUser;
    sqlite3_stmt* stmt;
    int rc;

    // SQL query to find mutual friends
    const char* sql = R"(
        SELECT u.username, u.name, u.age, u.email, u.bio
        FROM users u
        JOIN connections c1 ON u.username = c1.username_2
        JOIN connections c2 ON u.username = c2.username_2
        WHERE c1.username_1 = ? AND c2.username_1 = ?
    )";

    rc = sqlite3_open(dbFile, &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // Bind parameters to the prepared statement
    sqlite3_bind_text(stmt, 1, curr_username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, otherUser.c_str(), -1, SQLITE_STATIC);

    // Execute the query and fetch results
    cout << "Mutual Friends:" << endl;
    cout << "---------------" << endl;
    rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW) {
        cout << "Username: " << sqlite3_column_text(stmt, 0) << endl;
        cout << "Name: " << sqlite3_column_text(stmt, 1) << endl;
        cout << "Age: " << sqlite3_column_int(stmt, 2) << endl;
        cout << "Email: " << sqlite3_column_text(stmt, 3) << endl;
        cout << "Bio: " << sqlite3_column_text(stmt, 4) << endl;
        cout << "---------------" << endl;
        rc = sqlite3_step(stmt);
    }

    // Finalize statement and close database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}




void SocialNetwork(){
    while(true){
        cout<<"======================================"<<endl;
        cout<<"1. View My Profile"<<endl;
        cout<<"2. Update My Profile"<<endl;
        cout<<"3. Show All Users"<<endl;
        cout<<"4. Search User"<<endl;
        cout<<"5. Followers"<<endl;
        cout<<"6. Following"<<endl;
        cout<<"7. View mutual Friends"<<endl;
        cout<<"8. Exit"<<endl;
        cout<<"Enter your choice : ";
        int x;
        cin>>x;
        if(x==1){
            cout<<endl;
            viewProfile();
            cout<<endl;
        }
        else if(x==2){
            cout<<endl;
            updateProfile();
            cout<<endl;
        }
        else if(x==3){
            cout<<endl;
            showAllUsers();
            cout<<endl;
        }
        else if(x==4){
            cout<<endl;
            searchUser();
            cout<<endl;
        }
        else if(x==5){
            cout<<endl;
            userFollowers();
            cout<<endl;
        }
        else if(x==6){
            cout<<endl;
            userFollowing();
            cout<<endl;
        }
        else if(x==7){
            cout<<endl;
            viewMutualFriends();
            cout<<endl;
        }
        else if(x==8){
            break;
        }
        cout<<"======================================"<<endl;
        cout<<endl;
    }
}


int main(){
    createTablesIfNeeded();
    cout<<"_________________________________________________________________________"<<endl;
    cout<<endl;
    cout<<"WELCOME TO THE LOCAL SOCIAL NETWORKING"<<endl;
    cout<<"1. Create Account"<<endl;
    cout<<"2. Login"<<endl;
    cout<<"3. Exit"<<endl;
    cout<<"Enter your choice : ";
    int x;
    cin>>x;
    if(x==1){
        int f=createAccount();
        if(f==0){
            cout<<"_________________________________________________________________________"<<endl;
            cout<<endl;
            return 0;
        }
    }
    else if(x==2){
        while(Authenticate()==4){
            cout<<"Wrong Credentials"<<endl;
            int ii;
            cout<<"If you wanna exit, type 99"<<endl;
            cin>>ii;
            if(ii==99){
                cout<<"_________________________________________________________________________"<<endl;
                cout<<endl;
                return 0;
            }
        }
        SocialNetwork();
    }
    else{
        cout<<"_________________________________________________________________________"<<endl;
        cout<<endl;
        return 0;
    }
    cout<<"_________________________________________________________________________"<<endl;
     cout<<endl;
    return 0;
}