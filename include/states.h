typedef enum
{
    Command_Wait,
    Payload_Wait,
    uart_GetName
} SERIAL_STATE;

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
} GAMESTATES;

extern volatile GAMESTATES state;
extern volatile GAMESTATES pb;
extern volatile SERIAL_STATE serial_state;
