#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<list>
#include<cmath> 
#include<sstream>
using namespace std;


list<string> spellSuggestions;
static int wordCounter=0;
//static int synonymsCounter=0;
vector< vector<int> > similarWordsGraph(10, vector<int>(10));
vector< vector<int> > synonymsGraph(10, vector<int>(10));
vector< vector<int> > antonymsGraph(10, vector<int>(10));
//vector< vector<int> > antonymsGraph;

struct node{
	string word;
	string definition;
	int index;
	node *left=NULL;
	node *right=NULL;
};
node *root=NULL;


void writeSynonymtoFile(string word,string newSynonym){
    string line;
    string word_line;
    bool found = false;

    ifstream file("synonyms.txt");
    ofstream temp_file("temp_file.txt",ios::trunc);

    if (file.is_open()) {
        while (getline(file, line)) {
            
            if (line.find(word) == 0) {
                found = true;
                word_line = line + "," + newSynonym;
                temp_file << word_line << endl;
            } 
            else {
                temp_file << line << endl;
            }
        }
        file.close();
    }
    temp_file.close();
    remove("synonyms.txt");
    rename("temp_file.txt","synonyms.txt");

    if (!found) {
        cout << "Word not found in the dictionary" << endl;
    } else {
        cout << "Synonym added successfully!" << endl;
    }
}


void writeNewWordtoFile(string word,string definition){
    ofstream file;
    file.open("words.txt", ios::app); 
    if (file.is_open()) {
        file << word << ":" << definition << endl;
        file.close();
        cout << "Word added successfully!" << endl;
    } 
    else {
        cout << "Unable to open file" << endl;
    }
}

void writeAntonymstoFile(string word1,string word2){
    ofstream file;
    file.open("antonyms.txt", ios::app); 
    //string line;
    if (file.is_open()) {
        file << word1 << "," << word2 << endl;
        file.close();
        cout << "Antonyms added successfully!" << endl;
    } 
    else {
        cout << "Unable to open file" << endl;
    }
}

void resizeGraphs(){
    for (int i = 0; i < wordCounter; i++) {
    similarWordsGraph[i].resize(wordCounter);
    }
    for (int i = 0; i < wordCounter; i++) {
    synonymsGraph[i].resize(wordCounter);
    }
    for (int i = 0; i < wordCounter; i++) {
    antonymsGraph[i].resize(wordCounter);
    }

}


void addEdgeSimilar(node* word1,node* word2){
    if(word1 != NULL && word2 != NULL){
        similarWordsGraph[word1->index][word2->index]=1;
        similarWordsGraph[word2->index][word1->index]=1;
    }
    else {
        cout << "Cannot add edge, one of the words not found" << endl;
    }

}

void addEdgeSynonyms(node* word1,node* word2){
    if(word1 != NULL && word2 != NULL){
        synonymsGraph[word1->index][word2->index]=1;
        synonymsGraph[word2->index][word1->index]=1;
    }
    else {
        cout << "Cannot add edge, one of the words not found" << endl;
    }

}

void addEdgeAntonyms(node* word1,node* word2){
    if(word1 != NULL && word2 != NULL){
        antonymsGraph[word1->index][word2->index]=1;
        antonymsGraph[word2->index][word1->index]=1;
    }
    else {
        cout << "Cannot add edge, one of the words not found" << endl;
    }

}

node* search(string word, node* p) {
    if (p == nullptr) { 
        return nullptr;
    }
    if (word < p->word) { 
        return search(word, p->left);
    } else if (word > p->word) { 
        return search(word, p->right);
    } else {
        return p;
    }
}

node* searchbyIndex(int index,node *p){
	if(p==NULL){
		return NULL;
	}
	else if(p->index==index){
		return p;
	}
	node *left= searchbyIndex(index,p->left);
	node *right =searchbyIndex(index,p->right); 
	if(left==NULL && right==NULL){
		return NULL;
	}
	else{
		if(right!=NULL){
			return right;
		}
		else if(left!=NULL){
			return left;
		}
 }
}

bool IsOneEditDistance(string word1, string word2) {
  int len1 = word1.size();
  int len2 = word2.size();
  if (abs(len1 - len2) > 1) {
    return false;
  }
  int differences = 0;   
  int i = 0;
  int j = 0;
  while (i < len1 && j < len2) {
    if (word1[i] != word2[j]) {
      ++differences;
      if (differences > 1) {
        return false;
      }
      if (len1 > len2) { // Word1 has one more character
        ++i;
      } else if (len1 < len2) { // Word2 has one more character
        ++j;
      } else {  // Words have the same length
       
        ++i;
        ++j;
      }
    } 
	else{
      ++i;
      ++j;
    }
  }
  // Check if one of the words has one more character.
  if (i < len1 || j < len2) {
    ++differences;
  }
  
  return differences == 1;
}

void spellCorrection(string word,node *p){  
	if(p!=NULL){
		if(IsOneEditDistance(word,p->word)){
			spellSuggestions.push_back(p->word);
		}
		spellCorrection(word,p->left);
		spellCorrection(word,p->right);
	}
}

void similarWords(node *word,node *p){
    if(p!=NULL){
        if(IsOneEditDistance(word->word,p->word)){
            addEdgeSimilar(word,p);
        }
        similarWords(word,p->left);
        similarWords(word,p->right);
    }
}

void insert(string word,string definition){
	resizeGraphs();

	node *curr = new node;
	curr->word=word;
	curr->definition=definition;
	wordCounter++;
	curr->index= (wordCounter-1);

	if(root==NULL){
		root=curr;
        resizeGraphs();
	}
	else{
		node *p=root;
		node *q=NULL;
		
		while(p!=NULL){
			if(word < p->word){
				q=p;
				p=p->left;
				
			}
			else if(word > p->word){
				q=p;
				p=p->right;
			}
		}
		if(q->word > word){
			q->left=curr;
		}
		else if(q->word< word){
			q->right=curr;
		}
        similarWordsGraph.resize(wordCounter);
        synonymsGraph.resize(wordCounter);
        antonymsGraph.resize(wordCounter);
        resizeGraphs();
	}

    similarWords(curr,root);
    
}

void insert_word(string word,string definition){
	node *p=search(word,root);
	if(p==NULL){
		insert(word,definition);
        
	}
	else{
		cout<<"The word "<<word<<" is already present in the dictionary\n";
	}
}

void insertSynonyms(string word){
    node *word1 = search(word,root);
    if(word1==NULL){
        cout<<"\nNo such word in the dictionary.\n";
        spellCorrection(word,root);
        if(!spellSuggestions.empty()){
		cout<<"\nDid you mean: ";
		while(!spellSuggestions.empty()){
			cout<<" "<<spellSuggestions.front();
			spellSuggestions.pop_front();
			}
		}
		else{
		cout<<"\nSorry, looks like there are no spell corrections for your word\n";
	}
    }
    else{
        cout<<"\nEnter synonym: ";
        string str;
        cin>>str;
        node *word2 = search(str,root);
        if(word2==NULL){
            cout<<"\nWord not present in dictionary.\n";
        }
        else{
            addEdgeSynonyms(word1,word2);
            writeSynonymtoFile(word1->word,word2->word);
        }
   
    }

}

void displayAllSynonyms(string word){
    node *word1= search(word,root);
    if(word1==NULL){
        cout<<"\nNo such word in the dictionary.\n";
        spellCorrection(word,root);
        if(!spellSuggestions.empty()){
		cout<<"\nDid you mean: ";
		while(!spellSuggestions.empty()){
			cout<<" "<<spellSuggestions.front();
			spellSuggestions.pop_front();
			}
		}
		else{
		cout<<"\nSorry, looks like there are no spell corrections for your word\n";
	    }
    }
    else{
        cout<<"\nSynonyms: ";
        for(int i=0;i<synonymsGraph.size();i++){
            if(synonymsGraph[word1->index][i]==1){
                node *word2 = searchbyIndex(i,root);
                cout<<" "<<word2->word;

            }
        }
        cout<<endl;
    }
}

void insertAntonyms(string word){
    node *word1 = search(word,root);
    if(word1==NULL){
        cout<<"\nNo such word in the dictionary.\n";
        spellCorrection(word,root);
        if(!spellSuggestions.empty()){
		cout<<"\nDid you mean: ";
		while(!spellSuggestions.empty()){
			cout<<" "<<spellSuggestions.front();
			spellSuggestions.pop_front();
			}
		}
		else{
		cout<<"\nSorry, looks like there are no spell corrections for your word\n";
	    }
    }
    else{
        cout<<"\nEnter antonym: ";
        string str;
        cin>>str;
        node *word2 = search(str,root);
        if(word2==NULL){
            cout<<"\nWord not present in dictionary.\n";
        }
        else{
            addEdgeAntonyms(word1,word2);
           writeAntonymstoFile(word1->word,word2->word);
        }
   
    }
}

void displayAllAntonyms(string word){
    node *word1= search(word,root);
    if(word1==NULL){
        cout<<"\nNo such word in the dictionary.\n";
        spellCorrection(word,root);
        if(!spellSuggestions.empty()){
		cout<<"\nDid you mean: ";
		while(!spellSuggestions.empty()){
			cout<<" "<<spellSuggestions.front();
			spellSuggestions.pop_front();
			}
		}
		else{
		cout<<"\nSorry, looks like there are no spell corrections for your word\n";
	    }
    }
    else{
        cout<<"\nAntonyms: ";
        for(int i=0;i<antonymsGraph.size();i++){
            if(antonymsGraph[word1->index][i]==1){
                node *word2 = searchbyIndex(i,root);
                cout<<" "<<word2->word;

            }
        }
        cout<<endl;
    }
}


void displayprefixWords(node *p,string s){
    if(p!=NULL){
        string str = p->word.substr(0,s.length());
        if(str==s){
		cout<<p->word<<endl;
		displayprefixWords(p->left,s);
		displayprefixWords(p->right,s);
        }
        else{
            displayprefixWords(p->left,s);
		    displayprefixWords(p->right,s);
 }
	}
}

void displaysuffixWords(node *p,string s){
    if(p!=NULL){
        if(p->word.rfind(s) == p->word.length()-s.length()){
        if(p->word.length()>s.length()){
		cout<<p->word<<endl;
		displaysuffixWords(p->left,s);
		displaysuffixWords(p->right,s);
        }
        }
        else{
            displaysuffixWords(p->left,s);
		    displaysuffixWords(p->right,s);
}
	}
}

vector<string> findSimilarWords(string word){
    node *wordd = search(word,root);
    vector<string> similar;
	if(wordd==NULL){
		cout<<"\nWord does not exist in your dictionary.\n";
		spellCorrection(word,root);
		if(!spellSuggestions.empty()){
		cout<<"\nDid you mean: ";
		while(!spellSuggestions.empty()){
			cout<<" "<<spellSuggestions.front();
			spellSuggestions.pop_front();
			}
		}
		else{
		cout<<"\nSorry, looks like there are no spell corrections for your word\n";
	}
    return similar;
	}
	else{
   
    for(int i=0;i<similarWordsGraph.size();i++){
     if(similarWordsGraph[wordd->index][i]==1){
        node *temp = searchbyIndex(i,root);
        similar.push_back(temp->word);
     }
    }
    return similar;
	}
}

void findDefinition(string word){
	node *p=search(word,root);
	if(p!=NULL){
		cout<<"\nWord: "<<word<<"\nDefinition: "<<p->definition<<endl;
	}
	else{
		cout<<"\nWord does not exist\n";
		spellCorrection(word,root);
		if(!spellSuggestions.empty()){
		cout<<"\nDid you mean: ";
		while(!spellSuggestions.empty()){
			cout<<" "<<spellSuggestions.front();
			spellSuggestions.pop_front();
			}
		}
		else{
		cout<<"\nSorry, looks like there are no spell corrections for your word\n";
	}
	}
	
	}


void displayAll(node *p){
	
	if(p!=NULL){
		displayAll(p->left);
        cout<<p->word<<":"<<p->definition<<endl;
		displayAll(p->right);
	}
}

void displayWords(node *p){
	if(p!=NULL){
		displayWords(p->left);
        cout<<p->word<<endl;
		displayWords(p->right);
	}
}



void synonymsFromFile(){
    ifstream file("synonyms.txt");
    if(!file.is_open()){
        cout<<"Error file opening"<<endl;
    }
    vector<string> words;  
    string line;
while(getline(file, line)) {
    stringstream ss(line); 
    string word;
    while(getline(ss, word, ',')) { 
        words.push_back(word);
    }
    for (int i = 0; i < words.size(); ++i) {
        node* word1 = search(words[i], root);
    for (int j = i + 1; j < words.size(); ++j) {
        node* word2 = search(words[j], root);
        if(word1!=NULL && word2!=NULL){
        addEdgeSynonyms(word1, word2);
        }
    }
    
    }
    while(!words.empty()){
        words.pop_back();
    }

}

}

void antonymsFromFile(){
    ifstream file("antonyms.txt");
	if(!file.is_open()){
		cout<<"Error file opening"<<endl;
	}

    string line;
    while(getline(file,line)){
        size_t pos = line.find(",");
        string word1 = line.substr(0,pos);
        string word2 = line.substr(pos+1);

        node *wordd1 = search(word1,root);
        node *wordd2 = search(word2,root);
        if(wordd1!=nullptr&&wordd2!=nullptr){
            addEdgeAntonyms(wordd1,wordd2);
        }
    }


}

void importFromFile(){
	ifstream file("words.txt");
	if(!file.is_open()){
		cout<<"Error file opening"<<endl;
	}
	
	string line;
	while(getline(file,line)){
		size_t pos = line.find(":");
		string word = line.substr(0,pos);
		string definition = line.substr(pos+1);
		
		insert_word(word,definition);
		
	}
	file.close();
	synonymsFromFile();
    antonymsFromFile();
}


void menu(){
	cout<<"\n1-Insert a word in your dictionary.\n2-Display all words.\n3-Display all words with definitions.\n4-Find a word's definition.\n5-Find Similar Words\n6-Search words by prefix.\n7-Search words by suffix.\n8-Insert Synonyms for a word.\n9-Display Synonyms for a word.\n10-Insert Antonyms for a word.\n11-Display Antonyms for a word.\n12-Exit\n";
	int choice;
	cout<<"\n\nSelect: ";
	cin>>choice;
	
	switch(choice){
		case 1:
		{
		cout<<"\nEnter word: ";
		string str;
		cin>>str;
		cin.ignore(); 
		cout<<"\nEnter definition: ";
		string def;
		getline(cin, def);
		insert_word(str,def);
        writeNewWordtoFile(str,def);
		menu();
		break;
		}
		case 2:
			{
				displayWords(root);
				menu();
				break;
			}
		case 3:
		{
			displayAll(root);
			menu();
			break;
			}
		case 4:
		{
			cout<<"\nEnter Word: ";
			string word;
			cin>>word;
			findDefinition(word);
			menu();
			break;
				}
		case 5:
		{
			cout<<"\nEnter a word: ";
			string word;
			cin>>word;
			vector<string> suggestions;
			suggestions = findSimilarWords(word);
			if(!suggestions.empty()){
			cout<<"\nSimilar words: ";
			for(string s:suggestions){
				cout<<" "<<s;
			}
			cout<<endl;
			}
            cout<<endl;
			menu();
			break;
		}
		case 6:
		{
            cout<<"\nEnter prefix: ";
            string str;
            cin>>str;
            cout<<"\nWords: ";
            displayprefixWords(root,str);
            menu();
            break;
        }
        case 7:
        {
            cout<<"\nEnter suffix: ";
            string str;
            cin>>str;
            cout<<"\nWords: ";
            displaysuffixWords(root,str);
            menu();
            break;
        }
        case 8:
        {
            cout<<"\nEnter word: ";
            string str;
            cin>>str;
            insertSynonyms(str);
            menu();
            break;
        }
        case 9:
        {
            cout<<"\nEnter word: ";
            string word;
            cin>>word;
            displayAllSynonyms(word);
            menu();
            break;
        }
        case 10:
        {
            cout<<"\nEnter word: ";
            string str;
            cin>>str;
            insertAntonyms(str);
            menu();
            break;
        }
        case 11:
        {
            cout<<"\nEnter word: ";
            string word;
            cin>>word;
            displayAllAntonyms(word);
            menu();
            break;
        }
        case 12:
        break;
		default:
		{
			cout<<"\nPlease enter a valid option.\n";
			menu();
			break;
		}				
	}

}

int main(){
    cout<<"Please wait......\n";
    cout<<"Loading Dictionary(Ignore the following)\n\n";
	importFromFile();
    cout<<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    cout<<"Almost done.......\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

	cout<<"***********************************************************\n";
	cout<<"-----------------------------------------------------------\n";
	cout<<"-------------------DIGITAL DICTIONARY----------------------\n";
	cout<<"-----------------------------------------------------------\n";
	cout<<"***********************************************************\n";
	cout<<"\nWELCOME!\n";
	menu();
}
