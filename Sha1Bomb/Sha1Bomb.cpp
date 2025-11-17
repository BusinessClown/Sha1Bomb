// Sha1Bomb.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

using namespace std;
// make these options what ever you need to im running late ttyl
string main()
{
	string end;
	while (end != "finished") {
		string menu;
		cout << "Enter the option you want to do: " << endl;
		cin >> menu;
		if (menu == "push_front") {

		}
		else if (menu == "push_back") {

		}
		else if (menu == "pop_front") {

		}
		else if (menu == "pop_back") {

		}
		else if (menu == "front") {

		}
		else if (menu == "back") {

		}
		else if (menu == "empty") {

		}
		else if (menu == "insert") {


		}
		else if (menu == "remove") {

		}
		else if (menu == "find") {

		}
		else if (menu == "end") {
			
			end = "finished";
		}
		else {
			// this is if the user didn't enter one of the prompted options
			cout << "Make sure you enter a valid option" << endl;
		}



	}
	return end;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
