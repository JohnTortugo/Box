#include <elf.h>

#include <iostream>
#include <queue>
#include <set>
#include <string>

#include "ElfParser.h"

using namespace std;

class ElfLoader {
	private:
		string executablePath;
		char *ldLibraryPath;

		ElfParser mainExecutable;

		vector<ElfParser> sharedLibs;

		/* uma lista com todos os elfs que possuem libs compartilhadas a serem carregadas */
		queue<ElfParser> hasDependencies;
		set<string> alreadyLoadedLibs;

	public:
		ElfLoader(string execPath, char *ldLibPath) : mainExecutable(execPath) {
			this->executablePath = execPath;
			this->ldLibraryPath = ldLibPath;

			printf("Main executable loaded.\n");

			/* inicia carregamento das bibliotecas compartilhadas */
			hasDependencies.push(mainExecutable);
			loadSharedLibs();
			printf("Shared libraries loaded. %d dependencies.\n", sharedLibs.size());
		}

		/* Carrega todas as bibliotecas necessarias a execucao de mainExecutable */
		void loadSharedLibs() {

			while ( !hasDependencies.empty() ) {
				ElfParser elf = hasDependencies.front(); hasDependencies.pop();

				// lista de bibliotecas compartilhadas necessarias para o exec principal
				vector<string> libs = elf.getNeededLibraries();

				// percorre todas as libs necessarias
				for (int i=0; i<libs.size(); i++) {
					string libName = libs[i];

					/* 
					 * verifica se essa biblioteca já nao foi carregada
					 */
					if (alreadyLoadedLibs.find(libName) != alreadyLoadedLibs.end()) {
						continue;
					}

					/* 
					 * procura a biblioteca e se encontrar retorna o caminho de onde encontrou.
					 */
					string libPath = findLibrary(libName, elf.getRpath());

					if (libPath != "") {
						ElfParser lib(libPath + "/" + libName);
						sharedLibs.push_back(lib);
						hasDependencies.push(lib);
						alreadyLoadedLibs.insert(libName);
					} 
					else {
						printf("Library not found: %s\n", libName.c_str());
					}
				}
			}

		}

		/* procura a biblioteca libName nos diretorios padroes */
		string findLibrary(string libName, string Rpath) {
			/* verifica se esta onde a lib disse que estaria */
			if (Rpath != "") {
				//printf("Searching %s in Rpath %s\n", libName.c_str(), Rpath.c_str());
				vector<string> paths = parseLibPath(Rpath);

				// find the needed library
				for (int i=0; i<paths.size(); i++) {
					FILE *fp = fopen((paths[i] + "/" + libName).c_str(), "r");
					if (fp) {
						printf("Lib %s found at %s\n", libName.c_str(), paths[i].c_str());
						fclose(fp);
						return paths[i];
					}
				}
			}

			/* verifica os diretorios padroes do ambiente */
			if (ldLibraryPath != NULL) {
				//printf("Starting search of %s in ld_library_path %s\n", libName.c_str(), ldLibraryPath);
				vector<string> paths = parseLibPath(ldLibraryPath);

				// find the needed library
				for (int i=0; i<paths.size(); i++) {
					FILE *fp = fopen((paths[i] + "/" + libName).c_str(), "r");
					if (fp) {
						printf("Lib %s found at %s\n", libName.c_str(), paths[i].c_str());
						fclose(fp);
						return paths[i];
					}
				}
			}

			/* por fim, verifica em /usr/lib */
			FILE *fp = fopen(("/usr/lib/" + libName).c_str(), "r");
			if (fp) {
				printf("Lib %s found at %s\n", libName.c_str(), "/usr/lib/");
				fclose(fp);
				return "/usr/lib/";
			}

			return "";
		}

		vector<string> parseLibPath(string str) {
			return parseLibPath(str.c_str());
		}

		vector<string> parseLibPath(char *str) {
			vector<string> paths;
			char *aux = strdup(str);
			char *p = strtok(aux, ":");

			while (p != NULL) {
				paths.push_back(p);
				p = strtok(NULL, ":");
			}

			return paths;
		}
};
