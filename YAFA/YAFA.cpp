// -std=c++11   --> C11 <C++ version>
// -std=c++0x   --> gcc 4.8.1
// -std=c++1y   --> gcc 4.9.2
// -std=c++14   --> gcc 5.3.0
// -std=c++17   --> gcc 5.3.0
#include "stdafx.h"
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include <iomanip>
#include <stdio.h>


/**  VARIABLES **/
#define isDev false

/** GETS FILE NAME **/					 std::string getFileName(const std::string &s);
/** GETS FILE SIZE **/					 long long getFileSize(const char* path);
/** CONVERTS long long TO HEX string **/ std::string ll_to_hex(long long d);
/** SHOW USAGE **/						 void showUsage(std::string fileName);
/** ASK REPLACE **/						 bool askReplace();


/** MAIN ENTRY POINT **/
int main(int argc, char** argv) {
	// 1tb = 1099511627775 = ffffffffff
	// 14941919232


	// IS DEV
	if (isDev) {


	}
	
	// NOT DEV
	else {
		if (argc == 1 || argc == 2 ) showUsage(argv[0]);
		else if (argc >= 3) {
			std::string command = argv[1];
			std::string archiveName = argv[argc - 1];

			// ADD FILES TO ARCHIVE
			if (command == "a") {
				if (argc <= 3) showUsage(argv[0]);
				std::string fcnt = std::to_string(argc - 3);
				std::string fileCount = std::string(6 - fcnt.length(), '0') + fcnt;

				try {
					std::ifstream checkfile(archiveName, std::ios::binary);
					if (checkfile.good()) {
						std::cout << "file \"" << argv[argc - 1] << "\" already exists!" << std::endl;
						exit(1);
					}
				} catch (std::ifstream::failure e) {}
				

				std::ofstream outFile(archiveName, std::ios::binary);

				outFile << fileCount;

				int i;
				for (i = 0; i < argc - 3; i++) {
					std::cout << "adding to archive <" << argv[argc - 1] << "> : " << argv[i + 2] << std::endl;

					std::string fileSize = ll_to_hex(getFileSize(argv[i + 2]));
					std::string flnm = getFileName(argv[i + 2]);
					std::string fileName = std::string(255 - flnm.length(), '\0') + flnm;

					outFile << fileSize;
					outFile << fileName;

					std::ifstream inFile(argv[i + 2], std::ios::binary);

					if (fileSize == "0000000000") outFile << '\0';
					else outFile << inFile.rdbuf();

					inFile.close();
				}

				outFile.close();
			}

			// LIST FILES IN ARCHIVE
			else if (command == "l") {
				if (argc != 3) showUsage(argv[0]);
				else {
					std::ifstream inFile(argv[2], std::ios::in | std::ios::binary);
					std::string fileRaw = "";
					char tempChar;
					int xa = 0;
					do {
						inFile.read((char*)&tempChar, 1);
						fileRaw += tempChar;
						xa++;
					} while (xa <= 6);

					long fileCount = strtol(fileRaw.substr(0, 6).c_str(), nullptr, 10);

					int i;
					unsigned long long totalFileBlockSize = 0;
					unsigned long long fileSize = 0;
					std::string fileName = "";
					inFile.seekg(6);

					std::cout << "Contents of " << argv[2] << ": " << std::endl;

					for (i = 0; i < fileCount; i++) {
						/*unsigned long long fileSize = strtoull(fileRaw.substr(6, 18).c_str(), nullptr, 16);
						std::string fileName = fileRaw.substr(18, 253);
						fileName = fileName.substr(fileName.find_last_of('\0') + 1, fileName.npos);*/

						std::string fileSizeRaw = "";
						std::string fileNameRaw = "";

						// BLOCK - BEGIN
						char tempChar;
						int xa = 0;
						do {
							inFile.read((char*)&tempChar, 1);
							fileSizeRaw += tempChar;
							xa++;
						} while (xa <= 10);
						fileSize = strtoull(fileSizeRaw.c_str(), nullptr, 16);
						
						inFile.seekg(totalFileBlockSize + 16);
						xa = 0;
						do {
							inFile.read((char*)&tempChar, 1);
							fileNameRaw += tempChar;
							xa++;
						} while (xa <= 254);
						if (fileSize == 0) totalFileBlockSize += 16 + 255 + fileSize + 1;
						else totalFileBlockSize += 16 + 255 + fileSize;
						// BLOCK - END

						//std::cout << fileNameRaw << std::endl;
						//std::cout << fileSizeRaw << " -- " << fileSize << " -- " << totalFileBlockSize << std::endl;

						fileName = fileNameRaw.substr(fileNameRaw.find_last_of('\0') + 1, fileNameRaw.npos);
						std::cout << std::setw(15) << fileSize << " bytes -- " << fileName <<  std::endl;

						inFile.seekg(totalFileBlockSize);
						totalFileBlockSize -= 6;
					}

					inFile.close();
				}
			}

			// EXTRACT FILES FROM ARCHIVE
			else if (command == "x") {
				if (argc != 3) showUsage(argv[0]);
				else {
					std::ifstream inFile(argv[2], std::ios::in | std::ios::binary);
					std::string fileRaw = "";
					char tempChar;
					int xa = 0;
					do {
						inFile.read((char*)&tempChar, 1);
						fileRaw += tempChar;
						xa++;
					} while (xa <= 6);

					long fileCount = strtol(fileRaw.substr(0, 6).c_str(), nullptr, 10);

					int i;
					unsigned long long totalFileBlockSize = 0;
					unsigned long long fileSize = 0;
					std::string fileName = "";
					inFile.seekg(6);

					for (i = 0; i < fileCount; i++) {
						std::string fileSizeRaw = "";
						std::string fileNameRaw = "";
						std::string fileContent = "";


						/** BLOCK - BEGIN **/
						/* fileSize */
						char tempChar;
						unsigned long long xa = 0;
						do {
							inFile.read((char*)&tempChar, 1);
							fileSizeRaw += tempChar;
							xa++;
						} while (xa <= 10);
						fileSize = strtoull(fileSizeRaw.c_str(), nullptr, 16);
						/* fileName */
						inFile.seekg(totalFileBlockSize + 16);
						xa = 0;
						do {
							inFile.read((char*)&tempChar, 1);
							fileNameRaw += tempChar;
							xa++;
						} while (xa <= 254);
						/* fileContent */
						inFile.seekg(totalFileBlockSize + 16 + 255);
						xa = 0;
						do {
							inFile.read((char*)&tempChar, 1);
							fileContent += tempChar;
							xa++;
						} while (xa < fileSize);

						if (fileSize == 0) totalFileBlockSize += 16 + 255 + fileSize + 1;
						else totalFileBlockSize += 16 + 255 + fileSize;
						/** BLOCK - END **/


						fileName = fileNameRaw.substr(fileNameRaw.find_last_of('\0') + 1, fileNameRaw.npos);
						//std::cout << fileName << " -- " << fileSize << " -- " << fileContent << "\n\n\n\n";

						std::ifstream checkfile(fileName, std::ios::binary);
						if (checkfile.good()) {
							checkfile.close();
							std::cout << "file \"" << fileName << "\" already exists in current directory, do you want to replace it? y/N ";
							if (askReplace()) {
								std::cout << std::endl << "Replacing \"" << fileName << "\"" << std::endl;
							}
						} else {
							std::cout << std::endl << "Extracting \"" << fileName << "\"" << std::endl;
						}
						

						inFile.seekg(totalFileBlockSize);
						totalFileBlockSize -= 6;
					}

					inFile.close();
				}
			}

			// UNKNOWN COMMAND, SHOW USAGE
			else showUsage(argv[0]);
		}
	}
	return 0;
}

/* DEFINITIONS */
void showUsage(std::string fileName) {
	std::cout << "Usage: " << std::endl << "    " << getFileName(fileName) << " a <file(s)...> <archive-name>" << std::endl;
	std::cout << "    " << getFileName(fileName) << " l <archive-name>" << std::endl;
	std::cout << "    " << getFileName(fileName) << " x <archive-name>" << std::endl;
	std::cout << std::endl << "Programmed in Visual Studio (C++) [by 07-IT-17, thanks to 28-IT-17, 54-IT-17, 11-IT-17, 06-IT-17]" << std::endl;
	exit(1);
}

std::string getFileName(const std::string &s) {
	char sep = '/';
#ifdef _WIN32
	sep = '\\';
#endif
	size_t i = s.rfind(sep, s.length());
	return(s.substr(i + 1, s.length() - i));
}

long long getFileSize(const char* path) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	return (long long)file.tellg();
}

std::string ll_to_hex(long long d) {
	std::string results;
	unsigned long long tmp = d;
	char digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	do {
		results += digits[tmp % 16];
		tmp /= 16;
	} while (tmp != 0);
	std::reverse(results.begin(), results.end());
	return (std::string(10 - results.length(), '0') + results);
}

bool askReplace() {
	char c = _getch();
	while (!(c == 'y') || !(c == 'Y') || !(c == 'n') || !(c == 'N') || !(c == '\n') || !(c == '\r')) {
		if (c == 'y' || c == 'Y') {
			std::cout << c;
			return true;
		}
		else if (c == 'n' || c == 'N') {
			std::cout << c;
			return false;
		}
		else if (c == '\n' || c == '\r') return false;
		else c = _getch();
	}
	return false;
}