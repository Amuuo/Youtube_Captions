#include"Main.h"


int main(int argc, char** argv) {
	
  vector<string> searchWords{};
  vector<Video>  videos{};	

  string youtube_dlOptions
    {"youtube-dl --write-auto-sub --skip-download "};
  string youtube_dlOptionSub
    {"youtube-dl --skip-download --write-sub --sub-lang en "};	
	
  string videoURL   {};	
  string searchLine {};
  string log        {" > logFile.txt"};
  char   response   {'q'};
	
  //push keywords into vector
	while (true) {
		
  if (firstWordSearch) {
		searchWords.erase(searchWords.begin(), searchWords.end());
  }		
  else {			
		videos.erase(videos.begin(), videos.end());      
	}
		redrawScreen();

    // get video information from user on initial search
		if (!firstWordSearch && argc != 2) {
			
      // prompt user for URL and y/n on download
      printf("\n\tENTER YOUTUBE URL: ");
      cin >> videoURL;			      

      printf("\n\tDOWNLOAD VIDEO? (Y/N): ");
      response = _getch();
			
      redrawScreen();
			
      // video class are initialized in this function			
      getFileNames(videos, videoURL);
			screenText = "\n\tVIDEO: ";

      // print video title to screen			
      for (const Video& vid : videos) {
				screenText += vid.title + '\n';
      }
			redrawScreen(screenText);

			printf("\n\tWhat words do you want to search?"
             "(Enter separated by a space): ");
			getline(cin, searchLine);
      
      // add all search words to searchWords vector
      stringstream searchLineStream(searchLine);      
      while(searchLineStream) {
        static string currSearchWord{};
        searchLineStream >> currSearchWord;
			  searchWords.push_back(currSearchWord);
      }
			redrawScreen(screenText);
			printf("\n\n");
		}

		if (!firstWordSearch) {
			redrawScreen(screenText);
			
      for (Video& v : videos) {
				v.timestampURLs.erase(v.timestampURLs.begin(), v.timestampURLs.end());
      }
			
      printf("\n\tWhat word do you want to search?: "); 
      cin >> searchLine;
			searchWords.push_back(searchLine);
		}

		if (firstWordSearch) {
			if (selection == 'v' || selection == 'V') {
				videos[0].url = videoURL;
        string cmd{youtube_dlOptions + videoURL};
				system((const char*)cmd.c_str());
				cmd = youtube_dlOptionSub + videoURL;
				system((const char*)cmd.c_str());
				if (response == 'y' || response == 'Y')
				{
					cmd = "youtube-dl " + videoURL;
					system((const char*)cmd.c_str());
				}
			}
			if (firstWordSearch == false) {
				cleanupFiles(videos);
      }
		}

		getTime(searchWords, videos);
		launchUrls(videos, searchWords);

		videos[0].countWords();

		auto store = [](unordered_map<string, short>& map) {
			vector<pair<string, short>> tmpVector;
			for(auto& word : map) {
				tmpVector.push_back(word);
			}
			return tmpVector;
		};

		auto wordCountCompare = [](pair<string, short> const & tmp1, 
                               pair<string, short> const & tmp2) {
			return tmp1.second > tmp2.second;
		};

		vector<pair<string, short>> vec = store(videos[0].wordCountMap);
		sort(vec.begin(), vec.end(), wordCountCompare);		
		ofstream out;
		string wordsFilename = "words_";
		wordsFilename += videoURL + ".txt";
		char f[30];
		out.open(wordsFilename);						
		
    for(auto& s : vec) {
			sprintf(f, "\n%-20s: %d", s.first.c_str(), s.second);
			out << f;
			memset(f, 0, 30);
		}
		out.close();

		cout << "\n\tDo you want to search another video? (Y/N)";
		firstWordSearch = _getch() == 'y' || 'Y' ? false : true;		
  }

	system("PAUSE");
	return 0;
}

void getFileNames(vector<Video>& vids, string firstUrl) {
  
	ifstream  filenameFile;	
	string    filename;	
	string    currentLine;
	string    youtubeDL_cmd;

	//load in .vtt file titles
	if (tolower(selection) == 'v') {
		youtubeDL_cmd = "youtube-dl --get-filename \"" + 
      firstUrl + "\" > filenames.txt";
  }
	
	system((const char*)youtubeDL_cmd.c_str());	
	
  filenameFile.open("filenames.txt");	

	//INITIALIZE VIDEO CLASSES	
	do {
		getline(filenameFile, currentLine);		
		istringstream lineStream(currentLine);
		
    char currentChar;
    int  positionCounter{0};
		int  dotPosition;
		
    while (lineStream) {
			lineStream.get(currentChar);
			if (currentChar == '.') {
				dotPosition = positionCounter;
      }
			++positionCounter;
		}
		
		if (currentLine[dotPosition + 1] != 'e' && 
        currentLine[dotPosition + 2] != 'n') {
			currentLine.erase(currentLine.begin() + dotPosition, 
                        currentLine.end());
    }

		filename = currentLine;
		currentLine += ".en.vtt";
		printf("%s %s","tmp:", currentLine);
		Video v(filename, currentLine, filename);
		vids.push_back(v);
	} while (filenameFile.peek() != EOF);

	filenameFile.close();
}

// clean up original file formatting
void cleanupFiles(vector<Video>& videoVector) {
	
  string   systemCall;
	string   fileContents;
	ofstream outStream;
  int      fileNumber{0};

	for (Video& video : videoVector) {
		video.shellFile = "cleanFile" + to_string(fileNumber) + ".sh";
		outStream.open(video.shellFile);
    string tmp{"#!\\bin\\bash\n\n" + sed + "\"" + video.filename + "\""};
		outStream << tmp;
		outStream.close();
		system((const char*)video.shellFile.c_str());
    ++fileNumber;
	}
}

void getTime(vector<string>& keys, vector<Video>& vids) {
	
  vector<string> captionLines;
	ifstream       captionFile;
	string         currentLine;	
	string         currentWord;
	
	if (firstWordSearch == false) {		
		captionFile.open(vids[0].filename);
		if (captionFile.fail()) {
			printf("\n\tCouldn't open file in getTime function");
			system("PAUSE");
			exit(1);
		}
		while (captionFile.good()) {
			getline(captionFile, currentLine);
			while (captionFile.peek() != EOF) {
				transform(currentLine.begin(), 
                  currentLine.end(), 
                  currentLine.begin(), 
                  ::tolower);
				vids[0].captionLines.push_back(currentLine);
				getline(captionFile, currentLine);
			}
		} captionFile.close();
	}
	//iterate through lines between '\n's
	short i = 0;
	for (string& line : vids[0].captionLines) {
		//iterate through keywords
		//parse look lines look for keywords
		istringstream lineStream(line);

		while (lineStream) {
			lineStream >> currentWord;

			if (currentWord == keys[0]) {
				
        try {
					if (isdigit(vids[0].captionLines[i - 3][0])){
						getTimestampURLs(vids[0], i - 3);
          }
				} catch (exception& e) { continue; }

				try {
					if (isdigit(vids[0].captionLines[i - 2][0])){
						getTimestampURLs(vids[0], i - 2);
          }
				} catch (exception& e) { continue; }

				try {
					if (isdigit(vids[0].captionLines[i - 1][0])){
						getTimestampURLs(vids[0], i - 1);
          }
				} catch (exception& e) { continue; }
			}
		}
		++i;
	}
}

void getTimestampURLs(Video& video, int line) {

  short hr{0};
  short min{0};
  short sec{0};

	//calculate timestamp
	try
	{
		if (video.captionLines[line][0] != '0') {
			hr += atoi(&video.captionLines[line][0]);
    }
		if (video.captionLines[line][0] == '0' && 
        video.captionLines[line][1] != '0') {
			hr += atoi(&video.captionLines[line][1]);
    }
		if (video.captionLines[line][3] != '0') {
			min += atoi(&video.captionLines[line][3]);
    }
		if (video.captionLines[line][3] == '0' && 
        video.captionLines[line][4] != '0') {
			min += atoi(&video.captionLines[line][4]);
    }
		if (video.captionLines[line][6] != '0') {
			sec += atoi(&video.captionLines[line][6]);
    }
		if (video.captionLines[line][6] == '0' && 
        video.captionLines[line][7] != '0') {			
      sec += atoi(&video.captionLines[line][7]);
    }
	}
	catch (exception& e) {}	
  
	if ((sec - mention) < 0) {
		--min;
		sec = 60 + (sec - mention);
	}
	string name{video.url + "&feature=youtu.be&t=" + 
              to_string(hr)  + 'h' + 
              to_string(min) + 'm' + 
              to_string(sec - mention) + 's'};

	video.timestampURLs.insert(name);

	return;
}

void launchUrls(vector<Video>& videos, vector<string> searchWord) {
	
  for (Video& video : videos) {
		if (video.timestampURLs.size() == 0) {
			printf("%s %s %s", "\n\n\tCouldn't find any mentions of",
                         searchWord[0],
                         " in this video...");
			return;
		}
		
    string openURLsysCmd{};
    short  statusDisplay{1};
    short  i{1};
    char   response{'q'};
		
    printf("\n\n");
		screenText += "\t\"" + searchWord[0] + "\" is mentioned: "
			+ to_string(video.timestampURLs.size()) + " times.";
		if (firstWordSearch == true) screenText += "\n";
		redrawScreen(screenText);
		printf("\n\n\tPress ENTER to launch first video...");
		_getch();

		if (firstWordSearch == false) {
			screenText += "\n\n\t    'S' -- skip"
				"\n\t    'C' -- cancel"
				"\n\t    'A' -- save all to clipboard"
				"\n\t'SPACE' -- next\n";
		}
		redrawScreen(screenText);

		for (string s : video.timestampURLs) {
			if (i > 1) {
        response = _getch();
        tolower(response);
      }
			if (response == 'a') {
				saveTimestampURLtoClipboard(video.timestampURLs);
				screenText += "\n\tURL List saved to clipboard";
				redrawScreen(screenText);
				response = _getch();
        tolower(response);
			}

      if(response == 's') { statusDisplay = 0; }
			if(response == 'c') { return;    }
			if(response == 'a') { continue;  }

			switch (statusDisplay) {
      
      case 0:
				screenText += "\n\tSKIPPING VIDEO " + 
          to_string(i) + "/" + 
          to_string(video.timestampURLs.size());				
				statusDisplay = 1;
				break;

			case 1:
				screenText += "\n\tPLAYING VIDEO " + 
          to_string(i) + "/" + 
          to_string(video.timestampURLs.size());
				openURLsysCmd = "start chrome \"" + s + "\"";
				system((const char*)openURLsysCmd.c_str());
				break;

			default:
				screenText += "\n\tINVALID ENTRY!";
				statusDisplay = 1;
				--i;
				break;
			}
		  redrawScreen(screenText);
			++i;
		}
	} return;
}

string getFileContents(string fileName) {
	
  ifstream inputStream;

	inputStream.open(fileName);
  string lines{""};

	if (inputStream.good()) {		
    while (inputStream.good()) {			
      string currentLine;
			getline(inputStream, currentLine);
			currentLine += "\n";
			lines += currentLine;
		} 
    return lines;
	}
	else { 
    return "File did not open\n"; 
  }
}

void redrawScreen() {
	
  system("cls");
	printf("%s",getFileContents("title.txt"));
	return;
}

void redrawScreen(string screenString) {
	
  system("cls");
	cout << getFileContents("title.txt");	
	char currentChar;

	if (screenString.size() > 40) {
		stringstream screenStringStream(screenString);
		while (screenStringStream) {
			cout << "\n\t";
			for (int i = 0; i < 80; ++i) {
				screenStringStream.get(currentChar);

				if (!screenStringStream)
					return;
				if (currentChar == '\n') {
					cout.put(currentChar);
					i = 0;
					continue;
				}
				cout.put(currentChar);
			}
		}
	}
	return;
}

void saveTimestampURLtoClipboard(set<string>& urls) {
	
  string   saveToClipCmdString{"cat urls.txt | clip"};
  ofstream urlOuputStream{"urls.txt", ofstream::out};
	
  for (string s : urls) {
		urlOuputStream << s << endl;
	}
	urlOuputStream.close();	

	system((const char*)saveToClipCmdString.c_str());
}
