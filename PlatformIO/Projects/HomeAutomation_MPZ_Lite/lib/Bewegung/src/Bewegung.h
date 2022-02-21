#ifndef Bewegung_h
#define Bewegung_h

/*****************************************************************************************************************************
*   This class is used to create moving objects for the simulation of a "intelligent" crossing								 *
*	Basically this class creates a moving object which navigates it self between given turn points based on a given start	 *
*	and destination point.                         		                                                                     *                                                          
******************************************************************************************************************************/

#define NumOfTurnPoints 4

class Bewegung
{
	public:

	//Constructors
	Bewegung();
	Bewegung(float sped, int start_x, int start_y, int dest_x, int dest_y, char direc);

	//Deconstructor
	//~Bewegung();

	//SetMethods
	void SetSpeed(float sped);				//Set the speed of the moving object
	void SetDestination(int x, int y);		//Set the destination of the moving object
	void SetStart(int x, int y);			//Set the start position of the moving object
	void SetStartDirection(char direc);		//Set the direction of the object in the start position
	void SetCurrentDirection(char direc);	//Set the current direciton of the moving object (the direction can change in the given turn points)
	void SetCurrentPosition(int x, int y);	//Sets the current position. Can be used to manipulate the current position.
	void SetStartTime(long time);			//Sets the start time to a custom value. Normally the start time is set and used by the CalcPosition method
	void SetErrorRadius(int err);			//Set the error radius which is used for the CheckPosition method. 
	//The error radius is necessary since the accuracy of millis(), which is used for time measurements, 
	//determines the accuracy of CalcPosition
	void SetTurnPoints(int arr[NumOfTurnPoints][2]);	//Sets the turn points 

	//GetMethods
	float GetSpeed();
	int GetStart();
	int GetStart(char x_y);
	int GetDestination();
	int GetDestination(char x_y);
	int GetState();
	int GetCurrentPos(char x_y);
	int GetDelta(char x_y);
	int GetErrorRadius();
	char GetStartDirection();
	char GetCurrentDirection();
	long GetStartTime();
	double GetCurrentTime();
	int GetTurnPoints(char x_y, int Num);
	int GetNumTurnPoints();

	//Other Methods
	void CalcPosition();						
	//This method calculates the current position of the object based on the given speed and the time elapsed since the last calculation
	//The time of the last position calculation ist stored in start_time using the arduino function "micros()"
	bool CalcDirection(int NumOfTurnPoint);
	//Decides wether the direction has to be changed to reach the destination
	//This method should be called if the object reached a turn point
	//Num of turn point should refer to all turn points but the turn point in which the object is currently positioned.
	//The calculation is based on the fact, that in a crossing, in every turn point and for every destination 
	//the object is always able to reach the destination coordinate for either the x or the y coodinate.
	//If the first destination coordinate is reached the direction is then determined by the difference of the current
	//position and the destination. For example: If the destinations x coordinate has already been reached the destination
	//is either in the north or in the south. If the difference is positive the destination will be in the south (due to
	//the our defined coordinate system) and if the difference is negative the destination will ne in north. 
	
	void Stop();									//Sets the objects speed to 0
	void Start(float speed);						//Starts the object with the defined speed
	bool CheckPosition(int x, int y);				//Checks if the object is in the given position (with an error radius of -10 per coordinate) 
	bool CheckCoordinate(char x_y, int coord);		//Checks if the object at the given coordinate (with an error radius of -10) 
	char CalcStartDirection();						//Calcutes the direction to the first turn point based on a given start point 
	
	protected:
	float speed;
	int destination;
	int start;
	int state;
	int turn_points[NumOfTurnPoints][2];
	int start_x;
	int start_y;
	int destination_x;
	int destination_y;
	int current_x;
	int current_y;
	int delta_x;
	int delta_y;
	int error_radius;
	char start_direction;
	char current_direction;
	long start_time;
	double current_time;
};

#endif
