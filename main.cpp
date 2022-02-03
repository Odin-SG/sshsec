#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <regex>
#define LEN_ARR 3

using namespace std;

string exec(const char* cmd) {
	char buffer[256];
	string result = "";
	FILE* pipe = popen(cmd, "r");
	if (!pipe) throw std::runtime_error("popen() failed!");
	try {
		while (fgets(buffer, sizeof buffer, pipe) != NULL) {
			result += buffer;
		}
	} catch (...) {
		pclose(pipe);
		throw;
	}
	pclose(pipe);
	return result;
}

vector<array<int, LEN_ARR>> parseCmdOut(string *cmdOut){
	const regex regular = regex(R"((\d+):(\d+):(\d+))");
	//regex_search(*cmdOut, matches, regular);
	vector<array<int, LEN_ARR>> datesNote;
	array<int, LEN_ARR> dates;
	for(auto i = sregex_iterator(cmdOut->begin(), cmdOut->end(), regular); i != sregex_iterator(); i++){
		smatch matches = *i;
		dates.fill(0);
		for(int j = 0; j < 5; j++)
			dates[j] = atoi((*i)[j+1].str().c_str());
		datesNote.push_back(dates);
	}

	return datesNote;
}

int analysingIncident(vector<array<int, LEN_ARR>> *analysingData){
	array<int, LEN_ARR> *prev = NULL;
	int countPoint = 0;
	for(auto const &val: *analysingData){
		if(prev != NULL){
			if((*prev)[0] == val[0]) countPoint++; else countPoint = 0;
			if(val[1] - (*prev)[1] <= 1) countPoint++;else countPoint = 0;
			if(val[2] - (*prev)[2] <= 15) countPoint++; else countPoint = 0;

			// Debug cout
			/*cout << (*prev)[0] << " " << val[0] << " " << val[0] - (*prev)[0] << endl;
			cout << (*prev)[1] << " " << val[1] << " " << val[1] - (*prev)[1] << endl;
			cout << (*prev)[2] << " " << val[2] << " " << val[2] - (*prev)[2] << "'" << countPoint << "'" << endl << endl;*/
		}
		prev = (array<int,LEN_ARR>*)&val;
	}
	// Сравнивая тройку чисел (3 ввода пароля считаем за 1 попытку входа) мы получим 2 результата (1 со 2 и 2 с 3) сравнения для каждой части даты (часы, минуты, секунды).
	// Следовательно 1 попытка входа генерирует 6 баллов
	// cout << endl << endl << countPoint << endl << endl; //debug string
	if(countPoint >= 12)
		countPoint = 1;
	else
		countPoint = 0;
	return countPoint;
}

int main(int argc, char *argv[]){
	string result = exec("journalctl -u ssh --since '10 min ago' | grep 'Failed password'");
	auto resultParse = parseCmdOut(&result);
	int trigger = analysingIncident(&resultParse);
	if(trigger)
		cout << endl << "Brutforce" << endl;
	else
		cout << endl << "Normal" << endl;
	//cout << result << endl;
}
