// NT4.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <ctime> // Include the ctime header for time functionality
#include <fstream>
#include <string>
#include <vector>

 //This is used to easily handle the conversions between types without losing data.
 //There is likely a better way but this is quick n easy and can be changed later.
union u_Data
{
	uint64_t U;
	double D;
	void* V;
};

//Used to output characters safely.
void opchr(char p_Char)
{
	if (int(p_Char) == 0)
	{
		std::cout << " ";
		return;
	}

	if (int(p_Char) == 7 || int(p_Char) == 8 || int(p_Char) == 9 || int(p_Char) == 10 || int(p_Char) == 13 || int(p_Char) == 255)
	{
		std::cout << "?";
	}
	else
	{
		std::cout << p_Char;
	}

}




std::string get_Padded(int p_Digits, int p_Num)
{
	std::string tmp_Return;
	int tmp_Digits = 1;
	int tmp_Pad = 0;

	int tmp_Num = p_Num;
	//---std::cout << "\n #." << tmp_Num << ".#";

	while (tmp_Num /= 10)
	{
		tmp_Digits++;
		//---std::cout << "\n ?." << tmp_Digits << ".?";
	}

	tmp_Pad = p_Digits - tmp_Digits;
	//---std::cout << "\n _." << tmp_Pad << "._";

	for (int cou_D = 0; cou_D < tmp_Pad; cou_D++)
	{
		tmp_Return += "0";
		//---std::cout << "\n {" << tmp_Return << "}";
	}

	tmp_Return += std::to_string(p_Num);

	//---std::cout << "\n [" << tmp_Return << "]";

	return tmp_Return;
}


#include "c_Sim.h"