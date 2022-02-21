#include "Vehicle.h"
#include <Arduino.h>

//Constructor
Vehicle::Vehicle()
    :Bewegung(), traffic_lights {0}, next_traffic_lights {0},
    delta_traffic_lights {0}, destination_points {0},
    start_points {{Min_X, 0},{Max_X, 0},{0, Min_Y},{0, Max_Y}},
    start_destination_map {0}, map_pos {0}, start_point {0}, destination_point {0},
    coord1 {'0'}, coord2 {'0'}, traffic_light_max_x {0}, traffic_light_min_x {0},
    traffic_light_max_y {0}, traffic_light_min_y {0}
{
  
}


//Set Methods
void Vehicle::SetTrafficLights(int arr[NumOfTrafficLights][2])
{
    for(int i = 0; i < NumOfTurnPoints; i++)
	{
		traffic_lights[i][0] = arr[i][0];
		traffic_lights[i][1] = arr[i][1];
	}
}

void Vehicle::SetNextTrafficLights(int arr[NumOfTrafficLights-1])
{
    for(int i = 0; i < NumOfTurnPoints-1; i++)
	{
		next_traffic_lights[i] = arr[i];
	}
}

void Vehicle::SetCoords(char x_y, int Num)
{
    if(Num == 1)
    {
        coord1 = x_y;
        if(x_y == 'x')
        {
            coord2 = 'y';
        }
        else if(x_y == 'y')
        {
            coord2 = 'x';
        }
    }
    else if(Num == 2)
    {
        coord2 = x_y;
        if(x_y == 'x')
        {
            coord1 = 'y';
        }
        else if(x_y == 'y')
        {
            coord1 = 'x';
        }
    }
}

void Vehicle::SetStartPoint(int Start_Num)
{
    start_point = Start_Num;
}

void Vehicle::SetDestinationPoint(int Dest_Num)
{
    destination_point = Dest_Num;
}

//Replaced Set Methods
void Vehicle::SetStart(int Num)
{
    start_x = start_points[Num][0];
    start_y = start_points[Num][1];

	delta_x = destination_x - start_x;
	delta_y = destination_y - start_y;
	
    current_x = start_x;
	current_y = start_y;

	start = (start_x << 16);
	start = start + start_y;
}

void Vehicle::SetDestination(int Dest_Num)
{
	destination_x = destination_points[Dest_Num][0];
	destination_y = destination_points[Dest_Num][1];
	
    delta_x = destination_x - start_x;
	delta_y = destination_y - start_y;
	
    destination = (destination_x << 16);
	destination = destination + destination_y;
}


//Get Methods
int Vehicle::GetTrafficLights(char x_y, int Num)
{
    if(x_y == 'x')
    {
        return traffic_lights[Num][0];
    }
    else if(x_y == 'y')
    {
        return traffic_lights[Num][1];
    }
    else
    {
        return -1;
    }
}

int Vehicle::GetNextTrafficLights(int Num)
{
    if(Num < NumOfTrafficLights-1)
    {
        return next_traffic_lights[Num];
    }
    else
    {
        return -1;
    }
}

int Vehicle::GetDeltaTrafficLights(int Num)
{
    return delta_traffic_lights[Num];
}

char Vehicle::GetCoords(int Num)
{
    if(Num == 1)
    {
        return coord1;
    }
    else if(Num == 2)
    {
        return coord2;
    }
    else
    {
        return 'F';
    }
}

int Vehicle::GetDestinationPoints(char x_y, int Num)
{
      if(x_y == 'x')
    {
        return destination_points[Num][0];
    }
    else if(x_y == 'y')
    {
        return destination_points[Num][1];
    }
    else
    {
        return -1;
    } 
}

int Vehicle::GetTrafficLightMax(char x_y)
{
    if(x_y == 'x')
    {
        return traffic_light_max_x;
    }
    else if(x_y == 'y')
    {
        return traffic_light_max_y;
    }
    else
    {
        return -1;
    }
}

int Vehicle::GetTrafficLightMin(char x_y)
{
    if(x_y == 'x')
    {
        return traffic_light_min_x;
    }
    else if(x_y == 'y')
    {
        return traffic_light_min_y;
    }
    else
    {
        return -1;
    }
}

int Vehicle::GetStartPoints(char x_y, int Num)
{
    if(x_y == 'x')
    {
        return start_points[Num][0];
    }
    else if(x_y == 'y')
    {
        return start_points[Num][1];
    }
}

int Vehicle::GetMapping(int Start_Num, int Dest_Num)
{
    return start_destination_map[Start_Num][Dest_Num];
}

int Vehicle::GetStartPoint()
{
    return start_point;
}

int Vehicle::GetDestinationPoint()
{
    return destination_point;
}


//Other Methods
void Vehicle::FindNextTrafficLight(int Start_Num)
{
    int x_y {0};
    int delta0_Temp = 999999;
    int delta1_Temp = 999999;
    int delta2_Temp = 999999;

    if(coord1 == 'x')
    {
        x_y = 0;
    }
    else if(coord1 == 'y')
    {
        x_y = 1;
    }

    //Sorting of the defined traffic lights from  nearest to farthest based on the given start point (Start_Num)
    for(int i = 0; i < NumOfTrafficLights; i++)
    {
        delta_traffic_lights[i] = abs(traffic_lights[i][x_y] -  Vehicle::GetStartPoints(coord1, Start_Num));
        if((delta_traffic_lights[i] < delta0_Temp) && (delta_traffic_lights[i] > 0)
            && (delta_traffic_lights[i] < delta1_Temp) && (delta_traffic_lights[i] < delta2_Temp))
        {
            delta2_Temp = delta1_Temp;
            delta1_Temp = delta0_Temp;
            delta0_Temp = delta_traffic_lights[i];

            next_traffic_lights[2] = next_traffic_lights[1];
            next_traffic_lights[1] = next_traffic_lights[0];
            next_traffic_lights[0] = i;
        }
        else if((delta_traffic_lights[i] < delta1_Temp) && (delta_traffic_lights[i] > delta0_Temp)
                && (delta_traffic_lights[i] < delta2_Temp))
        {
            delta2_Temp =  delta1_Temp;
            delta1_Temp = delta_traffic_lights[i];

            next_traffic_lights[2] = next_traffic_lights[1];
            next_traffic_lights[1] = i;
        }
        else if((delta_traffic_lights[i] < delta2_Temp) && (delta_traffic_lights[i] > delta1_Temp))
        {
            delta2_Temp = delta_traffic_lights[i];
            
            next_traffic_lights[2] = i;    
        }
    }
}

void Vehicle::CalcCoords()
{
    if(start_direction == 'W' || start_direction == 'O')
    {
        coord1 = 'x';
        coord2 = 'y';
    }
    else if(start_direction == 'N' || start_direction == 'S')
    {
        coord1 = 'y';
        coord2 = 'x';
    }
}

int Vehicle::CalcMaxMinCoord(int TrafficLightNum, int coord_num)
{
    int x_y {0};
    int min_coord {0};
    int max_coord {0}; 
    
    if(coord_num == 1)
    {
        if(coord1 == 'x')
        {
            x_y = 0;
            min_coord = Min_X;
            max_coord = Max_X;
        }
        else if(coord1 == 'y')
        {
            x_y = 1;
            min_coord = Min_Y;
            max_coord = Max_Y;
        }
    }

    else if(coord_num == 2)
    {
        if(coord2 == 'x')
        {
            x_y = 0;
            min_coord = Min_X;
            max_coord = Max_X;
        }
        else if(coord2 == 'y')
        {
            x_y = 1;
            min_coord = Min_Y;
            max_coord = Max_Y;
        }
    }

    int delta_min = abs(traffic_lights[TrafficLightNum][x_y] - min_coord);
    int delta_max = abs(traffic_lights[TrafficLightNum][x_y] - max_coord);

    if(delta_max < delta_min)
    {
        return min_coord;
    }
    else if(delta_max > delta_min)
    {
        return max_coord;
    }
}

void Vehicle::CalcDestinationPoints(int Start_Num)
{
    Vehicle::SetStart(Start_Num);
    //SetStart must be called since CalcStartDireciton is called.
    //The start direction is then need to map the x and y coordinates to coord1 and two coord2
    //The usage of coord1 and coord2 is explained in the header file
    Vehicle::CalcStartDirection();
    Vehicle::CalcCoords();
    
    int coord_1 {0};
    int coord_2 {0};

    if(coord1 == 'x')
    {
        coord_1 = 0;
        coord_2 = 1;
    }
    else if(coord1 == 'y')
    {
        coord_1 = 1;
        coord_2 = 0;
    }
    
    Vehicle::FindNextTrafficLight(Start_Num);
    
    //Sorting of the destination points. Explained in header file! 
    for(int i = map_pos; map_pos < i + NumOfMappingsPerPoint; map_pos++)
    {
        start_destination_map[Start_Num][0] = Start_Num;
        if(map_pos == i)    //The destination which is just straight a head of the start point has be to treated differently! 
        {
            destination_points[map_pos][coord_2] = traffic_lights[next_traffic_lights[0]][coord_2];
            destination_points[map_pos][coord_1] = Vehicle::CalcMaxMinCoord(next_traffic_lights[0], 1);
            start_destination_map[Start_Num][map_pos - i + 1] = map_pos;
        }
        else if(map_pos == i + 1)
        {
            destination_points[map_pos][coord_1] = traffic_lights[next_traffic_lights[1]][coord_1];
            destination_points[map_pos][coord_2] = Vehicle::CalcMaxMinCoord(next_traffic_lights[1], 2);
            start_destination_map[Start_Num][map_pos - i + 1] = map_pos;
        }
        else if(map_pos == i + 2)
        {
            destination_points[map_pos][coord_1] = traffic_lights[next_traffic_lights[2]][coord_1];
            destination_points[map_pos][coord_2] = Vehicle::CalcMaxMinCoord(next_traffic_lights[2], 2);
            start_destination_map[Start_Num][map_pos - i + 1] = map_pos;
        }
    }
}

void Vehicle::MapStartDestinationPoints()
{
    //Calculate and match the possible destination points for all the defined start points
    for(int i = 0; i < NumOfStartPoints; i++)
    {
        Vehicle::CalcDestinationPoints(i);
    }
}

void Vehicle::FindStartPoints()
{
    int TEMP_max {0};
    int TEMP_min = 9999;

    //Find the highest and lowest x coordinate of the given traffic lights 
    for(int i = 0; i < NumOfTrafficLights; i++)
    {
        if(traffic_lights[i][0] > TEMP_max)
        {
            TEMP_max = traffic_lights[i][0];
            traffic_light_max_x = i;
        }

        if(traffic_lights[i][0] < TEMP_min)
        {
            TEMP_min = traffic_lights[i][0];
            traffic_light_min_x = i;
        }
    }

    TEMP_max = 0;
    TEMP_min = 9999;

    //Find highest and lowest y coordinate of the given traffic lights
    for(int i = 0; i < NumOfTrafficLights; i++)
    {
        if(traffic_lights[i][1] > TEMP_max)
        {
            TEMP_max = traffic_lights[i][1];
            traffic_light_max_y = i;
        }

        if(traffic_lights[i][1] < TEMP_min)
        {
            TEMP_min = traffic_lights[i][1];
            traffic_light_min_y = i;
        }
    }

    //Store the founc values to the according position in the start point array
    //Note: The first of the possible start points is always defined by one coordinate of the traffic light positions.
    //      The second coordinate is always given by the defined bounds.
    start_points[0][1] = traffic_lights[traffic_light_min_x][1];
    start_points[1][1] = traffic_lights[traffic_light_max_x][1];
    start_points[2][0] = traffic_lights[traffic_light_min_y][0];
    start_points[3][0] = traffic_lights[traffic_light_max_y][0];
}

void Vehicle::SetNewPath()
{
    randomSeed(analogRead(A0));
    
    start_point = random(NumOfStartPoints-1);

    randomSeed(analogRead(A1));

    destination_point = Vehicle::GetMapping(start_point, random(1, NumOfMappingsPerPoint+1));
    
    Vehicle::SetStart(start_point);
    Vehicle::SetDestination(destination_point);
    Vehicle::CalcStartDirection();
}

bool Vehicle::CheckDestination()
{
    return Vehicle::CheckPosition(destination_x, destination_y);
}
