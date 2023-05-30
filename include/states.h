typedef enum
{
    Command_Wait,
    Payload_Wait
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
    Paused
} GAMESTATES;

