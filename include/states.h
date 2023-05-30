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

typedef struct
{
    char name[20];
    uint16_t HighScore;
} high_score_t;

