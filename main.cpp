#include<bits/stdc++.h>

using namespace std ;

template<class t>
vector<t> split_me(string line){
    vector<t>ret ; int pos ;
    while( (pos = (int)line.find(" | ")) != string::npos ){
        string sub = line.substr(0,pos);
        if(!sub.empty())
            ret.push_back(sub);
        line.erase(0 , pos+3);
    }
    if(!line.empty())
        ret.push_back(line);
    return ret ;
}

struct user{

    string username, email, password; bool allow_annon;
    int id ;
    user(){
        username = email = password = {""}, allow_annon = {0} ;
    }

    bool read_user(){

        string _username , _email , _password ; bool _allow ;

        cout << "Enter your username, email, password (separated by spaces) : ";
        cin >> _username >> _email >> _password  ;
        cout << "Do you allow annonymous messages ( 1 -> yes | 0 -> no ) :  " ;
        cin >> _allow ;
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

    bool valid_password(string&_password){
        bool found_num = false, found_alph = false ;
        for(auto&c : _password)
        {
            if( (c >='a' && c<='z')
               || (c>='A' && c <='Z') ) found_alph = true ;
            else if( c >= '1' && c <= '9' ) found_num = true ;
        }
        return (found_alph && found_num) ;
    }

    bool add_to_fs(){
        ofstream out_stream("users.txt",ios::app), outstream_ids("ids.txt",ios::app);
        if(!out_stream.is_open()||outstream_ids.fail()) return false ;
        id = generate4DigitID() ;
        out_stream<< id << " " << username << " " << email << " " << password << " " << allow_annon <<  endl ;
        outstream_ids << id << endl ;
        return true ;
    }

    void extract_data(string line){
        istringstream iss(line) ;
        iss >> id >> username >> email >> password >> allow_annon ;
    }


};

struct question{

    int parent_question_id , question_id , from_id , to_id ; bool annony ;
    string question_txt = "" , answer_txt = "" ;

    question(){
        parent_question_id = question_id = from_id = to_id = annony = 0 ;
    }

    int generate4DigitID() {
        ifstream in("questions_ids.txt");
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

        ofstream oss("questions_ids.txt" , ios::app) ;
        oss << ii << endl ;

        return ii ;

    }

    void print_me(){
        cout << "Question id (" << question_id << ") " ; if(!annony) cout << "From user("<<from_id<<")" ;
        cout << " to user(" << to_id << ")\n" ;
        cout << "Question : " << question_txt << endl ;
        cout << "Answer : " << ((answer_txt == "-1")? "Not answered yet" : answer_txt ) << endl<<endl ;
    }

    void read_question(int _to_id){
        cout << "Enter your question please (Make it short as possible) : " ;
        cin.ignore() ;
        getline( cin , question_txt ); to_id = _to_id ; question_id =generate4DigitID() ;
    }

    void answer_question(){
        cout << "Enter the answer please : " ;
        getline(cin , answer_txt);
    }

    void extract_data(string &line){
        //parent_question_id - question_id - from_id - to_id - question - answer
        vector<string>data = split_me<string>(line) ;
        parent_question_id = stoi(data[0]) , question_id = stoi(data[1]) , from_id = stoi(data[2]) , to_id = stoi(data[3]) ;
        question_txt = data[4] , answer_txt = data[5] ;
    }

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

    pair<bool,user> search_user(string _name , string _pass)
    {
        vector<user> users ;
        vector<string> data = fetch_data<string>("users.txt") ;
        for( auto&c : data)
        {
            istringstream iss(c) ;
            user u ;
            iss >> u.id >> u.username >> u.email >> u.password >> u.allow_annon ;
            if( u.username == _name && _pass == u.password ) return make_pair( true , u ) ;
        }
        user u ;
        return make_pair( false , u ) ;
    }

};


struct ask_system{

    user loggedin ; fs_system fs ;

    map< int , vector<int>> thread_questions ;

    map< int , question> id_to_question ;

    vector<user>sys_users ;


    void first_menu(){
        int choice ;
        while(true){
            cout << "Menu :\n 1. Login\n 2.Sign Up\n else.Exit\n Enter your choice : " ;
            cin >> choice ;
            if(choice == 1){
                if(login()){
                    start_menu();
                }
            }else if(choice == 2){
                signup();
            }
            else{
                break;
            }
        }
    }

    int print_start_menu(){
        fetch_users();
        fetch_questions() ;
        cout << endl << "Menu" << endl ;
        cout << "1: Print Questions To Me" << endl ;
        cout << "2: Print Questions From Me" << endl ;
        cout << "3: Answer Question"  << endl ;
        cout << "4: Delete Question" << endl ;
        cout << "5: Ask Question" << endl ;
        cout << "6: List system users" << endl ;
        cout << "7: Feed" << endl ;
        cout << "8: Logout" << endl << endl  ;
        cout << "Enter your choice [ 1 - 8 ] : " ;
        int ch ; cin >> ch ; cout << endl ;
        return ch ;
    }

    void start_menu(){
        while(true){

            int ch = print_start_menu();

            if(ch == 1) // 1: Print Questions To Me
                print_questions_to_me() ;
            else if(ch == 2) // 2: Print Questions From Me
                print_questions_from_me() ;
            else if(ch == 3) // 3: Answer Question
                answer_question() ;
            else if(ch == 4) // 4: Delete Question
                delete_question() ;
            else if(ch == 5) // 5: Ask Question
                ask_question() ;
            else if(ch == 6) // 6: List system users
                list_system_users() ;
            else if(ch == 7) // 7: Feed
                print_feed() ;
            else if(ch == 8) // 8: Logout
                break ;
            else cout << "INVALID INPUT\n";


        }
    }

    bool login(){
        string name , password ;
        cout << "Enter username and password : " ;
        cin >> name >> password ;
        fs_system fs ;
        pair<bool,user> auth = fs.search_user(name , password);
        if(auth.first){
            loggedin = auth.second ;
        }
        return auth.first ;
    }

    void signup(){
        user reg_user ;
        if(reg_user.read_user())
        {
            if(reg_user.add_to_fs())
                cout << "Registered!!" << endl ;
        }
    }

    void list_system_users(){
        fetch_users();
        for(auto&user : sys_users){
            cout << "User id : " << user.id << ", Username : " << user.username << endl ;
        }
    }

    void fetch_users(){
        vector<string>data = fs.fetch_data<string>("users.txt") ;
        sys_users.resize(data.size());
        for(int i = 0 ; i < (int)data.size() ; i++){
            sys_users[i].extract_data(data[i]) ;
        }
    }

    void fetch_questions(){
        vector<string>ques = fs.fetch_data<string>("questions.txt");
        vector<question> questions(ques.size()) ;
        for( int i = 0 ; i < (int)ques.size() ; i++ ){
            questions[i].extract_data(ques[i]) ;
        }
        map_questions(questions) ;
    }

    void map_questions(vector<question>&questions){
        //parent_question_id - question_id - from_id - to_id - question - answer
        for(auto&q : questions)
            id_to_question[q.question_id] = q ;

        map_threads();
    }

    void map_threads()
    {

        thread_questions.clear();
        for(auto[ question_id , questionn ] : id_to_question ){
            if( questionn.parent_question_id == -1 )
                thread_questions[question_id] = { question_id } ;
        }
        for(auto[ question_id , questionn ] : id_to_question )
        if( questionn.parent_question_id != -1 )
            thread_questions[questionn.parent_question_id].push_back(questionn.question_id) ;


    }

    void sortIdToQuestionByParentQuestionId() {
    vector<pair<int, question>> vec(id_to_question.begin(), id_to_question.end());
    sort(vec.begin(), vec.end(),
              [](const auto& a, const auto& b) {
                  return a.second.parent_question_id > b.second.parent_question_id;
              });
    id_to_question.clear();
    for (const auto& pair : vec) {
        id_to_question[pair.first] = pair.second;
    }
    }

    void write_questions(){
        ofstream out("questions.txt") ;
        sortIdToQuestionByParentQuestionId();
        for( auto&[id , questionn] : id_to_question ){
            out << questionn.parent_question_id << " | " << questionn.question_id << " | " << questionn.from_id << " | " << questionn.to_id << " | " << questionn.question_txt << " | " << ((questionn.answer_txt == "") ? "-1" : questionn.answer_txt) ;
            out<< endl ;
        }
    }

    void print_questions_to_me(){
        for( auto&[ thread_id , questions_id ] : thread_questions ){
            if( id_to_question[questions_id[0]].to_id == loggedin.id ){
                for( int i = 0 ; i < (int)questions_id.size() ; i++){
                    if( i != 0 )cout << "Thread : " ;
                    id_to_question[questions_id[i]].print_me() ;
                }
            }
        }
    }

    void print_questions_from_me(){
        for( auto&[ thread_id , questions_id ] : thread_questions ){
            if( id_to_question[questions_id[0]].from_id == loggedin.id ){
                for( int i = 0 ; i < (int)questions_id.size() ; i++){
                    id_to_question[questions_id[i]].print_me() ;
                }
            }
        }
    }

    void answer_question()
    {
        cout << "answer_question\n" ;
    }

    void delete_question()
    {
        cout << "delete_question\n" ;
    }

    void ask_question()
    {
        cout << "Enter user id or -1 to cancel : " ; string ch ; cin >> ch ;
        auto in = find_if(sys_users.begin() , sys_users.end() ,  [ch](const user& u) { return u.id == stoi(ch); } ) ;
        if( in != sys_users.end() )
        {
            int index = in - sys_users.begin() ; user us = sys_users[index] ;
            if( !us.allow_annon ) cout << "Note : the user doesnt allow annonymus questions\n" ;
            cout << "For thread questions : Enter question id -1 for new question : "; cin >> ch ;
            if( ch != "-1" && thread_questions.find(stoi(ch)) == thread_questions.end() ){
                cout << "Invalid question id.\n" ; return ;
            }
            question new_question ;
            new_question.from_id = ((us.allow_annon) ? -1 : loggedin.id) ;
            new_question.parent_question_id = ((ch == "-1") ? -1 : stoi(ch)) ;
            new_question.read_question(us.id) ;

            id_to_question[new_question.question_id] = new_question ;
            if(new_question.parent_question_id != -1 ) thread_questions[new_question.parent_question_id].push_back(new_question.question_id) ;

            write_questions();

        }else{
            cout << "The user id is not found, re write it please\n" ;
        }
        fetch_questions();
    }

    void print_feed()
    {
        for( auto&[ thread_id , questions ] : thread_questions ){
            if(questions.size() > 1) cout << "Thread (" << thread_id <<") : ";
            for(auto&id:questions)
                id_to_question[id].print_me();
            cout << "\n--------------------------------------------\n" ;
        }
    }
};


int main()
{
    ask_system ask ;
    ask.first_menu() ;
}









