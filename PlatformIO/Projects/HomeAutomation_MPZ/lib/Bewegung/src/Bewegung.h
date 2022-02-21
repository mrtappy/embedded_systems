#ifndef Bewegung_h
#define Bewegung_h

#define NumOfTurnPoints 10

class Bewegung
{
	public:

	//Constructors
	Bewegung();
	Bewegung(float sped, int start_x, int start_y, int dest_x, int dest_y, char direc);

	//Deconstructor
	//~Bewegung();

	//SetMethods
	void SetSpeed(float sped);
	void SetDestination(int y, int x);
	void SetStart(int y, int x);
	void SetState(int st);
	void SetStartDirection(char direc);
	void SetCurrentDirection(char direc);
	void SetCurrentPosition(int y, int x);
	void SetStartTime(long time);
	void SetErrorRadius(int err);
	void SetTurnPoints(int arr[NumOfTurnPoints][2]);

	//GetMethods
	float GetSpeed();
	int GetStart();
	int GetStart(char x_y);
	int GetDestination();
	int GetDestination(char x_y);
	int GetState();
	// int GetStartX();
	// int GetStartY();
	//int GetDestinationX();
	//int GetDestinationY();
	int GetCurrentPos(char x_y);
	// int GetCurrentX();
	// int GetCurrentY();
	int GetDelta(char x_y);
	// int GetDeltaX();
	// int GetDeltaY();
	int GetErrorRadius();
	char GetStartDirection();
	char GetCurrentDirection();
	long GetStartTime();
	double GetCurrentTime();
	int GetTurnPoints(char x_y, int Num);
	int GetNumTurnPoints();
	
	//Other Methods
	void CalcPosition();
	bool CalcDirection(int NumOfTurnPoint);
	void Stop();
	void Start(float speed);
	bool CheckPosition(int x, int y);
	bool CheckCoordinate(char x_y, int coord);
	char CalcStartDirection();
	
	private:
	float speed;
	int destination;
	int start;
	int state;
	int turn_points[NumOfTurnPoints][2];
	int num_turn_points;
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
