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
				v.clips.erase(v.clips.begin(), v.clips.end());
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
			vector<pair<string, short>> vec;
			for(auto& s : map) {
				vec.push_back(s);
			}
			return vec;
		};

		auto cmp = [](pair<string, short> const & tmp1, 
                  pair<string, short> const & tmp2) {
			return tmp1.second > tmp2.second;
		};

		vector<pair<string, short>> vec = store(videos[0].wordCountMap);
		sort(vec.begin(), vec.end(), cmp);		
		ofstream out;
		string fName = "words_";
		fName += videoURL + ".txt";
		char f[30];
		out.open(fName);						
		
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
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void getFileNames(vector<Video>& vids, string firstUrl) {
  
	ifstream  inFile;
	ifstream  urlFile;
	string    ttmp;
	string    utmp;
	string    tmp;
	string    cmd;


	//load in .vtt file titles
	if (selection == 'v' || selection == 'V') {
		cmd = "youtube-dl --get-filename \"" + firstUrl + "\" > filenames.txt";
  }
	
	system((const char*)cmd.c_str());	

	inFile.open("filenames.txt");	

	//INITIALIZE VIDEO CLASSES
	int itmp = 0;

	do {
		getline(inFile, tmp);		
		istringstream iss(tmp);
		char c;
		int i = 0;
		int dotSpot;
		while (iss) {
			iss.get(c);
			if (c == '.')
				dotSpot = i;
			++i;
		}
		
		if (tmp[dotSpot + 1] != 'e' && tmp[dotSpot + 2] != 'n') {
			tmp.erase(tmp.begin() + dotSpot, tmp.end());
    }

		ttmp = tmp;
		tmp += ".en.vtt";
		cout << "tmp: " << tmp;
		Video v(ttmp, tmp, utmp);
		vids.push_back(v);
	} while (inFile.peek() != EOF);

	inFile.close();
}

// clean up original file formatting
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void cleanupFiles(vector<Video>& vids) {
	
  string   systemCall;
	string   tmp;
	ofstream shell;
	int      i = 1;

	for (Video& vid : vids) {
		vid.shellFile = "cleanFile" + to_string(i) + ".sh";
		shell.open(vid.shellFile);
		string tmp = "#!\\bin\\bash\n\n" + sed + "\"" + vid.filename + "\"";
		shell << tmp;
		shell.close();
		system((const char*)vid.shellFile.c_str());

		++i;
	}
	return;
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void getTime(vector<string>& keys, vector<Video>& vids) {
	
  vector<string> lines;
	ifstream       inFile;
	string         temp;
	string         s;
	string         currentWord;
	
	if (firstWordSearch == false) {
		s = '0';
		inFile.open(vids[0].filename);
		if (inFile.fail()) {
			cout << "\n\tCouldn't open file in getTime function";
			system("PAUSE");
			exit(1);
		}
		while (inFile.good()) {
			getline(inFile, temp);
			while (inFile.peek() != EOF) {
				transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
				vids[0].lines.push_back(temp);
				getline(inFile, temp);
			}
		} inFile.close();
	}
	//iterate through lines between '\n's
	short i = 0;
	for (string& line : vids[0].lines) {
		//iterate through keywords
		//parse look lines look for keywords
		istringstream iss(line);

		while (iss) {
			iss >> currentWord;

			if (currentWord == keys[0]) {
				
        try {
					if (isdigit(vids[0].lines[i - 3][0])){
						getClips(vids[0], i - 3);
          }
				} catch (exception& e) { continue; }

				try {
					if (isdigit(vids[0].lines[i - 2][0])){
						getClips(vids[0], i - 2);
          }
				} catch (exception& e) { continue; }

				try {
					if (isdigit(vids[0].lines[i - 1][0])){
						getClips(vids[0], i - 1);
          }
				} catch (exception& e) { continue; }
			}
		}
		++i;
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void getTimestampURLs(Video& vid, int line) {

	short hr = 0;
	short min = 0;
	short sec = 0;

	//calculate timestamp
	try
	{
		if (vid.lines[line][0] != '0') {
			hr += atoi(&vid.lines[line][0]);
    }
		if (vid.lines[line][0] == '0' && vid.lines[line][1] != '0') {
			hr += atoi(&vid.lines[line][1]);
    }
		if (vid.lines[line][3] != '0') {
			min += atoi(&vid.lines[line][3]);
    }
		if (vid.lines[line][3] == '0' && vid.lines[line][4] != '0') {
			min += atoi(&vid.lines[line][4]);
    }
		if (vid.lines[line][6] != '0') {
			sec += atoi(&vid.lines[line][6]);
    }
		if (vid.lines[line][6] == '0' && vid.lines[line][7] != '0') {
			sec += atoi(&vid.lines[line][7]);
    }
	}
	catch (exception& e) {}	

	if ((sec - mention) < 0) {
		--min;
		sec = 60 + (sec - mention);
	}
	string name = vid.url + "&feature=youtu.be&t=" + to_string(hr) + 'h'
		+ to_string(min) + 'm' + to_string(sec - mention) + 's';
	vid.clips.insert(name);

	return;
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void launchUrls(vector<Video>& vids, vector<string> key) {
	
  for (Video& vid : vids) {
		if (vid.clips.size() == 0) {
			cout << "\n\n\tCouldn't find any mentions of "
				<< key[0] << " in this video...";
			return;
		}
		
    string newFile;
		short  switc = 1;
		short  i = 1;
		char   response = 'q';
		
    cout << "\n\n";
		screenText += "\t\"" + key[0] + "\" is mentioned: "
			+ to_string(vid.clips.size()) + " times.";
		if (firstWordSearch == true) screenText += "\n";
		redrawScreen(screenText);
		cout << "\n\n\tPress ENTER to launch first video...";
		_getch();

		if (firstWordSearch == false)
		{
			screenText += "\n\n\t    'S' -- skip"
				"\n\t    'C' -- cancel"
				"\n\t    'A' -- save all to clipboard"
				"\n\t'SPACE' -- next\n";
		}
		redrawScreen(screenText);

		for (string s : vid.clips) {
			if (i > 1) {
        response = _getch();
      }
			if (response == 'a' || response == 'A') {
				saveTimestampURLtoClipboard(vid.clips);
				screenText += "\n\tURL List saved to clipboard";
				redrawScreen(screenText);
				response = _getch();
			}

			if (response == 's' || response == 'S') {
				switc = 0;
      }
			if (response == 'c' || response == 'C') {
				return;
      }
			if (response == 'a' || response == 'A') {
				continue;
      }

			switch (switc) {
      case 0:
				screenText += "\n\tSKIPPING VIDEO " + to_string(i) + "/" + to_string(vid.clips.size());
				redrawScreen(screenText);
				switc = 1;
				break;

			case 1:
				screenText += "\n\tPLAYING VIDEO " + to_string(i) + "/" + to_string(vid.clips.size());
				redrawScreen(screenText);

				newFile = "start chrome \"" + s + "\"";
				system((const char*)newFile.c_str());
				break;

			default:
				screenText += "\n\tINVALID ENTRY!";
				switc = 1;
				--i;
				break;
			}
			++i;
		}
	} return;
}
//////////////////////////////////////////////////////////////////////////////////

string getFileContents(string fileName) {
	
  ifstream File;

	File.open(fileName);
	string   Lines = "";

	if (File.good()) {
		while (File.good()) {
			string TempLine;
			getline(File, TempLine);
			TempLine += "\n";
			Lines += TempLine;
		} return Lines;
	}
	else { return "File did not open\n"; }
}
//////////////////////////////////////////////////////////////////////////////////

void redrawScreen() {
	
  system("cls");
	cout << getFileContents("title.txt");
	return;
}

void redrawScreen(string s) {
	
  system("cls");
	cout << getFileContents("title.txt");
	int count = 0;
	char e;

	if (s.size() > 40) {
		stringstream is(s);
		while (is) {
			cout << "\n\t";
			for (int i = 0; i < 80; ++i) {
				is.get(e);

				if (!is)
					return;
				if (e == '\n') {
					cout.put(e);
					i = 0;
					continue;
				}
				cout.put(e);
			}
		}
	}
	return;
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void saveTimestampURLtoClipboard(set<string>& urls) {
	
  string   saveToClipCmdString{"cat urls.txt | clip"};
  ofstream urlOuputStream{"urls.txt", ofstream::out};
	
  for (string s : urls) {
		urlOuputStream << s << endl;
	}
	urlOuputStream.close();	

	system((const char*)saveToClipCmdString.c_str());
}
