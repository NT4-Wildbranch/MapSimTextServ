class c_Agent
{
public:

	int HP;

	bool flg_Alive;
	bool flg_Spawn;

	float Goal;
	std::string Color;

	float X;
	float Y;
	float Speed;

	std::string Name;

	c_Agent()
	{
		HP = 1;

		flg_Alive = true;
		flg_Spawn = false;

		X = 0;
		Y = 0;

		Speed = 0.01 + ((rand() % 5) * 0.01);

		Goal = 80;
		Color = "Gray";
		Name = "Agent";
	}

	void output()
	{
		std::cout << "\n " << Name;

		std::cout << " X " << X << " Y " << Y << " HP " << HP;
	}

	void update(float p_Temps[5], int p_Max_X, int p_Max_Y)
	{
		float tmp_Best = 99 * 99;
		int tmp_Best_Index = -99;

		float tmp_Current = -99;

		float tmp_MSE[5];

		for (int cou_M = 0; cou_M < 5; cou_M++)
		{
			tmp_MSE[cou_M] = (Goal - p_Temps[cou_M]) * (Goal - p_Temps[cou_M]);
		}

		if (tmp_MSE[0] > tmp_MSE[2]) { X += Speed; }
		if (tmp_MSE[0] < tmp_MSE[2]) { X -= Speed; }

		if (tmp_MSE[1] > tmp_MSE[3]) { Y -= Speed; }
		if (tmp_MSE[1] < tmp_MSE[3]) { Y += Speed; }

		if (rand() % 2)
		{
			if (rand() % 2)
			{
				if (tmp_MSE[0] > tmp_MSE[2]) { X += Speed; }
				if (tmp_MSE[0] < tmp_MSE[2]) { X -= Speed; }
			}
			else
			{
				if (tmp_MSE[1] > tmp_MSE[3]) { Y -= Speed; }
				if (tmp_MSE[1] < tmp_MSE[3]) { Y += Speed; }
			}
		}

		if (X <= 0) { X = 1; }
		if (Y <= 0) { Y = 1; }
		if (X >= p_Max_X) { X = (p_Max_X - 2); }
		if (Y >= p_Max_Y) { Y = (p_Max_Y - 2); }

		tmp_Current = (Goal - p_Temps[4]) * (Goal - p_Temps[4]);

		//std::cout << "\n " << tmp_Current << " = (" << Goal << " - " << p_Temps[4] << ")* (" << Goal << " - " << p_Temps[4] << ")";

		if (tmp_Current > 2000) { HP--; }
		if (tmp_Current < 1000) { HP += 1; }
		if (tmp_Current < 10) { HP += 5; }

		if (HP > 10) { HP = 10; }
		if (HP <= 0) { flg_Alive = false; HP = 0; }
		if (HP >= 9) { flg_Alive = true; }
	}
};

class c_Sim
{
public:

	int Iteration;

	int O2;
	int O2_Delta;

	int Temp;
	int Temp_Delta;

	int O2_Pump;
	int Heater;

	c_Sim()
	{
		Iteration = 0;

		O2 = 5;
		O2_Delta = 1;

		Temp = 5;
		Temp_Delta = 1;

		O2_Pump = 0;
		Heater = 0;
	}

	// Afferent Sensors - Goals
	// O2      | -1/iteration //Breathing   | Goal: 5 | G:5 Y:(4 & 6)      R: (< 4 & > 6)  | Priority: 2  
	// Temp    | -2/iteration //Winter      | Goal: 8 | G:8 Y:(6-7 & 9-10) R: (< 6 & > 10) | Priority: 2  

	// Actuator Systems - Feedback
	// O2 Pump | +4/Iteration [O2]          | -1/Iteration [Temperature] | if (O2 < 5) (Heater = 1) else (Heater = 0)
	// Heater  | +3/Iteration [Temperature] |                            | if (Temp < 8) (Heater = 1) else (Heater = 0)

	int iterate(int p_O2_Pump = -1, int p_Heater = -1)
	{
		Iteration++;

		//Environmental
		int tmp_O2 = O2;
		O2 -= 1; //Breathing

		int tmp_Temp = Temp;
		Temp -= 2; //Winter

		//Calculate actuator effects.
		if (O2_Pump) { O2 += 4; Temp -= 1; } //O2 Pump
		if (Heater) { Temp += 3; } //Heater

		//Calculate the environmental changes based on the previous iterations.
		if (O2 < tmp_O2) { O2_Delta = 0; }
		if (O2 == tmp_O2) { O2_Delta = 1; }
		if (O2 > tmp_O2) { O2_Delta = 2; }

		if (Temp < tmp_Temp) { Temp_Delta = 0; }
		if (Temp == tmp_Temp) { Temp_Delta = 1; }
		if (Temp > tmp_Temp) { Temp_Delta = 2; }

		if ((p_Heater != -1) && (p_O2_Pump != -1))
		{
			Heater = p_Heater;
			O2_Pump = p_O2_Pump;
		}
		else
		{
			//Calculate the actuator state.
			if (tmp_Temp < 8) { Heater = 1; }
			if (tmp_O2 < 5) { O2_Pump = 1; }

			if (tmp_Temp >= 8) { Heater = 0; }
			if (tmp_O2 >= 5) { O2_Pump = 0; }
		}

		return Iteration;
	}

	void output()
	{
		std::cout << "\n";
		std::cout << Iteration;
		std::cout << "\nencode 0 set_input 0 O2 " << O2;
		std::cout << "\nencode 0 set_input 0 O2_Delta " << O2_Delta;

		std::cout << "\nencode 0 set_input 0 Temp " << Temp;
		std::cout << "\nencode 0 set_input 0 Temp_Delta " << Temp_Delta;

		std::cout << "\nencode 0 set_input 0 O2_Pump " << O2_Pump;
		std::cout << "\nencode 0 set_input 0 Heater " << Heater;
	}

	void output_F(std::string p_FName)
	{
		std::ofstream tmp_F;

		tmp_F.open(p_FName, std::ios::app);


		tmp_F << "\n";
		tmp_F << "\noutput_newline";
		tmp_F << "\nset_input 0 ar_O2 " << O2 << " /end/ encode 0 gather_treetop_node 0 ";
		tmp_F << "\nset_input 1 ac_O2_Delta " << O2_Delta << " /end/ encode 1 gather_treetop_node 1 ";
		tmp_F << "\nset_input 2 em_O2_Pump " << O2_Pump << " /end/ encode 2 gather_treetop_node 2 ";

		tmp_F << "\nset_input 3 ar_Temp " << Temp << " /end/ encode 3 gather_treetop_node 3 ";
		tmp_F << "\nset_input 4 ac_Temp_Delta " << Temp_Delta << " /end/ encode 4 gather_treetop_node 4 ";
		tmp_F << "\nset_input 5 em_Heater " << Heater << " /end/ encode 5 gather_treetop_node 5 ";

		tmp_F.close();
	}
};


class c_Cell
{
public:

	float Temp;

	std::vector<float> Factors;

	int Direction;
	float Magnitude_X;
	float Magnitude_Y;

	int Type;

	c_Cell(int p_Factor_Count = 1)
	{
		Temp = 0;
		Type = 1;
		Direction = 0;

		Magnitude_X = 0;
		Magnitude_Y = 0;

		Factors.resize(p_Factor_Count);
	}


};

class c_Sensor
{
public:

	int X;
	int Y;
	float Previous;
	float Delta;
	float Current;

	c_Sensor()
	{
		X = 0;
		Y = 0;
		Previous = 0;
		Current = 0;
		Delta = 0;
	}

	float get_Delta()
	{
		return Delta;
	}

	void set_Current(float p_Current)
	{
		Previous = Current;
		Current = p_Current;

		Delta = Current - Previous;
	}
};

class c_Actuator
{
public:

	int X;
	int Y;

	bool flg_On_Off;

	float On_Temp;

	float Current;

	c_Actuator()
	{
		X = 0;
		Y = 0;
		Current = 0;
		On_Temp = 100;

		flg_On_Off = false;
	}

	int is_On(){ if (flg_On_Off) { return 1; } else { return -1; } }

	bool turn_Off() { flg_On_Off = false; return flg_On_Off; }
	bool turn_On() { flg_On_Off = true; return flg_On_Off; }

	float get_Temp() { return On_Temp; }
};

class c_Historical_DB
{
	std::vector<std::vector<std::vector<u_Data>>> Hist;
	int Chrono_Depth;
	int Raw_Depth;

public:

	c_Historical_DB()
	{
		Chrono_Depth = 0;
		Raw_Depth = 0;
	}

	void reset()
	{
		Hist.clear();
	}

	void set_Chrono_Depth(int p_Chrono_Depth)
	{
		Chrono_Depth = p_Chrono_Depth;
	}

	void set_Raw_Depth(int p_Raw_Depth)
	{
		Raw_Depth = p_Raw_Depth;
	}

	int get_Chrono_Depth()
	{
		return Chrono_Depth;
	}

	int get_Raw_Depth()
	{
		return Raw_Depth;
	}

	std::vector<u_Data> get_Entry(int p_Step, int p_Raw)
	{
		//Build the entry.
		std::vector<u_Data> return_Data;

		for (int cou_Chrono = 0; cou_Chrono < Chrono_Depth; cou_Chrono++)
		{
			//---std::cout << "\n Chrono[" << cou_Chrono << "]";
			//---std::cout << " " << Hist[cou_Step][cou_Chrono][cou_Raw].D;
			return_Data.push_back(Hist[p_Step][cou_Chrono][p_Raw]);
		}

		return return_Data;
	}

	void add_To_Hist(std::vector<std::vector<u_Data>> p_Data)
	{
		//So we need history for every frame
		//One datatype per history DB
		//We store the projection associated with each index
		//As we step away from the peojections we do the comparison the ground truth to get the scores

		//One frame per tick so far, this is pushed on.
		//One array per frame to hold the prediction or current stats.

		//This tick.
		//Hist.push_back(std::vector<std::vector<u_Data>>());
		Hist.push_back(p_Data);
		return;

		//Hold the current frame chrono block depth.
		Hist[Hist.size() - 1].resize(p_Data.size());

		//Copy the frame data over.
		for (int cou_Chrono = 0; cou_Chrono < p_Data.size(); cou_Chrono++)
		{
			Hist[Hist.size() - 1][cou_Chrono].resize(p_Data[cou_Chrono].size());
			for (int cou_Raw = 0; cou_Raw < p_Data[cou_Chrono].size(); cou_Raw++)
			{
				Hist[Hist.size() - 1][cou_Chrono][cou_Raw] = p_Data[cou_Chrono][cou_Raw];
			}
		}
	}

	int get_Entry_Count()
	{
		return Hist.size();
	}

	void output_DB()
	{
		std::cout << "\n\n output_DB()";
		for (int cou_Step = 0; cou_Step < Hist.size(); cou_Step++)
		{
			std::cout << "\n Index[" << cou_Step << "]";
			for (int cou_Raw = 0; cou_Raw < Raw_Depth; cou_Raw++)
			{
				std::cout << "\n ... Raw[" << cou_Raw << "]";
				for (int cou_Chrono = 0; cou_Chrono < Chrono_Depth; cou_Chrono++)
				{
					//---std::cout << "\n Chrono[" << cou_Chrono << "]";
					std::cout << " " << Hist[cou_Step][cou_Chrono][cou_Raw].D;
				}
			}
		}
	}
};

class c_Map_Sim
{
	std::vector<std::vector<std::vector<c_Cell>>> Map;
	bool Current_Frame;
	bool Next_Frame;

	std::vector<c_Sensor> Sensors;

	std::vector<c_Actuator> Actuators;

	int Width;

	int Height;

	std::string Filename;

	int Tick;

	std::vector<c_Agent> The_Outsiders;

public:

	c_Map_Sim(int p_Width, int p_Height)
	{
		Current_Frame = 0;
		Next_Frame = 1;

		Filename = "./Scripts/";

		new_Map(p_Width, p_Height);

		Tick = 0;
	}

	c_Map_Sim()
	{
		Current_Frame = 0;
		Next_Frame = 1;
		Filename = "./Scripts/";

		Tick = 0;
	}

	void init_Agents(int p_Count)
	{
		The_Outsiders.resize(p_Count * 3);

		for (int cou_Index = 0; cou_Index < p_Count; cou_Index++)
		{
			The_Outsiders[cou_Index].X = (rand() % Width);
			The_Outsiders[cou_Index].Y = (rand() % Height);
			The_Outsiders[cou_Index].Name = "Agent_" + std::to_string(cou_Index);
			The_Outsiders[cou_Index].Goal = 70;
			The_Outsiders[cou_Index].Color = "blue";
		}

		for (int cou_Index = p_Count; cou_Index < (p_Count * 2); cou_Index++)
		{
			The_Outsiders[cou_Index].X = (rand() % Width);
			The_Outsiders[cou_Index].Y = (rand() % Height);
			The_Outsiders[cou_Index].Name = "Agent_" + std::to_string(cou_Index);
			The_Outsiders[cou_Index].Goal = 70;
			The_Outsiders[cou_Index].Color = "green";
		}

		for (int cou_Index = (p_Count * 2); cou_Index < (p_Count * 3); cou_Index++)
		{
			The_Outsiders[cou_Index].X = (rand() % Width);
			The_Outsiders[cou_Index].Y = (rand() % Height);
			The_Outsiders[cou_Index].Name = "Agent_" + std::to_string(cou_Index);
			The_Outsiders[cou_Index].Goal = 50;
			The_Outsiders[cou_Index].Color = "red";
		}
	}

	//B 70
	//G 70
	//R 50

	int get_Agent_Count()
	{
		return The_Outsiders.size();
	}

	int get_Width() { return Width; }
	int get_Height() { return Height; }

	void swap_Frame()
	{
		Current_Frame = !Current_Frame;
		Next_Frame = !Current_Frame;
		//std::cout << "\n[" << Current_Frame << " " << Next_Frame << "]";
	}

	void set_Agent_XY(int p_Agent, float p_X, float p_Y)
	{
		The_Outsiders[p_Agent].X = p_X;
		The_Outsiders[p_Agent].Y = p_Y;
	}

	void set_Agent_HP(int p_Agent, int p_HP)
	{
		The_Outsiders[p_Agent].HP = p_HP;
	}

	int add_Sensor(int p_X, int p_Y)
	{
		Sensors.push_back(c_Sensor());

		Sensors[Sensors.size() - 1].X = p_X;
		Sensors[Sensors.size() - 1].Y = p_Y;

		Map[Current_Frame][p_X][p_Y].Type = 2;
		Map[Next_Frame][p_X][p_Y].Type = 2;

		return int(Sensors.size()) - 1;
	}

	int add_Actuator(int p_X, int p_Y, float p_Temp)
	{
		Actuators.push_back(c_Actuator());

		Actuators[Actuators.size() - 1].X = p_X;
		Actuators[Actuators.size() - 1].Y = p_Y;
		Actuators[Actuators.size() - 1].On_Temp = p_Temp;

		Map[Current_Frame][p_X][p_Y].Type = 3;
		Map[Next_Frame][p_X][p_Y].Type = 3;

		return int(Actuators.size()) - 1;
	}

	int get_Actuator_State(int p_Actuator)
	{
		return Actuators[p_Actuator].is_On();
	}

	float get_Actuator_Temp(int p_Actuator)
	{
		return Actuators[p_Actuator].get_Temp();
	}

	void turn_Actuator_On(int p_Actuator)
	{
		Actuators[p_Actuator].turn_On();
	}

	void turn_Actuator_Off(int p_Actuator)
	{
		Actuators[p_Actuator].turn_Off();
	}
	
	float get_Sensor_Data(int p_Sensor)
	{
		Sensors[p_Sensor].set_Current(Map[Current_Frame][Sensors[p_Sensor].X][Sensors[p_Sensor].Y].Temp);
		return Sensors[p_Sensor].Current;
	}
	
	float get_Sensor_Delta(int p_Sensor)
	{
		return Sensors[p_Sensor].get_Delta();
	}

	void new_Map(int p_Width, int p_Height)
	{
		Width = p_Width;
		Height = p_Height;

		std::cout << "\n Creating Map(" << Width << ", " << Height << ")";

		Map.resize(2);

		Map[0].resize(p_Width);
		Map[1].resize(p_Width);
		for (int cou_X = 0; cou_X < p_Width; cou_X++)
		{
			Map[0][cou_X].resize(p_Height);
			Map[1][cou_X].resize(p_Height);
		}

		std::cout << "\n Finished Map";
	}

	void reset_Tick()
	{
		Tick = 0;
	}

	int get_Tick()
	{
		return Tick;
	}

	void set_Temp(int p_X, int p_Y, float p_Temp)
	{
		Map[Current_Frame][p_X][p_Y].Temp = p_Temp;
	}

	void set_Map_Temp(float p_Temp)
	{
		for (int cou_X = 0; cou_X < Width; cou_X++)
		{
			for (int cou_Y = 0; cou_Y < Height; cou_Y++)
			{
				set_Temp(cou_X, cou_Y, p_Temp);
			}
		}
	}

	void set_Type(int p_X, int p_Y, int p_Type)
	{
		Map[Current_Frame][p_X][p_Y].Type = p_Type;
		Map[Next_Frame][p_X][p_Y].Type = p_Type;
	}

	bool check_XY(int p_X, int p_Y)
	{
		if (p_X < 0) { return 0; }
		if (p_Y < 0) { return 0; }
		if (p_X >= Width) { return 0; }
		if (p_Y >= Height) { return 0; }

		if (Map[Current_Frame][p_X][p_Y].Type == 0) { return 0; }

		return 1;
	}

	void diffuse_Temp(int p_X, int p_Y)
	{
		float tmp_Total = 0.0;
		float tmp_Count = 0.0;
		float tmp_Current = 0.0;
		float tmp_Flow = 0.0;

		if (!check_XY(p_X, p_Y)) { return; }
		
		//Find the difference for each side. Average the lowest only.
		tmp_Current = Map[Current_Frame][p_X][p_Y].Temp;
		float tmp_Current_Diff = 0;
		
		Map[Next_Frame][p_X][p_Y].Direction = -1;
		Map[Next_Frame][p_X][p_Y].Magnitude_X = 0;
		Map[Next_Frame][p_X][p_Y].Magnitude_Y = 0;
		if (check_XY(p_X + 1, p_Y)) { if ((tmp_Current - Map[Current_Frame][p_X + 1][p_Y].Temp) > tmp_Current_Diff) { tmp_Current_Diff = (tmp_Current - Map[Current_Frame][p_X + 1][p_Y].Temp); Map[Next_Frame][p_X][p_Y].Direction = 0; } }
		if (check_XY(p_X, p_Y + 1)) { if ((tmp_Current - Map[Current_Frame][p_X][p_Y + 1].Temp) > tmp_Current_Diff) { tmp_Current_Diff = (tmp_Current - Map[Current_Frame][p_X][p_Y + 1].Temp); Map[Next_Frame][p_X][p_Y].Direction = 1; } }
		if (check_XY(p_X - 1, p_Y)) { if ((tmp_Current - Map[Current_Frame][p_X - 1][p_Y].Temp) > tmp_Current_Diff) { tmp_Current_Diff = (tmp_Current - Map[Current_Frame][p_X - 1][p_Y].Temp); Map[Next_Frame][p_X][p_Y].Direction = 2; } }
		if (check_XY(p_X, p_Y - 1)) { if ((tmp_Current - Map[Current_Frame][p_X][p_Y - 1].Temp) > tmp_Current_Diff) { tmp_Current_Diff = (tmp_Current - Map[Current_Frame][p_X][p_Y - 1].Temp); Map[Next_Frame][p_X][p_Y].Direction = 3; } }

		tmp_Current = Map[Current_Frame][p_X][p_Y].Temp;

		int tmp_XX = 0;
		int tmp_YY = 0;

		Map[Next_Frame][p_X][p_Y].Magnitude_X = 0;
		Map[Next_Frame][p_X][p_Y].Magnitude_Y = 0;


		if (check_XY(p_X + 1, p_Y)) 
		{
			Map[Next_Frame][p_X][p_Y].Magnitude_X = (Map[Current_Frame][p_X + 1][p_Y].Temp - tmp_Current);
		}

		if (check_XY(p_X - 1, p_Y)) 
		{
			Map[Next_Frame][p_X][p_Y].Magnitude_X -= (Map[Current_Frame][p_X - 1][p_Y].Temp - tmp_Current);
		}
		
		if (check_XY(p_X, p_Y + 1)) 
		{
			Map[Next_Frame][p_X][p_Y].Magnitude_Y = (Map[Current_Frame][p_X][p_Y + 1].Temp - tmp_Current);
		}

		if (check_XY(p_X, p_Y - 1)) 
		{
			Map[Next_Frame][p_X][p_Y].Magnitude_Y -= (Map[Current_Frame][p_X][p_Y - 1].Temp - tmp_Current);
		}


		/*
		if ((tmp_Direction == 0) && (check_XY(p_X + 1, p_Y)))
		{
			tmp_Flow = (tmp_Current + Map[Current_Frame][p_X + 1][p_Y].Temp) / 2;
		}
		
		if ((tmp_Direction == 1) && (check_XY(p_X, p_Y + 1)))
		{
			tmp_Flow = (tmp_Current + Map[Current_Frame][p_X][p_Y + 1].Temp) / 2;
		}
		
		if ((tmp_Direction == 2) && (check_XY(p_X - 1, p_Y)))
		{
			tmp_Flow = (tmp_Current + Map[Current_Frame][p_X - 1][p_Y].Temp) / 2;
		}
		
		if ((tmp_Direction == 3) && (check_XY(p_X, p_Y - 1)))
		{
			tmp_Flow = (tmp_Current + Map[Current_Frame][p_X][p_Y - 1].Temp) / 2;
		}

		*/
		
		//std::cout << "\n C: " << Map[Current_Frame][p_X][p_Y].Temp;
		tmp_Current = Map[Current_Frame][p_X][p_Y].Temp;
		if (check_XY(p_X + 1, p_Y)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X + 1][p_Y].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X, p_Y + 1)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X][p_Y + 1].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X - 1, p_Y)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X - 1][p_Y].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X, p_Y - 1)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X][p_Y - 1].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }

		tmp_Current = Map[Current_Frame][p_X][p_Y].Temp;
		if (check_XY(p_X, p_Y - 1)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X][p_Y - 1].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X + 1, p_Y)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X + 1][p_Y].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X, p_Y + 1)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X][p_Y + 1].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X - 1, p_Y)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X - 1][p_Y].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }

		tmp_Current = Map[Current_Frame][p_X][p_Y].Temp;
		if (check_XY(p_X - 1, p_Y)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X - 1][p_Y].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X, p_Y - 1)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X][p_Y - 1].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X + 1, p_Y)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X + 1][p_Y].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X, p_Y + 1)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X][p_Y + 1].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }

		tmp_Current = Map[Current_Frame][p_X][p_Y].Temp;
		if (check_XY(p_X, p_Y + 1)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X][p_Y + 1].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X - 1, p_Y)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X - 1][p_Y].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X, p_Y - 1)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X][p_Y - 1].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }
		if (check_XY(p_X + 1, p_Y)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X + 1][p_Y].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }

		//tmp_Current = Map[Current_Frame][p_X][p_Y].Temp;
		//if (check_XY(p_X + 1, p_Y)) { tmp_Current = (tmp_Current + Map[Current_Frame][p_X + 1][p_Y].Temp) / 2; tmp_Count++; tmp_Total += tmp_Current; }

		Map[Next_Frame][p_X][p_Y].Temp = (tmp_Total /= tmp_Count);
		
		//std::cout << "\n C: " << Map[Current_Frame][p_X][p_Y].Temp;
		//std::cout << "\n N: " << Map[Next_Frame][p_X][p_Y].Temp;
	}

	float get_Temp(int p_X, int p_Y)
	{
		return Map[Current_Frame][p_X][p_Y].Temp;
	}

	void chill_Map(float p_Chill)
	{
		for (int cou_X = 0; cou_X < Width; cou_X++)
		{
			for (int cou_Y = 0; cou_Y < Height; cou_Y++)
			{
				Map[Current_Frame][cou_X][cou_Y].Temp -= p_Chill;
				if (Map[Current_Frame][cou_X][cou_Y].Temp < 0){ Map[Current_Frame][cou_X][cou_Y].Temp = 0; }
			}
		}
	}

	void update()
	{
		for (int cou_Index = 0; cou_Index < Actuators.size(); cou_Index++)
		{
			if (Actuators[cou_Index].is_On() == 1)
			{
				Map[Current_Frame][Actuators[cou_Index].X][Actuators[cou_Index].Y].Temp = Actuators[cou_Index].get_Temp();
			}
		}

		for (int cou_X = 0; cou_X < (Width); cou_X++)
		{
			for (int cou_Y = 0; cou_Y < (Height); cou_Y++)
			{
				diffuse_Temp(cou_X, cou_Y);
			}
		}

		float tmp_Temps[5] = { -99, -99, -99, -99, -99 };

		//std::cout << "\n";
		for (int cou_Index = 0; cou_Index < The_Outsiders.size(); cou_Index++)
		{

			tmp_Temps[0] = -99;
			tmp_Temps[1] = -99;
			tmp_Temps[2] = -99;
			tmp_Temps[3] = -99;
			tmp_Temps[4] = -99;

			if (check_XY(The_Outsiders[cou_Index].X - 1, The_Outsiders[cou_Index].Y)) { tmp_Temps[0] = Map[Current_Frame][The_Outsiders[cou_Index].X - 1][The_Outsiders[cou_Index].Y].Temp; }
			if (check_XY(The_Outsiders[cou_Index].X, The_Outsiders[cou_Index].Y + 1)) { tmp_Temps[1] = Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y + 1].Temp; }
			if (check_XY(The_Outsiders[cou_Index].X + 1, The_Outsiders[cou_Index].Y)) { tmp_Temps[2] = Map[Current_Frame][The_Outsiders[cou_Index].X + 1][The_Outsiders[cou_Index].Y].Temp; }
			if (check_XY(The_Outsiders[cou_Index].X, The_Outsiders[cou_Index].Y - 1)) { tmp_Temps[3] = Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y - 1].Temp; }
			if (check_XY(The_Outsiders[cou_Index].X, The_Outsiders[cou_Index].Y)) { tmp_Temps[4] = Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Temp; }
			//std::cout << "\n MT " << Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Temp;
			//std::cout << "\n TM " << tmp_Temps[4];
			The_Outsiders[cou_Index].update(tmp_Temps, Width, Height);

			The_Outsiders[cou_Index].X -= (The_Outsiders[cou_Index].Speed * Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Magnitude_X) * (The_Outsiders[cou_Index].Speed * Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Magnitude_X);
			
			if (The_Outsiders[cou_Index].X >= Width) { The_Outsiders[cou_Index].X = Width - 1; }
			if (The_Outsiders[cou_Index].X <= 0) { The_Outsiders[cou_Index].X = 1; }

			The_Outsiders[cou_Index].Y -= (The_Outsiders[cou_Index].Speed * Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Magnitude_Y) * (The_Outsiders[cou_Index].Speed * Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Magnitude_Y);

			if (The_Outsiders[cou_Index].Y >= Height) { The_Outsiders[cou_Index].Y = Height - 1; }
			if (The_Outsiders[cou_Index].Y <= 0) { The_Outsiders[cou_Index].Y = 1; }

			if (The_Outsiders[cou_Index].flg_Alive == false) { continue; }
			/*
			if (Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Direction == 0)
			{
				The_Outsiders[cou_Index].Y += The_Outsiders[cou_Index].Speed * Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Magnitude;
				if (The_Outsiders[cou_Index].Y >= Height) { The_Outsiders[cou_Index].Y = Height - 1; }
			}

			if (Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Direction == 1)
			{
				The_Outsiders[cou_Index].X += The_Outsiders[cou_Index].Speed * Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Magnitude;
				if (The_Outsiders[cou_Index].X >= Width) { The_Outsiders[cou_Index].X = Width - 1; }
			}

			if (Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Direction == 2)
			{
				The_Outsiders[cou_Index].Y -= The_Outsiders[cou_Index].Speed * Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Magnitude;
				if (The_Outsiders[cou_Index].Y <= 0) { The_Outsiders[cou_Index].Y = 1; }
			}

			if (Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Direction == 3)
			{
				The_Outsiders[cou_Index].X -= The_Outsiders[cou_Index].Speed * Map[Current_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Magnitude;
				if (The_Outsiders[cou_Index].X <= 0) { The_Outsiders[cou_Index].X = 1; }
			}*/

			if (check_XY(The_Outsiders[cou_Index].X, The_Outsiders[cou_Index].Y))
			{
				if (The_Outsiders[cou_Index].Color == "red")
				{
					Map[Next_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Temp += 2;
				}
				if (The_Outsiders[cou_Index].Color == "blue")
				{
					Map[Next_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Temp++;
				}
				if (The_Outsiders[cou_Index].Color == "green")
				{
					Map[Next_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Temp--;
					Map[Next_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Temp--;
					Map[Next_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Temp--;
					Map[Next_Frame][The_Outsiders[cou_Index].X][The_Outsiders[cou_Index].Y].Temp--;
				}
			}

		}


		swap_Frame();

		Tick++;
	}

	void output_Agents()
	{
		std::cout << "\n";
		for (int cou_Index = 0; cou_Index < The_Outsiders.size(); cou_Index++)
		{
			The_Outsiders[cou_Index].output();
		}
	}

	void view_Map()
	{
		std::string tmp_Out = "\n";
		for (int cou_X = 0; cou_X < Width; cou_X++)
		{
			tmp_Out += "\n[";
			for (int cou_Y = 0; cou_Y < Height; cou_Y++)
			{
				//std::cout << "[" << Map[Current_Frame][cou_X][cou_Y].Temp << "]";

				if (Map[Current_Frame][cou_X][cou_Y].Type == 0)
				{
					tmp_Out += char(178);
					continue;
				}
				if (Map[Current_Frame][cou_X][cou_Y].Type == 2)
				{
					tmp_Out += 'S';
					continue;
				}
				if (Map[Current_Frame][cou_X][cou_Y].Type == 3)
				{
					tmp_Out += 'A';
					continue;
				}
				if (char(Map[Current_Frame][cou_X][cou_Y].Temp) > 20)
				{
					tmp_Out += char(Map[Current_Frame][cou_X][cou_Y].Temp);
				}
				else
				{
					tmp_Out += " ";
				}
			}
			tmp_Out += "]";
			if (cou_X < Sensors.size())
			{
				tmp_Out += " S[" + std::to_string(cou_X) + "]: [";
				tmp_Out = tmp_Out + "(" + std::to_string(Sensors[cou_X].X) + ", " + std::to_string(Sensors[cou_X].Y) + ") ";
				tmp_Out += "\t" + std::to_string(get_Sensor_Data(cou_X) + 0.000001);
				tmp_Out += "   \tDelta: " + std::to_string(get_Sensor_Delta(cou_X));
				tmp_Out += "] ";
			}
			if (cou_X > Sensors.size())
			{
				int tmp_Index = (cou_X - Sensors.size() - 1);

				if (tmp_Index < Actuators.size())
				{
					tmp_Out += " A[" + std::to_string(tmp_Index) + "]: [";
					if (get_Actuator_State(tmp_Index) == -1) { tmp_Out += " ----[OFF] "; }
					if (get_Actuator_State(tmp_Index) == 0) { tmp_Out += " [BROKEN] "; }
					if (get_Actuator_State(tmp_Index) == 1) { tmp_Out += " [ON]---- "; }
					//tmp_Out += " On/Off: " + std::to_string(get_Actuator_State(tmp_Index));
					tmp_Out += " Temp: " + std::to_string(get_Actuator_Temp(tmp_Index));
					tmp_Out = tmp_Out + "(" + std::to_string(Actuators[tmp_Index].X) + ", " + std::to_string(Actuators[tmp_Index].Y) + ") ";
					tmp_Out += "] ";
				}
			}
		}
		//tmp_Out += "\n";

		std::cout << tmp_Out;
	}

	
	void write_Map_Tiles(std::string p_FName)
	{
		std::ofstream SF;
		std::string tmp_FName = p_FName + "." + get_Padded(10, Tick) + ".MAP.ssv";
		SF.open(tmp_FName, std::ios::trunc);
		
		std::ofstream SFL;
		std::string tmp_LFName = p_FName + "." + get_Padded(10, Tick) + ".MAP.lbl";
		SFL.open(tmp_LFName, std::ios::trunc);

		std::vector<std::vector<float>> tmp_Map;
		tmp_Map.resize(Width);

		for (int cou_X = 0; cou_X < Width; cou_X++)
		{
			tmp_Map[cou_X].resize(Height);
		}

		for (int cou_X = 0; cou_X < Width; cou_X++)
		{
			for (int cou_Y = 0; cou_Y < Height; cou_Y++)
			{
				tmp_Map[cou_X][cou_Y] = Map[Current_Frame][cou_X][cou_Y].Temp;
			}
		}
		
		for (int cou_S = 0; cou_S < Sensors.size(); cou_S++)
		{
			tmp_Map[Sensors[cou_S].X][Sensors[cou_S].Y] = 0.0;
			SFL << "S_" << std::to_string(cou_S) << " " << Sensors[cou_S].Y << " " << Sensors[cou_S].X << " 8 white [" << int(Sensors[cou_S].Current) << "]\n";
		}

		for (int cou_A = 0; cou_A < Actuators.size(); cou_A++)
		{
			tmp_Map[Actuators[cou_A].X][Actuators[cou_A].Y] = 500.0;
			if (Actuators[cou_A].flg_On_Off == 1)
			{
				SFL << "A_" << std::to_string(cou_A) << " " << Actuators[cou_A].Y << " " << Actuators[cou_A].X << " 8 red [" << Actuators[cou_A].get_Temp() << "]\n";
			}
			if (Actuators[cou_A].flg_On_Off == 0)
			{
				SFL << "A_" << std::to_string(cou_A) << " " << Actuators[cou_A].Y << " " << Actuators[cou_A].X << " 8 gray [" << Actuators[cou_A].get_Temp() << "]\n";
			}
		}

		for (int cou_Agent = 0; cou_Agent < The_Outsiders.size(); cou_Agent++)
		{
			if (The_Outsiders[cou_Agent].flg_Alive == 0) { continue; }

			SFL << "A" << cou_Agent << " " << The_Outsiders[cou_Agent].Y << " " << The_Outsiders[cou_Agent].X << " 4 " << The_Outsiders[cou_Agent].Color << " +\n";
		}

		for (int cou_X = 0; cou_X < Width; cou_X++)
		{
			if (cou_X > 0)
			{
				SF << "\n";
			}
			SF << Tick << " ";
			for (int cou_Y = 0; cou_Y < Height; cou_Y++)
			{
				SF << tmp_Map[cou_X][cou_Y] << " ";
			}
		}

		SF.close();
		SFL.close();
	}

	void write_Map_Temp(int p_Sensor, std::string p_FName)
	{
		std::ofstream SF;
		std::string tmp_FName = p_FName + "." + std::to_string(p_Sensor) + ".ssv";
		SF.open(tmp_FName, std::ios::app);

		SF << Tick << " " << get_Sensor_Data(p_Sensor);

		SF << "\n";
		SF.close();
	}

	void write_Map_Temps(std::string p_FName)
	{
		float tmp_Total = 0;

		for (int cou_S = 0; cou_S < Sensors.size(); cou_S++)
		{
			write_Map_Temp(cou_S, p_FName);
			tmp_Total += get_Sensor_Data(cou_S);
		}

		tmp_Total /= Sensors.size();

		std::ofstream SF;
		std::string tmp_FName = p_FName + ".Temp_Avg.ssv";
		SF.open(tmp_FName, std::ios::app);

		SF << Tick << " " << tmp_Total;

		SF << "\n";
		SF.close();
	}

	void write_Map_MSE(int p_Sensor, std::string p_FName)
	{
		std::ofstream SF;
		std::string tmp_FName = p_FName + "." + std::to_string(p_Sensor) + ".ssv";
		SF.open(tmp_FName, std::ios::app);

		SF << Tick << " " << ((80 - get_Sensor_Data(p_Sensor)) * (80 - get_Sensor_Data(p_Sensor)));

		SF << "\n";
		SF.close();
	}

	void write_Map_MSE(std::string p_FName)
	{
		float tmp_Total_MSE = 0;
		for (int cou_S = 0; cou_S < Sensors.size(); cou_S++)
		{
			write_Map_MSE(cou_S, p_FName);

			tmp_Total_MSE += ((80 - get_Sensor_Data(cou_S)) * (80 - get_Sensor_Data(cou_S)));
		}

		tmp_Total_MSE /= Sensors.size();

		std::ofstream SF;
		std::string tmp_FName = p_FName + ".MSE_Avg.ssv";
		SF.open(tmp_FName, std::ios::app);

		SF << Tick << " " << tmp_Total_MSE;

		SF << "\n";
		SF.close();
	}

	int get_Actuator_Count()
	{
		return Actuators.size();
	}

	int get_Sensor_Count()
	{
		return Sensors.size();
	}

	void draw_Building(int p_X0, int p_Y0, int p_X1, int p_Y1)
	{
		for (int cou_X = p_X0; cou_X <= p_X1; cou_X++)
		{
			set_Type(cou_X, p_Y0, 0);
			set_Type(cou_X, p_Y1, 0);
		}

		for (int cou_Y = p_Y0; cou_Y <= p_Y1; cou_Y++)
		{
			set_Type(p_X0, cou_Y, 0);
			set_Type(p_X1, cou_Y, 0);
		}

		//p_Map->set_Type(23, 20, 1);
	}
};

void draw_Building(c_Map_Sim * p_Map, int p_X0, int p_Y0, int p_X1, int p_Y1)
{
	for (int cou_X = p_X0; cou_X <= p_X1; cou_X++)
	{
		p_Map->set_Type(cou_X, p_Y0, 0);
		p_Map->set_Type(cou_X, p_Y1, 0);
	}

	for (int cou_Y = p_Y0; cou_Y <= p_Y1; cou_Y++)
	{
		p_Map->set_Type(p_X0, cou_Y, 0);
		p_Map->set_Type(p_X1, cou_Y, 0);
	}

	//p_Map->set_Type(23, 20, 1);
}

void runmap()
{
	c_Map_Sim Map(60, 175);

	int tmp_X = 0;
	int tmp_Y = 0;
	float tmp_Temp = 0.0;
	float tmp_Sensor = 0;
	bool flg_Ranran = false;
	bool flg_Heater = false;
	bool flg_AC = false;


	draw_Building(&Map, 10, 10, 40, 40);
	draw_Building(&Map, 10, 45, 40, 75);
	draw_Building(&Map, 10, 80, 40, 110);
	draw_Building(&Map, 10, 115, 40, 145);

	draw_Building(&Map, 9, 9, 46, 146);
	draw_Building(&Map, 40, 10, 45, 145);

	draw_Building(&Map, 46, 10, 56, 20);

	Map.set_Type(11, 40, 1);
	Map.set_Type(11, 75, 1);
	Map.set_Type(11, 110, 1);
	Map.set_Type(11, 145, 1);

	Map.set_Type(39, 40, 1);
	Map.set_Type(39, 75, 1);
	Map.set_Type(39, 110, 1);
	Map.set_Type(39, 145, 1);

	Map.set_Type(11, 45, 1);
	Map.set_Type(11, 80, 1);
	Map.set_Type(11, 115, 1);

	Map.set_Type(9, 25, 1);
	Map.set_Type(10, 25, 1);

	Map.set_Type(40, 41, 1);
	Map.set_Type(40, 42, 1);
	Map.set_Type(40, 43, 1);
	Map.set_Type(40, 44, 1);

	Map.set_Type(40, 76, 1);
	Map.set_Type(40, 77, 1);
	Map.set_Type(40, 78, 1);
	Map.set_Type(40, 79, 1);

	Map.set_Type(40, 111, 1);
	Map.set_Type(40, 112, 1);
	Map.set_Type(40, 113, 1);
	Map.set_Type(40, 114, 1);

	Map.set_Type(39, 146, 1);


	Map.set_Type(45, 111, 1);
	Map.set_Type(45, 112, 1);
	Map.set_Type(45, 113, 1);
	Map.set_Type(46, 112, 1);


	for (int cou_Y = 13; cou_Y < 19; cou_Y++)
	{
		Map.set_Type(45, cou_Y, 1);
		Map.set_Type(46, cou_Y, 1);
	}

	//Map.set_Type(5, 5, 1);
	//Map.set_Type(45, 45, 1);

	Map.add_Sensor(11, 30);
	Map.add_Sensor(11, 65);
	Map.add_Sensor(11, 100);
	Map.add_Sensor(11, 135);

	Map.add_Sensor(47, 11);


	Map.add_Actuator(51, 14, 750);
	Map.add_Actuator(51, 16, 750);

	Map.add_Actuator(50, 90, 0);
	Map.turn_Actuator_On(2);

	//Outside
	Map.add_Actuator(0, 0, 0);
	Map.add_Actuator(0, 25, 0);
	Map.add_Actuator(0, 50, 0);
	Map.add_Actuator(0, 75, 0);
	Map.add_Actuator(0, 100, 0);
	Map.add_Actuator(0, 125, 0);
	Map.add_Actuator(0, 150, 0);
	Map.add_Actuator(0, 174, 0);

	Map.turn_Actuator_On(3);
	Map.turn_Actuator_On(4);
	Map.turn_Actuator_On(5);
	Map.turn_Actuator_On(6);
	Map.turn_Actuator_On(7);
	Map.turn_Actuator_On(8);
	Map.turn_Actuator_On(9);
	Map.turn_Actuator_On(10);

	/*
	//Map.set_Temp(tmp_X, tmp_Y, tmp_Temp);
	for (int cou_Index = 0; cou_Index < 4; cou_Index++)
	{
		for (int cou_Index = 0; cou_Index < 75; cou_Index++)
		{
			Map.set_Temp(24, 24, 0);

			Map.update();
		}
		Map.view_Map();
	}*/

	while (1)
	{
		tmp_X = 0;
		tmp_Y = 0;
		tmp_Temp = 0;

		/*
		std::cout << "Enter Coord:";
		std::cout << "\n X:";
		std::cin >> tmp_X;
		std::cout << " Y:";
		std::cin >> tmp_Y;
		std::cout << " Enter Char: ";

		std::cin >> tmp_Temp;
		*/

		tmp_X = (rand() % 10) + 20;
		tmp_Y = (rand() % 10) + 20;
		tmp_Temp = float((rand() % 2) * 200);

		flg_Ranran = 0;
		//if (!(rand() % 5)) { flg_Ranran = 1; }
		//if (!flg_AC && !flg_Heater) { flg_Ranran = 1; }

		//Map.set_Temp(tmp_X, tmp_Y, tmp_Temp);
		for (int cou_Index = 0; cou_Index < 1; cou_Index++)
		{

			for (int cou_Index = 0; cou_Index < 25; cou_Index++)
			{
				tmp_Sensor = Map.get_Sensor_Data(2);
				if (flg_Ranran) { Map.set_Temp(tmp_X, tmp_Y, tmp_Temp); }

				if (tmp_Sensor > 102) { flg_AC = true; }
				if (tmp_Sensor < 98) { flg_Heater = true; }

				if (tmp_Sensor > 100) { flg_Heater = false; }
				if (tmp_Sensor < 100) { flg_AC = false; }

				if (flg_Heater) 
				{ 
					//Map.set_Temp(45, 45, 225); 
					Map.turn_Actuator_On(0);
					Map.turn_Actuator_On(1);
				}
				if (!flg_Heater) 
				{ 
					//Map.set_Temp(45, 45, 225); 
					Map.turn_Actuator_Off(0);
					Map.turn_Actuator_Off(1);
				}

				/*
				if (flg_AC) 
				{ 
					//Map.set_Temp(5, 5, 0); 
					Map.turn_Actuator_On(2);
					Map.turn_Actuator_On(3);
				}

				if (!flg_AC) 
				{ 
					//Map.set_Temp(5, 5, 0); 
					Map.turn_Actuator_Off(2);
					Map.turn_Actuator_Off(3);
				}*/

				//Map.set_Temp(30, 40, 125);

				Map.update();
			}
			/*
			std::cout << " Temp at Sensor (25, 25): " << tmp_Sensor;
			std::cout << " AC: " << flg_AC;
			std::cout << " Heat: " << flg_Heater;
			*/
			std::cout << "     ";
			std::cout << "     ";
			std::cout << "     ";
			std::cout << "     ";
			std::cout << "     ";
			std::cout << "     ";
			Map.view_Map();
		}
	}

}



















/*
Map.set_Type(23, 40, 1);
Map.set_Type(24, 40, 1);

Map.set_Type(26, 40, 1);
Map.set_Type(27, 40, 1);
*/

/*
for (int cou_X = 40; cou_X <= 50; cou_X++)
{
	Map.set_Type(22, cou_X, 0);

	Map.set_Type(25, cou_X, 0);

	Map.set_Type(28, cou_X, 0);
}

for (int cou_X = 0; cou_X < 10; cou_X++)
{
	Map.set_Type(25, cou_X, 0);
}

for (int cou_X = 30; cou_X < 74; cou_X++)
{
	Map.set_Type(25, cou_X, 0);
}

for (int cou_X = 0; cou_X < 55; cou_X++)
{
	Map.set_Type(cou_X, (cou_X + 70), 0);
}
for (int cou_X = 130; cou_X < 140; cou_X++)
{
	Map.set_Type(15, cou_X, 0);
}
for (int cou_X = 130; cou_X < 140; cou_X++)
{
	Map.set_Type(45, cou_X, 0);
}*/

/*
for (int cou_N = 0; cou_N < 100; cou_N++)
{
	int tmp_XX = (rand() % 30) + 15;
	int tmp_YY = (rand() % 5) + 130;

	Map.set_Type(tmp_XX, tmp_YY, 0);
}*/

/*
for (int cou_X = 0; cou_X < 60; cou_X++)
{
	Map.set_Type(10, cou_X, 0);
}

for (int cou_X = 10; cou_X < 75; cou_X++)
{
	Map.set_Type(20, cou_X, 0);
}

for (int cou_X = 0; cou_X < 60; cou_X++)
{
	Map.set_Type(30, cou_X, 0);
}

for (int cou_X = 10; cou_X < 75; cou_X++)
{
	Map.set_Type(40, cou_X, 0);
}
*/





















/** \addtogroup MapSim_Text_Server
 *  @{
 */

 /** \class c_MapSim_Text_Server
	 \brief This is a handshake based text interface for the engine.


	 File-Based Command Control System for Asynchronous Standalone MapSim Environmental Simulator Control:

	 Text-Based Command Execution:
	 - Commands are represented as text strings that specify actions or operations to be performed by the system or process.
	 - Each command is associated with a specific functionality or operation within the system.

	 File I/O:
	 - Input and output operations are performed using files as the medium of communication.
	 - Commands are read from control_panel.ssv, afferent/efferent each have a cooresponding input/state/output files, though some commands do output to the console.
	 - The files for PLC style I/O tables do not have flags, instead they are gathered from the files on command, not consistently.
	 - This method facilitates asynchronous communication between systems, as they can read from and write to files independently.

	 Handshake Protocol:
	 - A handshake protocol is used to establish communication or synchronize actions between systems.
	 - In this context, a flag file serves as a mechanism for signaling the readiness or completion of certain operations, namely that the user has written a command sequence that is ready to be interpreted..
	 - The content of the flag file is used to indicate the status of the point of interface, allowing the system to either wait or act accordingly.

	 Flag File:
	 - A flag file is a small file used to signal the status or state of the system.
	 - It typically contains simple data or metadata, such as a single value or indicator, to convey information. In the NT4 text interface it is a boolean value.
	 - Flag files are used as synchronization primitives to coordinate activities between systems.

	 Control Panel:
	 - The control panel represents a set of commands or instructions that can be executed by the system or process.
	 - It is stored in a the file "Control_Panel.ssv", which contains a sequence of commands or directives to be processed.

	 IO:
	 - For each afferent and efferent index there is an "Output.n.ssv" file, where 'n' is the current IO table index. This is one way by which the system delivers information to the outside, another way being through the console.
	 - When querying information such as node counts, current symbol mappings, activation mappings, etc the user can specify the filename to output to, and whether to do append or truncate the output.

	 Interpretation and Execution:
	 - The system interprets commands read from the control file (e.g., Control_Panel.ssv) and executes them sequentially.
	 - Command execution may involve performing operations on data, manipulating the state of the system, or interacting with external resources such as files or console output.
	 - The input files are set externally and read in, whereas the output files are written and output.

	 Status Checking and Control Flow:
	 - The system periodically checks the status of a flag file (e.g., Control_Panel_Flag.ssv, etc) to determine if there are new commands to execute, input to load, or if certain operations have completed.
	 - Based on the status indicated by the flag file, the system may initiate specific actions, continue processing, or wait for further instructions.
	 - If an output file then the system flips the handshake to initiated when it is done writing to the output file.

	 Startup and 'autoexec.ssv':
	 - For 'booting up' the system interprets the commands found in 'autoexec.ssv' sequentially. This allows for shell scripting a startup sequence.

	 Custom Commands:
	 - A script file placed in "./Scripts/" can be called by simply entering the filename as you would a command token. A file can call other files so you can create dependency hell for yourself if you wish, but with the added complexity of ML.

	 \var string const RETURN_FILE defines the file in which returns write their data at this level of the API.
 */


class c_MapSim_Text_Server
{
	//The construct to hook into.
	c_Map_Sim API;

	//Current server tick.
	int Tick;

	//Exit flag to allow for exit after startup if the user puts 'exit' into the autoexec file, needed for CLI capabilities.
	bool flg_Exit;

	//How long to idle.
	int Idle_Time;


private:

	//    ---==  write_to_output  ==---
	int write_to_output(std::string p_FName, uint64_t p_Data)
	{
		//std::cout << "\n\n __COMMAND__| write_to_output |";
		//std::cout << " - FName \"" << p_FName << "\" Data:" << p_Data;
		std::ofstream tmp_Out(p_FName, std::ios::app);

		// Check if the flag file exists and can be opened
		if (tmp_Out.is_open())
		{
			tmp_Out << p_Data;
		}

		tmp_Out.close();


		return 1;
	}

	//    ---==  write_to_output  ==---
	int write_to_output(std::string p_FName, std::string p_Data)
	{
		//std::cout << "\n\n __COMMAND__| write_to_output |";

		std::ofstream tmp_Out(p_FName, std::ios::app);

		// Check if the flag file exists and can be opened
		if (tmp_Out.is_open())
		{
			tmp_Out << p_Data;
		}

		tmp_Out.close();


		return 1;
	}


	//I am currently writing the commands to register afferent and efferent I/O, plus basic state information output to user defined files with either app or ate by their choice. Once that is done I will then write the portion to handle the actual handshake protocol, assigning each afferent & efferent an index and a file coesponding to the index akin to a PLC. Then onto writing the portions to pipe the commands to the GaiaOS c_Homeostasis_Module. After that it is documentation, cleanup, and hosting on GitHub

	/*
	int eval_Command(std::string p_Command, std::ifstream* p_File)
	{
		//See end of class
	*/

	// Load Control_Panel.ssc & issue commands
	int interpret_File(std::string p_LFName)
	{
		std::ifstream LF(p_LFName);

		std::string tmp_In = "";
		int tmp_Count = 0;

		if (LF.is_open())
		{
			while (!LF.eof())
			{
				tmp_In = "";

				LF >> tmp_In;

				if (tmp_In == "") { continue; }

				//std::cout << "\n - [ " << tmp_Count << " ]: " << tmp_In;
				tmp_Count++;


				int tmp_Result = 0;
				tmp_Result = eval_Command(tmp_In, &LF);

				if (tmp_Result == -1) { return -1; }
			}

			return 1;
		}
		else
		{
			std::cerr << "\n Unable to open ScriptFile " << p_LFName << "...\n";

			return 0;
		}


		return 1;
	}


	//Control_Panel_Flag.ssv - Used to signal that there is a live message to read. Set by the user or an external program after inputs and controls are set and ready to have a function executed.
	int check_Control_Panel_Flag()
	{
		std::ifstream flagFile("./MapSimTextServ/Control_Panel_Flag.ssv");
		std::string flagValue = "";

		// Check if the flag file exists and can be opened
		if (flagFile.is_open())
		{
			flagFile >> flagValue;

			// Read the value from the flag file
			if (flagValue != "")
			{
				// Check if the value is 1
				if (flagValue != "")
				{
					flagFile.close();
					return 1;
				}
				else
				{
					flagFile.close();
				}
			}
			else
			{
				flagFile.close();
			}
		}
		else
		{
			std::cerr << "\nUnable to open flag file. \"./MapSimTextServ/Control_Panel_Flag.ssv\" \n";
		}

		return 0;
	}

	int execute_Control_Panel_Buffer()
	{

		int tmp_Result = 0;
		//tmp_Result = interpret_Control_Panel();
		tmp_Result = interpret_File("./MapSimTextServ/Control_Panel.ssv");

		if (tmp_Result)
		{
			std::ofstream clsFinishFlagFile("./MapSimTextServ/Control_Panel_Finished.ssv", std::ios::ate);

			// Check if the flag file exists and can be opened
			if (clsFinishFlagFile.is_open())
			{
				clsFinishFlagFile << "1";
			}

			clsFinishFlagFile.close();

		}

		if (tmp_Result == 0)
		{
			std::cerr << "\n\n   ERROR: Unable to interpret the control panel file...  \n";

			return 0;
		}

		if (tmp_Result == -1)
		{
			std::ofstream clsFinishFlagFile("./MapSimTextServ/Control_Panel_Finished.ssv", std::ios::ate);

			// Check if the flag file exists and can be opened
			if (clsFinishFlagFile.is_open())
			{
				clsFinishFlagFile << "1";
			}

			clsFinishFlagFile.close();

			return -1;
		}


		std::ofstream file_Object("./MapSimTextServ/Control_Panel_Flag.ssv", std::ios::ate);

		//Make sure the file was opened.
		if (!file_Object.is_open())
		{
			std::cerr << "\n\n   ERROR: Unable to interpret the file_Object file for truncation!...  \n";
		}

		file_Object.close();

		return 1;
	}


	int view_File(std::string p_FileName)
	{
		std::ifstream InputFile(p_FileName);

		std::string tmp_In = "";
		int tmp_Count = 0;

		if (InputFile.is_open())
		{
			while (!InputFile.eof())
			{
				InputFile >> tmp_In;
				std::cout << "\n - [ " << tmp_Count << " ]: " << tmp_In;
				tmp_Count++;

				//Neuralman.output_Input();
			}

			std::cout << "\n " << p_FileName << " loaded successfully.\n";

			return 1;
		}
		else
		{
			std::cerr << "\n Unable to open " << p_FileName << " ...\n";

			return 0;
		}
	}

public:

	//The default of the ../ is so that is navigates up from the scripts folder when finding boot status. It's so dumb I find it funny so now this engine's default autoexec file is "./scripts/../autoexec.ssv" lmao. If it doesn't work on other systems I'll have to change it but on windows it works (o.O)
	c_MapSim_Text_Server(std::string p_Autoexec = "../autoexec.ssv")
	{
		Tick = 0;
		flg_Exit = false;
		Idle_Time = 1000;

		std::cout << "\n\n   (~.~) BOOTING UP  ";
		//See if they submitted a command, these scripts are retrieved from the ./Scripts/ dir.
		std::string tmp_Autoexec_FName = "./MapSimTextServ/Scripts/" + p_Autoexec;
		std::cout << "\n\n   (o.o) LOADING BOOT FILE " << tmp_Autoexec_FName << "  \n\n";

		int tmp_Boot_Status = interpret_File(tmp_Autoexec_FName);

		//Load the boot sequence 
		if (tmp_Boot_Status)
		{
			std::cout << "\n\n   \\(^-^)/ SUCCESSFULLLY BOOTED  \n\n";
		}
		else
		{
			std::cout << "\n\n   (;_;) < FAILED TO BOOT PROPERLY  \n\n";
		}

		if (tmp_Boot_Status == -1) { flg_Exit = true; }
	}


	//      ---==================================---
	//     ---====================================---
	//    ---======================================---
	//   ---========================================---
	//  ---==   Generic commands for the engine.   ==---
	//   ---========================================---
	//    ---======================================---
	//     ---====================================---
	//      ---==================================---




	int echo(std::ifstream* p_File)
	{
		//---std::cout << "\n --> Echo |\n";

		std::string tmp_In = "";
		std::string tmp_In_Full = "";
		int tmp_Count = 0;

		bool flg_Gather_Input = true;

		while (flg_Gather_Input)
		{
			tmp_In = "";
			*p_File >> tmp_In;

			if (tmp_In == "/end/")
			{
				flg_Gather_Input = false;
				continue;
			}

			if (tmp_In != "")
			{

				tmp_Count++;

				if (tmp_In_Full != "")
				{
					tmp_In_Full = tmp_In_Full + " " + tmp_In;
				}
				else
				{
					tmp_In_Full = tmp_In;
				}
			}

			if (flg_Gather_Input)
			{
				flg_Gather_Input = (!p_File->eof());
			}
		}

		std::cout << "\n\n" << tmp_In_Full;
		std::cout << "\n";

		//std::cout << " [|x|]";

		return 1;
	}


	/** The main loop for the neuro-server.

		run

	\retval None This function doesn't return any values.

	Call this to start the server in C++.

	This loop outputs a the message as a means of delay, then checks the flag file, and if anything is found it calls the interpreter.

	Error Handling:

	- No error handling is implemented in this function.

	Additional Notes:

	- None.
	*/
	int run()
	{
		if (flg_Exit)
		{
			std::cout << "\n --> exit |";
			std::cerr << "\n\n   (o~o) It's been fun anon...goodbye...  \n\n";
			return 1;
		}

		int flg_Direction = 1;
		int tmp_Distance = -250;
		int tmp_MAX = 250;

		std::string tmp_Message = "______________________________IDLING______________________________";
		std::string tmp_Corrupt_Message = "";

		flg_Direction = 1;
		tmp_Distance = 0;
		while (1)
		{
			for (int cou_Time = 0; cou_Time < Idle_Time; cou_Time++)
			{
				tmp_Distance += flg_Direction;

				if (tmp_Distance >= tmp_MAX) { flg_Direction = -1; }
				if (tmp_Distance <= (tmp_MAX * -1)) { flg_Direction = 1; }

				for (int cou_Index = 0; cou_Index < (tmp_Message.length() + 9); cou_Index++)
				{
					std::cout << char(8);
				}

				tmp_Corrupt_Message = tmp_Message;

				for (int cou_Index = 0; cou_Index < tmp_Distance; cou_Index++)
				{
					tmp_Corrupt_Message[rand() % tmp_Message.length()] = char((std::rand() % 32) + 32);
				}

				std::cout << "STANDBY[" << tmp_Corrupt_Message << "]";
			}


			int tmp_Result = check_Control_Panel_Flag();

			//Check the control panel for an update
			if (tmp_Result)
			{
				//std::cout << "\n ~~~Calling control interpreter:";

				if (execute_Control_Panel_Buffer() == -1)
				{
					std::cout << "\n --> exit |";
					std::cerr << "\n\n   (o~o) It's been fun anon...goodbye...  \n\n";

					return 1;
				}

				std::cout << "\n\n (o.O)";

				std::cout << tmp_Message;

			}


			// Execute the system update script
			tmp_Result = interpret_File("./MapSimTextServ/Scripts/update.txt");
			if (tmp_Result == 0)
			{
				std::cerr << "\n\n   Error: Failed to execute system update script!\n\n";
			}

			Tick++;
		}
	}

	void rcon()
	{

	}

	void new_Map(std::ifstream* p_File)
	{
		std::cout << "\n --> new_Map |";

		int tmp_Width = 0, tmp_Height = 0;
		*p_File >> tmp_Width >> tmp_Height;
		std::cout << " Width: " << tmp_Width << " Height: " << tmp_Height;

		// Ensure step is not zero to avoid infinite loop
		if ((tmp_Height <= 0) && (tmp_Width <= 0))
		{
			std::cerr << "\nError: Height & Width must be non-zero. Height: " << tmp_Height << " Width: " << tmp_Width;
			return;
		}

		API.new_Map(tmp_Width, tmp_Height);
	}

	void update(std::ifstream* p_File)
	{
		std::cout << "\n --> update |";

		API.update();
	}

	void add_Actuator(std::ifstream* p_File)
	{
		std::cout << "\n --> add_Actuator |";

		int tmp_X = 0, tmp_Y = 0;
		*p_File >> tmp_X >> tmp_Y;
		float tmp_Temp = 0.0;
		*p_File >> tmp_Temp;
		std::cout << " X: " << tmp_Y << " Y: " << tmp_Y << " Temp: " << tmp_Temp;

		API.add_Actuator(tmp_X, tmp_Y, tmp_Temp);
	}

	void add_Sensor(std::ifstream* p_File)
	{
		std::cout << "\n --> add_Sensor |";

		int tmp_X = 0, tmp_Y = 0;
		*p_File >> tmp_X >> tmp_Y;
		std::cout << " X: " << tmp_X << " Y: " << tmp_Y;

		API.add_Sensor(tmp_X, tmp_Y);
	}

	void init(std::ifstream* p_File)
	{
		std::cout << "\n --> init |";

	}


	void toggle_Actuator(std::ifstream* p_File)
	{
		std::cout << "\n --> toggle_Actuator |";

		int tmp_Actuator = 0, tmp_Toggle = 0;
		*p_File >> tmp_Actuator >> tmp_Toggle;
		std::cout << " Actuator: " << tmp_Actuator << " Toggle: " << tmp_Toggle;

		if (tmp_Toggle > 0) { API.turn_Actuator_On(tmp_Actuator); }
		if (tmp_Toggle <= 0) { API.turn_Actuator_Off(tmp_Actuator); }
	}
	
	void set_Idle_Time(std::ifstream* p_File)
	{
		std::cout << "\n --> set_Idle_Time |";

		*p_File >> Idle_Time;
		std::cout << " Idle_Time: " << Idle_Time;
	}


	void view_Map(std::ifstream* p_File)
	{
		std::cout << "\n --> view_Map |";

		API.view_Map();
	}

	void read_Actuator_State(std::ifstream* p_File)
	{
		std::cout << "\n --> read_Actuator_State |";

		std::string tmp_Input_FName;
		*p_File >> tmp_Input_FName;

		int tmp_Actuator_ID = 0;
		*p_File >> tmp_Actuator_ID;

		std::cout << "\n Actuator ID: " << tmp_Actuator_ID;

		std::ifstream tmp_Input_File(tmp_Input_FName);

		if (!tmp_Input_File.is_open())
		{
			std::cerr << "\nError: Unable to open file " << tmp_Input_FName << " for reading.";
			return;
		}

		int actuator_State;
		tmp_Input_File >> actuator_State;
		std::cout  << " State: " << actuator_State;

		if (actuator_State > 0) { API.turn_Actuator_On(tmp_Actuator_ID); }
		if (actuator_State <= 0) { API.turn_Actuator_Off(tmp_Actuator_ID); }

		tmp_Input_File.close();
	}

	/*
	void XXX(std::ifstream* p_File)
	{
		std::cout << "\n --> XXX |";

		API
	}
	*/

	void write_Actuator_State(std::ifstream* p_File)
	{
		std::cout << "\n --> write_Actuator_State |";

		std::string tmp_Output_FName;
		*p_File >> tmp_Output_FName;

		int tmp_Actuator_ID = 0;
		*p_File >> tmp_Actuator_ID;

		std::cout << " Output_Filename: " << tmp_Actuator_ID;
		std::cout << " Actuator ID: " << tmp_Actuator_ID;

		std::ofstream tmp_Output_File(tmp_Output_FName, std::ios::trunc);

		if (!tmp_Output_File.is_open())
		{
			std::cerr << "\nError: Unable to open file " << tmp_Output_FName << " for writing.";
			return;
		}

		// Assuming API has a method to fetch the state of a specific actuator.
		int actuator_State = API.get_Actuator_State(tmp_Actuator_ID);
		tmp_Output_File << actuator_State;

		std::cout << " State: " << actuator_State;

		tmp_Output_File.close();
	}

	void write_String(std::ifstream* p_File)
	{
		std::cout << "\n --> write_String |";

		std::string tmp_Output_FName;
		*p_File >> tmp_Output_FName;

		std::string tmp_Write = "";
		*p_File >> tmp_Write;

		std::cout << " String: " << tmp_Write << " |";
		std::cout << " Output_FName: " << tmp_Output_FName << " |";

		std::ofstream tmp_Output_File(tmp_Output_FName, std::ios::trunc);

		if (!tmp_Output_File.is_open())
		{
			std::cerr << "\nError: Unable to open file " << tmp_Output_FName << " for writing.";
			return;
		}

		tmp_Output_File << tmp_Write;

		tmp_Output_File.close();
	}

	void write_Sensor_State(std::ifstream* p_File)
	{
		std::cout << "\n --> write_Sensor_State |";

		std::string tmp_Output_FName;
		*p_File >> tmp_Output_FName;

		int tmp_Sensor_ID = 0;
		*p_File >> tmp_Sensor_ID;

		std::cout << "\n Sensor_ID: " << tmp_Sensor_ID << " |";

		std::ofstream tmp_Output_File(tmp_Output_FName, std::ios::trunc);

		if (!tmp_Output_File.is_open())
		{
			std::cerr << "\nError: Unable to open file " << tmp_Output_FName << " for writing.";
			return;
		}

		// Assuming API has a method to fetch the state of a specific sensor.
		double sensor_State = API.get_Sensor_Data(tmp_Sensor_ID);
		tmp_Output_File << sensor_State;

		std::cout << " State: " << sensor_State << " |";

		tmp_Output_File.close();
	}

private:

	int eval_Command(std::string p_Command, std::ifstream* p_File)
	{
		//==--  Basic Command List  --=//
		// 
		//Meta commands for the engine rather than the nodes and internal structures.
		if (p_Command == "exit") { return -1; }
		if (p_Command == "/?") { help_Text(); return 1; }
		if (p_Command == "help") { help_Text(); return 1; }

		//Syntax
		//if (p_Command == "command_text") { command_text(p_File); return 1; }
		// 
		// 
		if (p_Command == "new_Map") { new_Map(p_File); return 1; }
		

		if (p_Command == "add_Actuator") { add_Actuator(p_File); return 1; }
		if (p_Command == "add_Sensor") { add_Sensor(p_File); return 1; }
		if (p_Command == "update") { update(p_File); return 1; }
		if (p_Command == "toggle_Actuator") { toggle_Actuator(p_File); return 1; }
		if (p_Command == "view_Map") { view_Map(p_File); return 1; }

		if (p_Command == "read_Actuator_State") { read_Actuator_State(p_File); return 1; }
		if (p_Command == "write_Actuator_State") { write_Actuator_State(p_File); return 1; }
		if (p_Command == "write_Sensor_State") { write_Sensor_State(p_File); return 1; }
		if (p_Command == "write_String") { write_String(p_File); return 1; }

		if (p_Command == "set_Idle_Time") { set_Idle_Time(p_File); return 1; }

		//See if they submitted a command, these scripts are retrieved from the ./Scripts/ dir.
		std::string tmp_Command = "./MapSimTextServ/Scripts/" + p_Command;
		int tmp_PF = interpret_File(tmp_Command);

		if (tmp_PF == 0)
		{
			std::cout << "\n ---Trying with .txt appended...\n\n";

			std::string tmp_Command = "./MapSimTextServ/Scripts/" + p_Command + ".txt";
			interpret_File(tmp_Command);
		}

		return 0;
	}

	void help_Text() 
	{
		std::cout << "\nAvailable Commands and Arguments:\n";
		std::cout << "=========================================================================================\n";
		std::cout << "Core Commands:\n";
		std::cout << "  add_Actuator        - Adds an actuator to the system.\n";
		std::cout << "                        Arguments:\n";
		std::cout << "                          <X>: X-coordinate of the actuator.\n";
		std::cout << "                          <Y>: Y-coordinate of the actuator.\n";
		std::cout << "                          <Temp>: Temperature value associated with the actuator.\n";
		std::cout << "  add_Sensor          - Adds a sensor to the system.\n";
		std::cout << "                        Arguments:\n";
		std::cout << "                          <X>: X-coordinate of the sensor.\n";
		std::cout << "                          <Y>: Y-coordinate of the sensor.\n";
		std::cout << "  update              - Updates the system state. No arguments required.\n";
		std::cout << "  toggle_Actuator     - Toggles the state of an actuator.\n";
		std::cout << "                        Arguments:\n";
		std::cout << "                          <Actuator>: ID of the actuator to toggle.\n";
		std::cout << "                          <Toggle>: 1 to turn on, 0 to turn off.\n";
		std::cout << "  view_Map            - Displays the current map. No arguments required.\n";
		std::cout << "  exit                - Exits the application. No arguments required.\n";
		std::cout << "  help                - Displays this help text. No arguments required.\n";
		std::cout << "=========================================================================================\n";
		std::cout << "Map Management Commands:\n";
		std::cout << "  new_Map             - Initializes a new map.\n";
		std::cout << "                        Arguments:\n";
		std::cout << "                          <Width>: Width of the map.\n";
		std::cout << "                          <Height>: Height of the map.\n";
		std::cout << "  read_Actuator_State - Reads actuator states from a specified file.\n";
		std::cout << "                        Arguments:\n";
		std::cout << "                          <input_filename>: Path to the file containing actuator states.\n";
		std::cout << "  write_Actuator_State - Writes current actuator states to a specified file.\n";
		std::cout << "                        Arguments:\n";
		std::cout << "                          <output_filename>: Path to the file where actuator states will be written.\n";
		std::cout << "  write_Sensor_State  - Writes current sensor states to a specified file.\n";
		std::cout << "                        Arguments:\n";
		std::cout << "                          <output_filename>: Path to the file where sensor states will be written.\n";
		std::cout << "=========================================================================================\n";
		std::cout << "Additional Information:\n";
		std::cout << "  Commands are case-sensitive.\n";
		std::cout << "  Arguments must be provided in the order specified.\n";
		std::cout << "=========================================================================================\n";
	}
};
