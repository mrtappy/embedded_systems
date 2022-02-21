#include "Bewegung.h"
#include <Arduino.h>
#include <stdlib.h>

Bewegung::Bewegung()
	:speed {200}, start {0}, destination {0}, start_x {0}, start_y {0}, destination_x {0}, destination_y {0}, state {0}, start_direction {'O'},
	current_y {start_y}, current_x {start_x}, current_direction {start_direction}, error_radius {-10}
{
	start_time = millis();
}

Bewegung::Bewegung(float sped, int start_x, int start_y, int dest_x, int dest_y, char direc)
	:speed {sped}, start {0}, destination {0}, start_x {0}, start_y {0}, destination_x {0}, destination_y {0}, state {0}, start_direction {'O'},
	current_y {start_y}, current_x {start_x}, current_direction {start_direction}, error_radius {-10} 
{
	Bewegung::SetStart(start_x, start_y);
	Bewegung::SetDestination(dest_x, dest_y);
	start_direction = direc;
	start_time = millis();
	delta_x = destination_x - start_x;
	delta_y = destination_y - start_y;
}



//Set Methods
void Bewegung::SetSpeed(float sped)
{
	speed = sped;
}

void Bewegung::SetDestination(int x, int y)
{
	if(y > 65535)
	{
		return;
	}

	if(x > 65535)
	{
		return;
	}

	destination_x = x;
	destination_y = y;
	delta_x = destination_x - start_x;
	delta_y = destination_y - start_y;
	x = (x << 16);
	destination = x+y;
}

void Bewegung::SetStart(int x, int y)
{
	if(y > 65535)
	{
		return;
	}

	if(x > 65535)
	{
		return;
	}

	start_x = x;
	start_y = y;
	delta_x = destination_x - start_x;
	delta_y = destination_y - start_y;
	current_x = x;
	current_y = y;
	x = (x << 16);
	start = x+y;
}

void Bewegung::SetStartDirection(char direc)
{
	start_direction = direc;
}

void Bewegung::SetCurrentDirection(char direc)
{
	current_direction = direc;
}

void Bewegung::SetCurrentPosition(int x, int y)
{
	current_x = x;
	current_y = y;
}

void Bewegung::SetStartTime(long time)
{
	start_time = time;
}

void Bewegung::SetErrorRadius(int err)
{
	error_radius = err;
}

void Bewegung::SetTurnPoints(int arr[NumOfTurnPoints][2])
{
	for(int i = 0; i < NumOfTurnPoints; i++)
	{
		turn_points[i][0] = arr[i][0];
		turn_points[i][1] = arr[i][1];
	}
}

//Get Methods
float Bewegung::GetSpeed()
{
	return speed;
}

int Bewegung::GetStart()
{
	return start;
}

int Bewegung::GetDestination()
{
	return destination;
}

int Bewegung::GetDestination(char x_y)
{
	if(x_y == 'x')
	{
		return destination_x;
	}
	else if(x_y == 'y')
	{
		return destination_y;
	}
	else
	{
		return -1;
	}
}

int Bewegung::GetState()
{
	return state;
}

int Bewegung::GetStart(char x_y)
{
	if(x_y == 'x')
	{
		return start_x;
	}
	else if(x_y == 'y')
	{
		return start_y;
	}
	else
	{
		return -1;
	}
}

int Bewegung::GetCurrentPos(char x_y)
{
	if(x_y == 'x')
	{
		return current_x;
	}
	else if(x_y == 'y')
	{
		return current_y;
	}
	else
	{
		return -1;
	}
}

int Bewegung::GetDelta(char x_y)
{
	if(x_y == 'x')
	{
		return delta_x;
	}
	else if(x_y == 'y')
	{
		return delta_y;
	}
	else
	{
		return -1;
	}
}

int Bewegung::GetErrorRadius()
{
	return error_radius;
}

char Bewegung::GetStartDirection()
{
	return start_direction;
}

char Bewegung::GetCurrentDirection()
{
	return current_direction;
}

long Bewegung::GetStartTime()
{
	return start_time;
}

double Bewegung::GetCurrentTime()
{
	return current_time;
}

int Bewegung::GetTurnPoints(char x_y, int Num)
{
	if(x_y == 'x')
	{
		return turn_points[Num][0];
	}
	else if(x_y == 'y')
	{
		return turn_points[Num][1];
	}
	else 
	{
		return -1;
	}
}

//Other Methods

bool Bewegung::CalcDirection(int NumOfTurnPoint)
{
	if(!Bewegung::CheckPosition(Bewegung::GetTurnPoints('x',NumOfTurnPoint),Bewegung::GetTurnPoints('y',NumOfTurnPoint)))
	{
		if(Bewegung::CheckCoordinate('x', destination_x))
		{
			delta_y = destination_y - current_y;
			if(delta_y > 0)
			{
				Bewegung::SetCurrentDirection('S');
				return true;
			}
			else if(delta_y < 0)
			{
				Bewegung::SetCurrentDirection('N');
				return true;
			}
		}

		else if(Bewegung::CheckCoordinate('y', destination_y))
		{
			delta_x = destination_x - current_x;
			if(delta_x > 0)
			{
				Bewegung::SetCurrentDirection('O');
				return true;
			}
			else if(delta_x < 0)
			{
				Bewegung::SetCurrentDirection('W');
				return true;
			}
		}
		else
		{
			if(Bewegung::GetTurnPoints('x', NumOfTurnPoint) == destination_x)
			{
				delta_x = Bewegung::GetTurnPoints('x', NumOfTurnPoint) - current_x;

				if(delta_x > 0)
				{
					Bewegung::SetCurrentDirection('O');
					return true;
				}
				else if(delta_x < 0)
				{
					Bewegung::SetCurrentDirection('W');
					return true;
				}
			}
				
			if(Bewegung::GetTurnPoints('y', NumOfTurnPoint) == destination_y)
			{
				delta_y = Bewegung::GetTurnPoints('y', NumOfTurnPoint) - current_y;

				if(delta_y > 0)
				{
					Bewegung::SetCurrentDirection('S');
					return true;
				}
				else if(delta_y < 0)
				{
					Bewegung::SetCurrentDirection('N');
					return true;
				}
			}

			else
			{
				return false;
			}
		}
	}
}

void Bewegung::CalcPosition()
{
	
	if(delta_x >= error_radius && delta_x <= 0 && delta_y >= error_radius && delta_y <= 0)
	{
		Bewegung::Stop();
		return;
	}

	
	current_time = millis() - start_time;	
	start_time = millis();

	if(current_direction == 'N' && current_y > 0)
	{
		current_y -= (double) speed*current_time;
 
		if(current_y < 0)
		{
			current_y = 0;
		}
	}
	
	if(current_direction == 'S' && current_y < 15000)
	{
		current_y += (double) speed*current_time;

		if(current_y > 15000)
		{
			current_y = 15000;
		}
	}

	if(current_direction == 'O' && current_x < 15000)
	{
		current_x += (double) speed*current_time;

		if(current_x > 15000)
		{
			current_x = 15000;
		}
	}

	if(current_direction == 'W' && current_x > 0)
	{
		current_x -= (double) speed*current_time;

		if(current_x < 0)
		{
			current_x = 0;
		}
	}
}

void Bewegung::Stop()
{
	Bewegung::SetSpeed(0);
}

void Bewegung::Start(float sped)
{
	Bewegung::SetSpeed(sped);
}

bool Bewegung::CheckPosition(int x, int y)
{
	int delta_x = x - current_x;
	int delta_y = y - current_y;
	
	if(delta_x <= 0 && delta_x >= error_radius && delta_y <= 0 && delta_y >= error_radius)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Bewegung::CheckCoordinate(char x_y, int coord)
{
	if(x_y == 'y')
	{
		int delta_y = coord - current_y;
		if(delta_y <= 0 &&  delta_y >= error_radius)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	else if(x_y == 'x')
	{
		int delta_x = coord - current_x;
		if(delta_x <= 0 &&  delta_x >= error_radius)
		{
			return true;
		}
		else
		{
			return false;
		}

	}
}

char Bewegung::CalcStartDirection()
{
	for(int j = 0; j<= NumOfTurnPoints; j++)
	{
		int delta_y = start_y - turn_points[j][1];
		int delta_x = start_x - turn_points[j][0];
		
		if(delta_y == 0)
		{
			if(delta_x < 0)
			{
				current_direction = 'O';
				start_direction = 'O';
				return current_direction;
			}
			else if(delta_x > 0)
			{
				current_direction = 'W';
				start_direction = 'W';
				return current_direction;
			}
			else if(delta_x == 0)
			{
				Bewegung::CalcDirection(j);
			}
		}

		else if(delta_x == 0)
		{
			if(delta_y < 0)
			{
				current_direction = 'S';
				start_direction = 'S';
				return current_direction;
			}
			else if(delta_y > 0)
			{
				current_direction = 'N';
				start_direction = 'N';
				return current_direction;
			}
			else if(delta_y == 0)
			{
				Bewegung::CalcDirection(j);
			}
		}
	}
}

