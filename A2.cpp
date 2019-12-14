//Rakshak Shrestha rs720 5375496 CSCI319 A2 chord implementation

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <string.h>

using namespace std;

const string myName = "Rakshak Shrestha \n5375496";

//Informtaion stored in each index node
struct node{
  long long id;
  long long ftable[32];
  vector <string> dataa;
};

vector <node*> total_nodes;     //storage for all existing nodes in the system
vector <node*>::iterator it_nodes, it_nodes2, it_nodes3, it_nodes4;
vector <string>::iterator it_string;

void StartChord(int c_size);
void InsertPeer(long long n, int c_size);
void RemovePeer(long long rem_id, int c_size);
void InsertData(string dat, int c_size);
void DeleteData(string dat, int c_size);
void calcFingTable(node* N, int c_size);
long long GetHash(string data, int c_size);
void Print(long long key, int c_size);
void Read(string filename);
void Terminate();
node* Lookup(node* currNode, long long key, int c_size);
long long ScanFingTable(node* cNode, int c_size, long long key);


int main(int argc, char** argv){
  string filename = "";
  if (argc == 2){
    filename = argv[1];
  }
  else{
    cout << "File not found.\n";
    return 1;
  }
  Read(argv[1]);
  return 0;
};

void StartChord(int c_size){
  cout << myName << endl;
  //deletes existing nodes from the system before proceeding
  for (it_nodes = total_nodes.begin(); it_nodes != total_nodes.end(); it_nodes++){
    delete (*it_nodes);
  }
  //initialing the first node for the system
  node *initial_node = new node;
  initial_node->id = 0;
  //initialing the finger table to be 0
  for (int i = 0; i < c_size; i++){
    initial_node->ftable[i] = 0;
  }
  total_nodes.push_back(initial_node);  //adds node to the system
}

void InsertPeer(long long new_id, int c_size){
  //initialising the next node
  node *new_node = new node;
  new_node->id = new_id;
  bool check = true;
  for (it_nodes = total_nodes.begin(); it_nodes != total_nodes.end(); it_nodes++){
    if (new_node->id < (*it_nodes)->id){
      total_nodes.insert(it_nodes, new_node);   //inserts new node into appropriate spot in the vector
      check = false;
      break;
    }
  }
  if (check) total_nodes.push_back(new_node);
  //updates finger tables for all nodes
  for (it_nodes = total_nodes.begin(); it_nodes != total_nodes.end(); it_nodes++){
    cout << (*it_nodes)->id << ">";
    calcFingTable((*it_nodes), c_size);
  }
  cout <<endl << "PEER " << new_id << " ADDED." << endl;
}

void RemovePeer(long long rem_id, int c_size){
  long long remove_id = rem_id;
  //finds, deletes memory and erases specified node from the system
  for (it_nodes = total_nodes.begin(); it_nodes != total_nodes.end(); it_nodes++){
    if (remove_id == (*it_nodes)->id){
      delete (*it_nodes);
      total_nodes.erase(it_nodes);
      break;
    }
  }
  //updates finger tables for all nodes
  for (it_nodes = total_nodes.begin(); it_nodes != total_nodes.end(); it_nodes++){
    cout << (*it_nodes)->id << ">";
    calcFingTable((*it_nodes), c_size);
  }
  cout << endl << "PEER " << rem_id << " REMOVED." << endl;
  if (total_nodes.empty()){
    cout << "Last of the peers removed. CHORD terminated" << endl;
  }
}

//function to calculate finger tables
void calcFingTable(node* N, int c_size){
  for (int i = 0; i < c_size; i++){
    long long calc =  pow(2.0, i);
    calc += N->id;
    bool check = true;
    for (it_nodes2 = total_nodes.begin(); it_nodes2 != total_nodes.end(); it_nodes2++){
      if (calc <= (*it_nodes2)->id){
        N->ftable[i] = (*it_nodes2)->id;
        check = false;
        break;
      }
    }
    if (check){
      int calc2 = calc % (long)(pow(2.0, c_size));
      bool check2 = true;
      for (it_nodes3 = total_nodes.begin(); it_nodes3 != total_nodes.end(); it_nodes3++){
        if(calc2 <= (*it_nodes3)->id){
          N->ftable[i] = (*it_nodes3)->id;
          check2 = false;
          break;
        }
      }
      if(check2) N->ftable[i] = (*total_nodes.begin())->id;
    }
  }
}


void InsertData(string dat, int c_size){
  long long hashedData = GetHash(dat, c_size);
  //get node where new data is to be inserted
  node* destNode = Lookup(*total_nodes.begin(), hashedData, c_size);
  cout << endl;
  destNode->dataa.push_back(dat);
  cout << "INSERTED " << dat << " (key = " << hashedData << ") " << "AT " << destNode->id << endl;
}

void Print(long long key, int c_size){
  //looks up for required node and then prints out stored data
  node* reqNode = Lookup((*total_nodes.begin()), key, c_size);
  cout << endl;
  cout << "DATA AT NODE INDEX " << reqNode->id << endl;
  for (it_string = (reqNode->dataa).begin(); it_string != (reqNode->dataa).end(); it_string++){
    cout << *it_string << endl;
  }
  cout << "FINGER TABLE OF NODE " << reqNode->id << endl;
  for (int i = 0; i < c_size; i++){
    cout << reqNode->ftable[i] << " ";
  }
  cout << endl;
}

void DeleteData(string dat, int c_size){
  //looks up for the required node and deletes specified data
  long long hashedData = GetHash(dat, c_size);
  node* reqNode = Lookup(*total_nodes.begin(), hashedData, c_size);
  cout << endl;
  for (it_string = (reqNode->dataa).begin(); it_string != (reqNode->dataa).end(); it_string++){
    if ((*it_string) == dat) {
      reqNode->dataa.erase(it_string);
      cout << "REMOVED " << dat << " (key = " << hashedData << ") FROM " << reqNode->id << endl;
      break;
    }
  }
}

node* Lookup(node* currNode, long long key, int c_size){
  node* cNode = currNode;
  cout << cNode->id << ">";
  if (key <= cNode->id || key > total_nodes.back()->id) return cNode;
  long long nextID = ScanFingTable(cNode, c_size, key);
  node* nextNode;
  for (it_nodes4 = total_nodes.begin(); it_nodes4 != total_nodes.end(); it_nodes4++){
    if ((*it_nodes4)->id == nextID){
       nextNode = (*it_nodes4);
       break;
    }
  }
  return Lookup(nextNode, key, c_size);
}

long long ScanFingTable(node* cNode, int c_size, long long key){
  node* curNode = cNode;
  for (int i = 0; i < c_size; i++){
    if(key < curNode->ftable[i]){
      if(i == 0) {
        return curNode->ftable[i];
      }
      else{
        return curNode->ftable[i-1];
      }
    }
  }
  long long max_val = 0;
  for (int j = 0; j < c_size; j++){
    if (max_val < curNode->ftable[j]) max_val = curNode->ftable[j];
  }
  return max_val;
}

long long GetHash(string data, int c_size){
  unsigned long long int i = 0;
  for (int j = 0; j < data.length(); j++){
    i = ((i << 5) + i) ^ (int)data[j];
  }
  if (i > (long)pow(2.0, c_size)) i = i % (long)pow(2.0, c_size);
  return i;
}

void Terminate(){
  ofstream fout;
  fout.open("dump.txt");
  for (it_nodes = total_nodes.begin(); it_nodes != total_nodes.end(); it_nodes++){
    fout << "DATA STORED AT " << (*it_nodes)->id << ":" <<  endl;
    for (it_string = (*it_nodes)->dataa.begin(); it_string != (*it_nodes)->dataa.end(); it_string++){
      fout << *it_string;
    }
    fout << endl << endl;
  }
  for (it_nodes = total_nodes.begin(); it_nodes != total_nodes.end(); it_nodes++){
    delete (*it_nodes);
  }
}

void Read(string filename){
  ifstream fin;
  fin.open("cmd.txt");
  string cmd;
  string comm, dat;
  int sizeCmd;
  int intCmd;

  while(getline(fin, cmd)){
    dat = "";
    istringstream is(cmd);
    is >> comm;
    if (comm == "StartChord"){
      is >> sizeCmd;
     StartChord(sizeCmd);
    }
    else if (comm == "InsertPeer"){
      is >> intCmd;
      InsertPeer(intCmd, sizeCmd);
    }
    else if (comm == "RemovePeer"){
      is >> intCmd;
      RemovePeer(intCmd, sizeCmd);
    }
    else if (comm == "Print"){
      is >> intCmd;
      Print(intCmd, sizeCmd);
    }
    else if (comm == "Terminate"){
      Terminate();
    }
    else if (comm == "InsertData"){
      string val;
      while(is >> val){
        if (val[0] == '#') break;
        dat.append(val);
        dat.push_back(' ');
      }
      InsertData(dat, sizeCmd);
    }
    else if (comm == "DeleteData"){
      string val;
      while (is >> val){
        if (val[0] == '#') break;
        dat.append(val);
        dat.push_back(' ');
      }
      DeleteData(dat, sizeCmd);
    }
  }
}
