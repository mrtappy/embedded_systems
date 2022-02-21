#include "Bewegung.h"
#include "Pedestrian.h"
#include "Arduino.h"

//Constructor
Pedestrian::Pedestrian():
    Bewegung(), start_points {0}, start_point {0}, destination_point {0}
{

}

//Set Methods
void Pedestrian::SetStartPoints(char x_y, int Num, int value)
{
    if(x_y == 'x')
    {
        start_points[Num][0] = value;
    }
    else if(x_y == 'y')
    {
        start_points[Num][1] = value;
    }
}

//Get Methods
int Pedestrian::GetStartPoints(char x_y, int Num)
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

int Pedestrian::GetStartArrPos()
{
    return start_point;
}

int Pedestrian::GetDestinationArrPos()
{
    return destination_point;
}

//Other Methods
void Pedestrian::FindStartPoints()
{
    for(int i = 0; i < NumOfTurnPoints; i++)
    {
        start_points[i][0] = turn_points[i][0];
        start_points[i][1] = CalcMaxMinCoord('y', i);
    }

    for(int i = NumOfTurnPoints; i < 2*NumOfTurnPoints; i++)
    {
        start_points[i][0] = CalcMaxMinCoord('x', i-NumOfTurnPoints);
        start_points[i][1] = turn_points[i-NumOfTurnPoints][1];
    }
}

int Pedestrian::CalcMaxMinCoord(char x_y, int Turn_Num)
{
    int xy {0};
    int min_coord {0};
    int max_coord {0}; 
    
    if(x_y == 'x')
    {
        xy = 0;
        min_coord = Min_X;
        max_coord = Max_X;
    }
    else if(x_y == 'y')
    {
        xy = 1;
        min_coord = Min_Y;
        max_coord = Max_Y; 
    }


    int delta_min = abs(turn_points[Turn_Num][xy] - min_coord);
    int delta_max = abs(turn_points[Turn_Num][xy] - max_coord);

    if(delta_max < delta_min)
    {
        return max_coord;
    }
    else if(delta_max > delta_min)
    {
        return min_coord;
    }
}

void Pedestrian::SetNewPath()
{
    randomSeed(analogRead(A0));
    
    start_point = (int) random(NumOfStartPoints-1);
    Pedestrian::SetStart(Pedestrian::GetStartPoints('x', start_point),Pedestrian::GetStartPoints('y', start_point));
    
    randomSeed(analogRead(A1)); 
    
    destination_point = (int) random(NumOfStartPoints-1);
    while(start_point == destination_point)
    {
        destination_point = (int) random(NumOfStartPoints-1);
    }
    
    Pedestrian::SetDestination(Pedestrian::GetStartPoints('x', destination_point),Pedestrian::GetStartPoints('y', destination_point));

    Pedestrian::CalcStartDirection();
}

bool Pedestrian::CheckDestination()
{
    return Pedestrian::CheckPosition(destination_x, destination_y);
}