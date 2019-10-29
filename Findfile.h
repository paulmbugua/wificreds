#include "Header.h"
class findfile {
public:
	std::vector<std::string> files;
	void findfiles(std::string path,std::string name,std::string extension,bool recurs) {
		std::string strsearchdir = path;
		std::vector<std::string> vdirs;
		if (!strsearchdir.empty()) {
			switch (strsearchdir[strsearchdir.size() - 1]) {
			case'\\':
			case'/':
				break;
			default:
				strsearchdir += "\\";
				break;
			}
		}
		WIN32_FIND_DATAA wfd = {};
		HANDLE hfile = FindFirstFileA((strsearchdir+"*").c_str(),&wfd);
		if (INVALID_HANDLE_VALUE == hfile) { return; }
		do {
			std::string strfilename(wfd.cFileName);
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				
				if ((strfilename == ".") || (strfilename == ".."))
				{
					continue;
				}
				if (recurs) {
					vdirs.push_back(strsearchdir + strfilename);
				}
			}
			else {
				if (!extension.empty()){									   
					if (CompareStringA(LOCALE_USER_DEFAULT, NORM_IGNORECASE, strfilename.c_str() + (strfilename.size() - extension.size()), extension.size(), extension.c_str(), extension.size()) != 2)
						continue;
				}
					files.push_back(strsearchdir + strfilename);
				
			}

		} while (FindNextFile(hfile, &wfd));
		if (recurs) {
			for (std::vector<std::string>::iterator iter = vdirs.begin(), end = vdirs.end(); iter != end; ++iter) {
				findfiles(*iter, name, extension, recurs);
			}
		}
	}
};