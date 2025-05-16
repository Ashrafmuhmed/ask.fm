#include<bits/stdc++.h>

using namespace std ;

struct user{

    string username, email, password, id; bool allow_annon;

    user(){
        username = email = password = {""}, allow_annon = {0} ;
    }

    bool read_user()
    {

        string _username , _email , _password ; bool _allow ;

        cout << "Enter your username, email, password (separated by spaces) : ";
        cin >> _username >> _email >> _password  ;
        cout << "Do you allow annonymous messages ( 1 -> yes | 0 -> no ) :  " ;
        cin >> allow_annon ;
        if(valid_username(_username)
           && valid_email(_email)
           && valid_password(_password)){
            username = _username , email = _email , password = _password , allow_annon = _allow ;
            return true ;
           }
           return false ;

    }

    int generate4DigitID() {
        ifstream in("ids.txt");
        vector<int>ids ;
        string id ;
        while(getline(in , id)){
            try{
                ids.push_back(stoi(id));
            }catch(...){
                return -1 ;
            }
        }
        int ii ;
        while(true)
        {
            mt19937 rng(std::random_device{}());
            uniform_int_distribution<int> dist(0, 9999);
            ii = dist(rng) ;
            auto i = find(ids.begin(),ids.end(), ii) ;
            if( i == ids.end()) break ;
        }

        return ii ;

    }

    bool valid_username(string&_name){
        for(auto&c : _name){
            if( (c >='a' && c<='z')
               || (c>='A' && c <='Z') ) return true ;
        }
        return false ;
    }

    bool valid_email(string&_email){
        auto i = _email.find('@') ;
        auto lh =  _email.substr(i) ;
        return lh.find('.') != string::npos ;
    }

    bool valid_password(string&_password)
    {
        bool found_num = false, found_alph = false ;
        for(auto&c : _password)
        {
            if( (c >='a' && c<='z')
               || (c>='A' && c <='Z') ) found_alph = true ;
            else if( c >= '1' && c <= '9' ) found_num = true ;
        }
        return (found_alph && found_num) ;
    }

    bool add_to_fs()
    {
        ofstream out_stream("users.txt",ios::app), outstream_ids("ids.txt",ios::app);
        if(!out_stream.is_open()||outstream_ids.fail()) return false ;
        int id = generate4DigitID() ;
        out_stream<< id << " " << username << " " << email << " " << password << " " << allow_annon <<  endl ;
        outstream_ids << id << endl ;
        return true ;
    }

};

struct question{

};

struct fs_system{

    template<typename t>
    vector<t> fetch_data( string path )
    {
        vector<t>data ;
        ifstream in_stream(path) ;
        string line;
        while(getline(in_stream,line))
            data.push_back(line) ;
        return data ;
    }

    user search_user(string _name , string _pass)
    {
        vector<user> users ;
        vector<string> data = fetch_data<string>("users.txt") ;
        for( auto&c : data)
        {
            user u ;
            istringstream iss(c) ;
            iss >> u.id >> u.username >> u.email >> u.password >> u.allow_annon ;
            if( u.username == _name && _pass == u.password ) return u ;
        }
        user u ;
        return u ;
    }

};

struct session{

    user current_user ;

    session(){}

    session(user _user)
    {
        current_user = _user ;
    }

};


void ask_fm() ;
void initial_menu();
void login();
void signup() ;
int main()
{
    ask_fm();
}

void ask_fm()
{
    while(1) initial_menu() ;
}

void initial_menu()
{
    int choice ;
    cout << "Menu :\n 1. Login\n 2.Sign Up\n Enter your choice : " ;
    cin >> choice ;
    if(choice == 1){
        login();
    }else if(choice == 2){
        signup();
    }
}

void login(){
    string name , password ;
    cout << "Enter username and password : " ;
    cin >> name >> password ;
    fs_system fs ;
    user current_user = fs.search_user(name , password);
    cout << "You are currently logged in as " << current_user.username << endl ;
}

void signup(){
    user reg_user ;
    if(reg_user.read_user())
    {
        if(reg_user.add_to_fs())
            cout << "Registered!!" << endl ;
    }
}

