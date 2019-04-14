# confide: A CLI utility that syncs config files

* api: HTTP JSON API written with Node, Express, and MongoDB
* cli: C++ utility written with libcurl and [nlohmann/json](https://github.com/nlohmann/json)

## Usage

First, run the API server:

```
cd api
npm install
npm run start
```

Then, to run the CLI:

```
cd cli
make
./client
```

### API Dependencies

* bcrypt - password hashing
* express - webserver
* moment - date formatting
* mongodb - database (requires mongodb server to be installed)
* mongoose - modeling for MongoDB
* node and npm

### CLI Dependencies

* libcurl - interacting with API
* [nlohmann/json](https://github.com/nlohmann/json) - JSON object interaction (.hpp included in cli/)

### CLI Options

* LOG/REG
    * LOG: login to an existing user profile
    * REG: create a new user
* LS: list files associated with the currently logged in user
* VC: view contents of a file
* DL: Download a config file from the server
* UP: Upload a config file to the server. If the file has not been uploaded before, a profile for the config will be created
* EXIT: Exit CLI

## API Structure

* GET /listconfs/:json: List names of configs associated with a user profile
    * Input: {username, password}
    * Output: {result, [name]}
* GET /config/:json: Get info associated with a config profile
    * Input: {username, password, name}
    * Output: {result, name, created, modified, content}
* POST /addconfig: Add a config to a user's profile. If a config with that name already exists, update its contents
    * Input: {username, password, name, path, content}
    * Output: {result}
* POST /deleteconfig: Delete a config from a user's profile
    * Input: {username, password, name}
    * Output: {result}
* POST /newuser: Create a new user profile
    * Input: {username, password, email}
    * Output: {result}
* GET /validuser/:json: Check if a user with a specific username and password exists
    * Input: {username, password}
    * Output: {result}

## Tools Used

* Postman - for testing API calls
* Robo3T - for interacting with MongoDB
* Vim, VS Code - editors
