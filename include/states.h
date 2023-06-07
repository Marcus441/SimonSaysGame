typedef enum
{
    Command_Wait,
    Payload_Wait,
    uart_GetName
} SERIAL_STATE; // serial state machine states

typedef enum
{
    sequence_start,
    PB1,
    PB2,
    PB3,
    PB4,
    Success,
    Fail,
    UserInput,
    Reset,
    Paused,
    GetName,
    SetName
} GAMESTATES; // game state machine states

extern volatile GAMESTATES state;          // declare state variable
extern volatile GAMESTATES pb;             // declare pushbutton variable
extern volatile SERIAL_STATE serial_state; // declare serial state variable
