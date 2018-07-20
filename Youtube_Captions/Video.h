#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<unordered_map>
#include<vector>
#include<map>
#include<set>
using namespace std;

// ===================================================
struct Video
{
	Video() : title{""} {}	
	Video(string t, 
        string f, 
        string u) : title{t}, url{u}, filename{f} {}
	
  string shellFile, title, filename, url;
	
  unordered_map<string, short> wordCountMap{};
  map<string, bool>            commonWords{};
  vector<string>               captionLines{};
  set<string>                  timestampURLs{};

  void import_iWords();
  void countWords();
  void printWordCount();
};
// ===================================================

void Video::import_iWords(){
  
  ifstream commonWordsInputStream;
  string   currentWord;
	
  commonWordsInputStream.open("commonWords.txt");
	
  while(commonWordsInputStream) {
		
    commonWordsInputStream >> currentWord;
		
    transform(currentWord.begin(), 
              currentWord.end(), 
              currentWord.begin(), 
              ::tolower);

		commonWords[currentWord] = true;
	}
}

// delete irrelevant words from word count map
void Video::countWords(){
  
  string currentWord;
	import_iWords();
	
  for (auto& currentLine : captionLines) {		
    
    istringstream lineStream(currentLine);
		
    while(lineStream){
			lineStream >> currentWord;
			
      if (currentWord[currentWord.size()-1] == '!' || 
          currentWord[currentWord.size()-1] == '?' ||
			    currentWord[currentWord.size()-1] == '.' || 
          currentWord[currentWord.size()-1] == ',' ||
			    currentWord[currentWord.size()-1] == ':' || 
          currentWord[currentWord.size()-1] == ';' ) {
				
        currentWord.erase(currentWord.size()-1);
			}
			if (currentWord[0] == '[' || 
          currentWord[0] == '"' || 
          currentWord[0] == '(') {
				
        currentWord.erase(0);
				continue;
			}
			if (!isdigit(currentWord[0])) {
				auto wordCountIter  = wordCountMap.find(currentWord);
				auto commonWordIter = commonWords.find(currentWord);
				
        if (wordCountIter  == wordCountMap.end() && 
            commonWordIter == commonWords.end()) {

					wordCountMap[currentWord] = 1;
				}
				else if (commonWordIter == commonWords.end()) {
					wordCountMap[currentWord]++;
				}
			}
		}
	}
}

void Video::printWordCount() {

	for(auto& word : wordCountMap) {
		printf("\n%s: %u", word.first.c_str(), word.second);
	}
}
#pragma once
