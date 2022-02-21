#ifndef StateMachine_h
#define StateMachine_h

#define RadioReset 0
#define RadioScan 1
#define OtherSystemDetected 2
#define CollisionDetected 3
#define RadioBeacon 4
#define CheckForTimeSlot 5
#define WaitForTimeSlot 6
#define TakeTimeSlot 7
#define WaitForNeighborAcknowledge 8
#define WaitForAcknowledge 9
#define SendAcknowledge 10
#define WaitForSync 11
#define HoldTimeSlot 12
#define NeighborRequest 13
#define ListenForNeighbor 14
#define SendNeighborAcknowledge 15
#define WaitForNeighborAcknowledge 16
#define WaitForNeighborRequest 17
#define AnswerNeighborRequest 18
#define WaitForInsertion 19

#endif