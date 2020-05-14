//
// Basic UEFI Libraries
//
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
//#include <Library/MemoryAllocationLib.h>
//#include <Library/BaseMemoryLib.h>
//#include <Guid/MemoryAttributesTable.h>
//#include <Guid/PropertiesTable.h>

//
// Boot and Runtime Services
//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

//
// Shell Library
//
#include <Library/ShellLib.h>

//
// Additional protocols
//
#include <Protocol/Shell.h>

//
// Defines
//
#pragma warning( disable: 4152 )

#define TO_MILLISECONDS(Ms) (Ms * 1000)

#define PROMPT_LEN 80

#define MAX_EXERCISE_COUNT               9
#define MAX_EXERCISE_NAME_SIZE          32
#define EXERCISE_DESCRIPTION_SIZE      128

//
// Types
//

typedef EFI_STATUS (EFIAPI* ACTION_FUNC)();

typedef struct _EXERCISE {
    CHAR16       ExerciseName[MAX_EXERCISE_NAME_SIZE];
    ACTION_FUNC  Action;
} EXERCISE;

//
// Prototypes
//

EFI_STATUS EFIAPI ClearScreen();
VOID       EFIAPI SleepMs(IN UINTN Ms);
VOID       EFIAPI Greet();
VOID       EFIAPI Menu();
VOID       EFIAPI StartExercise(IN UINT8 Choise);
VOID       EFIAPI ListExercise();
VOID       EFIAPI CreateExercise();
EFI_STATUS EFIAPI ExerciseHands();
EFI_STATUS EFIAPI ExerciseFlip();
UINT8      EFIAPI ReadDigit();
UINTN      EFIAPI ReadString(IN OUT CHAR16* Buf, IN UINTN Size, IN BOOLEAN HexOnly);

//
// Globals
//

UINT8 gExerciseCount = 2;

EXERCISE gExerciseList[MAX_EXERCISE_COUNT] = {
    { L"Flip"            , (ACTION_FUNC)ExerciseFlip },
    { L"Where are our hands?", (ACTION_FUNC)ExerciseHands },
    { {0}, 0 },
    { {0}, 0 },
    { {0}, 0 },
    { {0}, 0 },
    { {0}, 0 },
    { {0}, 0 },
    { {0}, 0 }
};

EFI_SHELL_PROTOCOL* gEfiShellProtocol;
EFI_HANDLE          gEfiShellImageHandle;

CONST CHAR16* MoveHands[] = {
    L"   o   \r\n"
    L"  /|\\  \r\n"
    L"  _|_  \r\n",

    L"  \\o/  \r\n"
    L"   |   \r\n"
    L" _/ \\_ \r\n",
};

CONST CHAR16* MoveFlip[] = {
    L"   o    \r\n"
    L"  /|\\   \r\n"
    L"  / \\   \r\n",

    L"  \\o/   \r\n"
    L"   |    \r\n"
    L"  / \\   \r\n",

    L"  _ o   \r\n"
    L"   /\\   \r\n"
    L"  | \\   \r\n",

    L"        \r\n"
    L"   ___\\o\r\n"
    L"  /)  | \r\n",

    L"  __|   \r\n"
    L"    \\o  \r\n"
    L"    ( \\ \r\n",

    L"  \\ /   \r\n"
    L"   |    \r\n"
    L"  /o\\   \r\n",

    L"     |__\r\n"
    L"   o/   \r\n"
    L"  / )   \r\n",

    L"        \r\n"
    L"  o/__  \r\n"
    L"  |  (\\ \r\n",

    L"   o _  \r\n"
    L"   /\\   \r\n"
    L"   / |  \r\n",

    L"   o    \r\n"
    L"  /|\\   \r\n"
    L"  / \\   \r\n",

    L"  \\o/   \r\n"
    L"   |    \r\n"
    L"  / \\   \r\n",
};

CONST CHAR16* Rabbit[] = {
    L"     (\\_____/)      \r\n",
    L"     ( o w o )      \r\n",
    L"  __/ \\     / \\ __  \r\n",
    L" /  `/   Y    Y   \\ \r\n",
    L"(   (=` /\\   /     |\r\n",
    L"|  / \\  ____ \\    / \r\n",
    L" \\___ > ________/   \r\n",
    L"     | ( -- ) |     \r\n",
    L"     /   --   \\     \r\n",
};

//
// Funcions
//

EFI_STATUS
EFIAPI
ClearScreen()
{
    return gST->ConOut->ClearScreen(gST->ConOut);
}

VOID
EFIAPI
SleepMs(
    IN UINTN Ms
)
{
    gBS->Stall(TO_MILLISECONDS(Ms));
    return;
}

VOID
EFIAPI
Greet()
{
    ClearScreen();
    for (UINT8 i = 0; i < 9; i++) {
        Print(Rabbit[i]);
    }
    
    Print(L"\r\n");
    Print(L"Welcome to our EFItness Club, player!\r\n");
    Print(L"Let me check everything is ready for your training!\r\n");
    SleepMs(500);
    Print(L"Hm...");
    SleepMs(500);
    Print(L"There are some exercise equipment: 0x%0X, 0x%0X, OK!\r\n", gEfiShellProtocol, gEfiShellImageHandle);
    Print(L"\r\n");

    return;
}

BOOLEAN
EFIAPI
IsDigit(CHAR16 Ch)
{
    return (Ch <= 0x39 && Ch >= 0x30);
}

BOOLEAN
EFIAPI
IsHexLetter(CHAR16 Ch)
{
    return (Ch <= 0x46 && Ch >= 0x41) || (Ch <= 0x66 && Ch >= 0x61);
}

BOOLEAN
EFIAPI
IsAscii(CHAR16 Ch)
{
    return (Ch < 0x7F && Ch >= 0x20);
}

VOID
EFIAPI
Menu()
{
    Print(L"So...What do you want to do?\r\n");
    ListExercise();
    Print(L"\r\n");

    if (gExerciseCount < MAX_EXERCISE_COUNT) {
        Print(L"[0] Add new exercise\r\n");
    }
    
    return;
}

VOID
EFIAPI
StartExercise(
    IN UINT8 Choise
)
{
    EFI_STATUS Status;

    Print(L"[Debug] Call Function: 0x%0X\r\n", gExerciseList[Choise].Action);

    Status = gExerciseList[Choise].Action();
    
    if (Status == EFI_SUCCESS) {
        Print(L"Well done!\r\n");
    }
    else {
        Print(L"Something went wrong, error %r\r\n", Status);
    }
    
    return;
}

VOID
EFIAPI
ListExercise()
{
    for (UINT8 i = 0; i < gExerciseCount; i++) {
        Print(L"[%d] Do \"%s\" exercise\r\n", i + 1, gExerciseList[i].ExerciseName);
    }
    return;
}

VOID
EFIAPI
CreateExercise()
{
    EFI_STATUS  Status;
    ACTION_FUNC Action = NULL;
    UINTN       ActionSize = EXERCISE_DESCRIPTION_SIZE;
    CHAR16*     HexString = NULL;
    UINTN       HexStringSize = EXERCISE_DESCRIPTION_SIZE * 2;
    UINTN       ActualSize = 0;

    if (gExerciseCount >= MAX_EXERCISE_COUNT) {
        Print(L"I can't remember anymore exersice!\r\n");
        return;
    }

    EXERCISE*   Exercise = &gExerciseList[gExerciseCount];

    // Input exercise name
    Print(L"What's the name of your exercise? (<%d)\r\n", MAX_EXERCISE_NAME_SIZE);
    ReadString(Exercise->ExerciseName, MAX_EXERCISE_NAME_SIZE, FALSE);

    // Alloc string
    Status = gBS->AllocatePool(EfiLoaderData,
                               HexStringSize * sizeof(CHAR16),
                               (VOID**)&HexString);
    if (EFI_ERROR(Status)) {
        goto Error;
    }
    
    Print(L"Tell me about your exersice in hex (<%d binary bytes) with '\\n' at the end, please!\r\nLet's start:\r\n", EXERCISE_DESCRIPTION_SIZE);
    ActualSize = ReadString(HexString, HexStringSize, TRUE);
    if (ActualSize % 2 == 1) {
        ActualSize++;
    }

    // Alloc action
    ActionSize = ActualSize >> 1;
    Status = gBS->AllocatePool(EfiLoaderCode,
                               ActionSize,
                               (VOID**)&Action);
    
    if (EFI_ERROR(Status)) {
        goto Error;
    }

    // From hex to bytes
    Status = StrHexToBytes(HexString, ActualSize, (UINT8*)Action, ActionSize);
    if (EFI_ERROR(Status)) {
        goto Error;
    }

    Print(L"Ok, I understood\r\n");
    
    Exercise->Action = Action;
    gExerciseCount++;
    
    if (HexString != NULL) {
        Status = gBS->FreePool(HexString);
    }

    return;

Error:
    Print(L"I can't remember this exercise!\r\n");

    if (HexString != NULL) {
        Status = gBS->FreePool(HexString);
    }

    if (Action != NULL) {
        Status = gBS->FreePool(Action);
    }

    return;
}

EFI_STATUS
EFIAPI
ExerciseHands()
{
    for (UINT8 i = 0; i < 4; i++) {
        ClearScreen();
        Print(MoveHands[i % 2]);
        SleepMs(337);
    }

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ExerciseFlip()
{
    for (UINT8 i = 0; i < 11; i++) {
        ClearScreen();
        Print(MoveFlip[i]);
        SleepMs(150);
    }

    return EFI_SUCCESS;
}

UINTN
EFIAPI
ReadString(
    IN OUT CHAR16* Buf,
    IN     UINTN   Size,
    IN     BOOLEAN HexOnly
)
{
    EFI_INPUT_KEY Key;
    UINTN         EventIndex;
    EFI_STATUS    Status;
    UINTN         i = 0;
    //UINTN         StartCursorRow = gST->ConOut->Mode->CursorRow;
    //INT32         CursorCol;
    //INT32         CursorRow;

    for (i = 0; i < Size - 1;) {
        gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
        Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        
        if (EFI_ERROR(Status)) {
            continue;
        }

        if (Key.UnicodeChar != 0) {
            Print(L"%c", Key.UnicodeChar);
        }

        if (Key.UnicodeChar == CHAR_LINEFEED || Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
            Buf[i] = CHAR_NULL;
            break;
        }
        else if (Key.UnicodeChar == CHAR_BACKSPACE || Key.ScanCode == CHAR_BACKSPACE) {
            if (i > 0) {
                gST->ConOut->OutputString(gST->ConOut, L"\x08 \x08");
                i--;
                Buf[i] = CHAR_NULL;
            }
        }
        else if (!HexOnly && IsAscii(Key.UnicodeChar)) {
            Buf[i] = Key.UnicodeChar;
            i++;
        }
        else if (HexOnly && (IsHexLetter(Key.UnicodeChar) || IsDigit(Key.UnicodeChar))) {
            Buf[i] = Key.UnicodeChar;
            i++;
        }
    }

    Print(L"\r\n");
    gST->ConIn->Reset(gST->ConIn, FALSE);
    Buf[Size - 1] = CHAR_NULL;
    
    return i;
}

UINT8
EFIAPI
ReadDigit()
{
    EFI_INPUT_KEY Key;
    CHAR16        DigitChar[2] = {0};
    UINTN         EventIndex;

    do {
        gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
        gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
    } while (!IsDigit(Key.UnicodeChar));

    DigitChar[0] = Key.UnicodeChar;
    gST->ConIn->Reset(gST->ConIn, FALSE);
    return (UINT8)StrDecimalToUintn(DigitChar);
}

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE* SystemTable
)
{
    EFI_STATUS    Status;
    UINT8         Choise;

    gEfiShellImageHandle = ImageHandle;
    Status = gBS->LocateProtocol(&gEfiShellProtocolGuid, NULL, (VOID**)&gEfiShellProtocol);
    if (EFI_ERROR(Status)) {
        return (Status);
    }

    DebugAssertEnabled();

    Greet();

    while (TRUE) {
        Menu();
        
        Choise = ReadDigit();
        
        ClearScreen();

        if (Choise == 0) {
            CreateExercise();
        }
        else if (Choise > 0 && Choise <= gExerciseCount) {
            StartExercise(Choise - 1);
        }
        else {
            Print(L"Don't worry, just try again!\r\n");
        }
    }

    return Status;
}
