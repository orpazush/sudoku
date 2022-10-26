/*****************************************************************************
 * 					written by : Orpaz Houta								 *
 * 					Last update : 25.12.20									 *
 ****************************************************************************/
#include <iostream>     //cout

#include "sudoku.hpp"


/////////////////////////// Sudoku's Constructors /////////////////////////////

//construct the sudoku game and initialise the 'm_board' with random values
//according to the chosen 'level'
Sudoku::Sudoku(Sudoku::difficulty_t level)
        : m_noChanges(false), m_missingDigits(NUM_OF_DIGITS*NUM_OF_DIGITS)
{
    //init all the squares in the board
    for(u_int8_t row = 0; row < NUM_OF_DIGITS; ++row)
    {
        for(u_int8_t column = 0; column < NUM_OF_DIGITS; ++column)
        {
            u_int8_t square = ((row/BASE) * BASE) + (column/BASE);
            m_board[row][column].InitSquare(row, column, square);
        }
    }

    srand (time(NULL));

    //sets a 'NUM_OF_DIGITS' random values in random places on the board
    for (size_t i = 0; i < NUM_OF_DIGITS; ++i)
    {
        u_int8_t randRow = rand() % NUM_OF_DIGITS;
        u_int8_t randColumn = rand() % NUM_OF_DIGITS;
        int randValue = rand() % NUM_OF_DIGITS;

        m_board[randRow][randColumn].UpdatePossibleValues(*this);
        while (!m_board[randRow][randColumn].IsPossible(randValue))
        {
            randValue = rand() % NUM_OF_DIGITS;
            m_board[randRow][randColumn].UpdatePossibleValues(*this);
        }

        m_board[randRow][randColumn].SetValue(this, randValue);
    }

    //solves the board
    Solve();

    //unset an amount of squares according to the chosen difficulty (level)
    size_t numToHide = 0;
    switch (level)
    {
        case EASY: numToHide = NUM_OF_DIGITS * (NUM_OF_DIGITS-3); break;
        case MEDIUM: numToHide = NUM_OF_DIGITS * (NUM_OF_DIGITS-2); break;
        case HARD: numToHide = NUM_OF_DIGITS * (NUM_OF_DIGITS-1); break;
    }

    //unset an amount of squares according to the chosen difficulty
    for (size_t i = 0; i < numToHide; ++i)
    {
        u_int8_t randRow = rand() % NUM_OF_DIGITS;
        u_int8_t randColumn = rand() % NUM_OF_DIGITS;

        m_board[randRow][randColumn].UnSetValue(this);
    }
}


//construct the sudoku game and initialise the 'm_board' with the given 'values'
Sudoku::Sudoku(int values[NUM_OF_DIGITS][NUM_OF_DIGITS]) : m_noChanges(false)
{
    m_missingDigits = NUM_OF_DIGITS * NUM_OF_DIGITS;
    for(size_t row = 0; row < NUM_OF_DIGITS; ++row)
    {
        for(size_t column = 0; column < NUM_OF_DIGITS; ++column)
        {
            u_int8_t square = ((row/BASE) * BASE) + (column/BASE);
            m_board[row][column].InitSquare(row, column, square);
            m_board[row][column].SetValue(this, values[row][column]);
        }
    }
}

//////////////////////////// Sudoku's public Methods //////////////////////////

//TODO
void Sudoku::Play()
{
    while (m_missingDigits)
    {
        PrintBoard();
        GetInput();
    }
//    PrintBoard();
//    Solve();

    if (!Check())
    {
        std::cout << "  👎👎👎👎 you have errors!! 👎👎👎👎\n";
        std::cout << "  try to fix it\n";
        while (!Check())
        {
            PrintBoard();
            GetInput();
        }
    }

    std::cout << "  🎉🎊🎉🎊 you won!! 🎉🎊🎉🎊\n";
    PrintBoard();
}

//returns weather the board is solved correctly according to the game rules
bool Sudoku::Check()
{
    //updates all the values
    //TODO it is possible to make an safe Set &UnSet instead
    Square *lastSquare = &m_board[NUM_OF_DIGITS-1][NUM_OF_DIGITS-1];
    for(Square *curr = &m_board[0][0]; curr != lastSquare;
        curr = curr->NextSquare(this))
    {
        u_int8_t value = curr->GetValue();
        curr->SetValue(this, value);
    }
    u_int8_t value = lastSquare->GetValue();
    lastSquare->SetValue(this, value);

    digits_bitarray_t checker;
    checker.SetAll();
    for (size_t i = 0; i < NUM_OF_DIGITS; ++i)
    {
        checker &= m_rows[i];
        checker &= m_columns[i];
        checker &= m_fullSquares[i];
    }

    //remove any impact of the first bit which isn't really used
    checker[0] = false;

    return (checker.Count() == NUM_OF_DIGITS);
}

//uses StepOne, StepTwo & BackTracking to solve the board and sets all the
//squares with their right values
void Sudoku::Solve()
{
    srand(time(NULL));
    while (!m_noChanges)
    {
        m_noChanges = true;
        StepOne();
        StepTwo();
    }

    if (m_missingDigits != 0)
    {
        u_int8_t randRow = rand() % NUM_OF_DIGITS;
        u_int8_t randColumn = rand() % NUM_OF_DIGITS;

        BackTracking(&m_board[randRow][randColumn]);
    }

    //so it could be solved again
    m_noChanges = false;
}

//TODO to adjust it to different BASES
//prints the values of the board on the terminal
void Sudoku::PrintBoard()
{
    std::cout << "    ";
    for (size_t row = 1; row <= NUM_OF_DIGITS; ++row)
    {
        std::cout << row << ((row%BASE) ? "  " : "   ");
    }
    std::cout << std::endl;

    for (size_t row = 0; row < NUM_OF_DIGITS; ++row)
    {
        char numOfRow = 'A' + row;
        if (!(row%BASE) && row != NUM_OF_DIGITS)
        {
            std::cout << "  ║═════════║═════════║═════════║\n";
        }
        std::cout << numOfRow << " ║ ";
        for (size_t column = 0; column < NUM_OF_DIGITS; ++column)
        {
            int value = m_board[row][column].GetValue();
            if (!value)
            {
                std::cout << '.';
            } else{
                std::cout << value;
            }
            std::cout << (((column+1)%BASE) ? "  " : " ║ ");
        }
        std::cout << "\n";
    }
    std::cout << "  ╚═════════════════════════════╝\n";
}

/////////////////////////// Sudoku's private Methods //////////////////////////

//sets all the values which are the only possible in every square
void Sudoku::StepOne()
{
    Square *lastSquare = &m_board[NUM_OF_DIGITS-1][NUM_OF_DIGITS-1];
    for (Square *currSquare = &m_board[0][0];
         currSquare < lastSquare;
         currSquare = currSquare->NextSquare(this))
    {
        if (0 == currSquare->GetValue())
        {
            if (1 == currSquare->CountPossibleValues(*this))
            {
                int newValue = currSquare->FindNextOption(1);
                currSquare->SetValue(this, newValue);
                m_noChanges = false;
            }
        }
    }
}

//sets all the digits which are possible in only one position in their
//line/column/square
//it seems like this step is not very useful:(
void Sudoku::StepTwo()
{
    for (size_t row = 0; row < NUM_OF_DIGITS; ++row)
    {
        for (int missingDigit = FindMissingDigit(m_rows[row], 1);
             0 != missingDigit;
             missingDigit = FindMissingDigit(m_rows[row], missingDigit + 1))
        {
            size_t numOfOptions = 0;
            for (size_t column = 0; column < NUM_OF_DIGITS; ++column)
            {
                numOfOptions += m_board[row][column].CountPossibleValues(*this);
            }

            if (1 == numOfOptions)
            {
                for (size_t column = 0; column < NUM_OF_DIGITS; ++column)
                {
                    if (m_board[row][column].IsPossible(missingDigit))
                    {
                        m_board[row][column].SetValue(this, missingDigit);
                        m_noChanges = false;
                        std::cout << "step two is helping!\n";
                    }
                }
            }
        }
    }

    for (size_t column = 0; column < NUM_OF_DIGITS; ++column)
    {
        for (int missingDigit = FindMissingDigit(m_columns[column], 1);
             0 != missingDigit;
             missingDigit = FindMissingDigit(m_columns[column],missingDigit+1))
        {
            size_t numOfOptions = 0;
            for (size_t row = 0; row < NUM_OF_DIGITS; ++row)
            {
                numOfOptions += m_board[column][row].CountPossibleValues(*this);
            }

            if (1 == numOfOptions)
            {
                for (size_t row = 0; row < NUM_OF_DIGITS; ++row)
                {
                    if (m_board[column][row].IsPossible(missingDigit))
                    {
                        m_board[column][row].SetValue(this, missingDigit);
                        m_noChanges = false;
                        std::cout << "step two is helping!\n";
                    }
                }
            }
        }
    }
}

//solves the rest of Unsets Squares using the method of BackTracking
void Sudoku::BackTracking(Square *square)
{
    if (m_missingDigits == 0)
    {
        return;
    }

    else if (0 != square->GetValue())
    {
        BackTracking(square->NextSquare(this));
    }

    else
    {
        square->UpdatePossibleValues(*this);
        for (int nextOption = square->FindNextOption(1);
             0 != nextOption;
             nextOption = square->FindNextOption(nextOption+1))
        {
            square->SetValue(this, nextOption);
            BackTracking(square->NextSquare(this));
            if (m_missingDigits == 0)
            {
                return;
            }
            square->UnSetValue(this);
        }
    }
}

//static method to return the first unset index in the given
//'bitarray' starting at 'startIdx', or return 0 if not found.
int Sudoku::FindMissingDigit(Sudoku::digits_bitarray_t bitarray, size_t startIdx)
{
    for (int digit = startIdx; digit <= NUM_OF_DIGITS; ++digit)
    {
        if (false == bitarray[digit])
        {
            return digit;
        }
    }

    return 0;
}

void Sudoku::GetInput()
{
    char row = 0;
    int column = 0;
    int value = 0;

    std::cout << "row: ";
    std::cin >> row;

    std::cout << "column: ";
    std::cin >> column;

    std::cout << "value: ";
    std::cin >> value;

    --row;
    --column;

    m_board[(row-'A'+1)][column].SetValue(this, value);
}

////////////////////////////// Square's Methods ///////////////////////////////

//returns m_value
int Sudoku::Square::GetValue() const
{
    return m_value;
}

//init the square with the given values & set its m_value to 0
void Sudoku::Square::InitSquare(u_int8_t row, u_int8_t column, u_int8_t square)
{
    m_value = 0;
    m_row = row;
    m_column = column;
    m_square = square;
}

//sets the square to be 'value' & updates all the relevant members of 'sudoku'
void Sudoku::Square::SetValue(Sudoku *sudoku, int value)
{
    if (0 == value)
    {
        UnSetValue(sudoku);
        return;
    }
    //case the square has a former value unset it first
    if (0 != m_value)
    {
        UnSetValue(sudoku);
    }
    m_possibleValues.ClearAll();
    --sudoku->m_missingDigits;
    sudoku->m_rows[m_row][value] = true;
    sudoku->m_columns[m_column][value] = true;
    sudoku->m_fullSquares[m_square][value] = true;
    m_value = value;
}

//sets this m_value to 0 & updates all the relevant members of 'sudoku'
void Sudoku::Square::UnSetValue(Sudoku *sudoku)
{
    if (0 == m_value)
    {
        return;
    }
    ++sudoku->m_missingDigits;
    sudoku->m_rows[m_row][m_value] = false;
    sudoku->m_columns[m_column][m_value] = false;
    sudoku->m_fullSquares[m_square][m_value] = false;

    UpdatePossibleValues(*sudoku);
    m_value = 0;
}

//calculates the current possible values and updates 'm_possibleValues' accordingly
void Sudoku::Square::UpdatePossibleValues(const Sudoku &sudoku)
{
    m_possibleValues.ClearAll();
    m_possibleValues |= sudoku.m_rows[m_row];
    m_possibleValues |= sudoku.m_columns[m_column];
    m_possibleValues |= sudoku.m_fullSquares[m_square];
    m_possibleValues.Flip();
}

//returns the amount of current possible values in this square
size_t Sudoku::Square::CountPossibleValues(const Sudoku &sudoku)
{
    UpdatePossibleValues(sudoku);
    return m_possibleValues.Count();
}

//returns the next possible digit in this square starting in 'startIdx'
//or 0 if there isn't
int Sudoku::Square::FindNextOption(size_t startIdx) const
{
    for (int digit = startIdx; digit <= NUM_OF_DIGITS; ++digit)
    {
        if (m_possibleValues[digit])
        {
            return digit;
        }
    }

    return 0;
}

//returns weather a given 'value' is possible to be put in this square
bool Sudoku::Square::IsPossible(size_t value) const
{
    return m_possibleValues[value];
}

//returns a pointer to the square comes after this in the 'm_board'
Sudoku::Square *Sudoku::Square::NextSquare(Sudoku *sudoku) const
{
    return (m_column == NUM_OF_DIGITS-1) ?
           &(sudoku->m_board[(m_row+1)%NUM_OF_DIGITS][0]) :
           &(sudoku->m_board[m_row][m_column+1]);
}
