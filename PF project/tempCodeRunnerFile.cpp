#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iomanip>
#include <conio.h>

using namespace std;

struct QuizQuestion {
    string text;
    string opt1, opt2, opt3, opt4;
    char correctAnswer;
};

void menu();
void Quick_start();
    void History();
    void Cars();
    void PCs();
    void Books();
    void HarryPotter();
void Resume();
void Highest_score();
int  level();

const int W = 60;

void line(char c = '=')
{
    cout << string(W, c) << "\n";
}

void box_row(const string& text, char border = '|')
{
    int pad = W - 2 - (int)text.size();
    int left  = pad / 2;
    int right = pad - left;
    cout << border
         << string(left,  ' ')
         << text
         << string(right, ' ')
         << border << "\n";
}

void banner(const string& title)
{
    cout << "\n";
    line('=');
    box_row("");
    box_row(title);
    box_row("");
    line('=');
    cout << "\n";
}

void divider() { cout << string(W, '-') << "\n"; }

string lives_bar(int lives)
{
    string bar = "Lives: [ ";
    for (int i = 0; i < 3; i++)
        bar += (i < lives) ? "* " : "_ ";
    bar += "]";
    return bar;
}

string timer_bar(int t, int total)
{
    const int BAR = 20;
    int filled = (t * BAR) / total;
    string bar = "[ ";
    for (int i = 0; i < BAR; i++)
        bar += (i < filled) ? "#" : ".";
    bar += " " + to_string(t) + "s ]";
    return bar;
}

char timed_input(int seconds)
{
    time_t start = time(0);
    int last_printed = -1;

    while (true)
    {
        int elapsed  = (int)difftime(time(0), start);
        int remaining = seconds - elapsed;

        if (remaining <= 0)
        {
            cout << "\r  " << string(55, ' ') << "\r";
            cout << "  [ TIME'S UP! ]\n";
            return '\0';
        }

        if (remaining != last_printed)
        {
            last_printed = remaining;
            cout << "\r  " << timer_bar(remaining, seconds)
                 << "  (L=lifeline | Q=quit)  " << flush;
        }

        if (_kbhit())
        {
            char ch = (char)_getch();
            cout << "\r  " << string(55, ' ') << "\r";
            cout << "  >> You entered: " << (char)toupper(ch) << "\n";
            return ch;
        }
    }
}

void update_leaderboard(int score, const string& category)
{
    banner("  QUIZ COMPLETE  ");

    cout << "  Final Score : " << score << "\n";
    cout << "  Category    : " << category << "\n\n";

    cout << "  Enter your name for the leaderboard: ";
    string name;
    cin  >> name;

    const int SIZE = 5;
    string names[SIZE]  = {"", "", "", "", ""};
    int    scores[SIZE] = { 0,  0,  0,  0,  0};
    int count = 0;

    ifstream reader("Score.txt");
    if (reader.is_open())
    {
        string tmp_name; int tmp_score;
        while (count < SIZE && (reader >> tmp_name >> tmp_score))
        {
            names [count] = tmp_name;
            scores[count] = tmp_score;
            count++;
        }
        reader.close();
    }

    int target_index = -1;
    for (int i = 0; i < SIZE; i++)
    {
        if (i >= count || score > scores[i])
        { target_index = i; break; }
    }

    if (target_index != -1)
    {
        for (int i = min(count, SIZE - 1); i > target_index; i--)
        {
            names [i] = names [i - 1];
            scores[i] = scores[i - 1];
        }
        names [target_index] = name;
        scores[target_index] = score;
        if (count < SIZE) count++;

        cout << "\n  >> Congratulations! You made the leaderboard! <<\n";
    }
    else
    {
        cout << "\n  Sorry, your score didn't make the top 5.\n";
    }

    ofstream writer("Score.txt");
    if (!writer)
        cout << "  Warning: score could not be saved.\n";
    else
    {
        for (int i = 0; i < count; i++)
            if (names[i] != "")
                writer << names[i] << " " << scores[i] << "\n";
        writer.close();
    }
}

void save_resume(const string& tag, int score, int lives,
                 const int* indices, int start, int total)
{
    ofstream rf("Resume.txt", ios::out);
    if (!rf) return;
    rf << tag << "\n" << lives << "\n";
    for (int i = start; i < total; i++)
        rf << indices[i] << "\nscore: " << score << "\n";
    rf.flush();
    rf.close();
}

void run_quiz(QuizQuestion bank[], int indices[], int count,
              int score, int lives, const string& resume_tag)
{
    banner("  GET READY!  ");
    cout << "  Category : " << resume_tag        << "\n";
    cout << "  Questions: " << count             << "\n";
    cout << "  " << lives_bar(lives)             << "\n";
    divider();
    cout << "  * L = use a lifeline (max 3 total)\n";
    cout << "  * Q = save & quit\n";
    divider();
    cout << "\n";

    for (int i = 0; i < count; i++)
    {
        int qIndex = indices[i];

        cout << "\n";
        line('-');
        cout << "  Q" << setw(2) << (i + 1) << " / " << count
             << "   Score: " << score
             << "   " << lives_bar(lives) << "\n";
        line('-');
        cout << "\n";

        cout << "  " << bank[qIndex].text << "\n\n";

        cout << "    A)  " << bank[qIndex].opt1 << "\n";
        cout << "    B)  " << bank[qIndex].opt2 << "\n";
        cout << "    C)  " << bank[qIndex].opt3 << "\n";
        cout << "    D)  " << bank[qIndex].opt4 << "\n";
        cout << "\n";
        divider();

        int  time_for_question = 30;
        char answer = 0;
        bool answered = false;

        while (!answered)
        {
            cout << "\n";
            answer = toupper(timed_input(time_for_question));

            if (answer == '\0')
            {
                divider();
                cout << "  WRONG! (Time ran out)\n";
                cout << "  Correct Answer : "
                     << (char)toupper(bank[qIndex].correctAnswer) << "\n";
                cout << "  Score so far  : " << score << "\n";
                divider();
                save_resume(resume_tag, score, lives, indices, i + 1, count);
                answered = true;
                continue;
            }

            if (answer == 'Q')
            {
                divider();
                cout << "  Quiz saved. See you next time!\n";
                cout << "  Score so far : " << score << "\n";
                cout << "  " << lives_bar(lives) << "\n";
                divider();
                save_resume(resume_tag, score, lives, indices, i, count);
                cout << "\n";
                menu();
                return;
            }

            if (answer == 'L')
            {
                divider();
                if (lives <= 0)
                {
                    cout << "  No lives remaining! Enter your answer:\n";
                    divider();
                    continue;
                }

                lives--;
                int roll = rand() % 10;

                cout << "  LIFELINE USED!   " << lives_bar(lives) << "\n";
                divider();

                if (roll == 0)
                {
                    score++;
                    cout << "  ** LUCKY DRAW! **   +1 Bonus Score!\n";
                    cout << "  New Score: " << score << "\n";
                }
                else
                {
                    time_for_question += 10;
                    cout << "  +10 Seconds added to this question!\n";
                    cout << "  New Time  : " << time_for_question << "s\n";
                }
                divider();
                continue;
            }

            answered = true;
        }

        if (answer == '\0') continue;

        divider();
        if (answer == (char)toupper(bank[qIndex].correctAnswer))
        {
            cout << "  CORRECT!   +1 point\n";
            score++;
        }
        else
        {
            cout << "  WRONG!\n";
            cout << "  Correct Answer : "
                 << (char)toupper(bank[qIndex].correctAnswer) << "\n";
        }
        cout << "  Score so far  : " << score << "\n";
        divider();

        save_resume(resume_tag, score, lives, indices, i + 1, count);
    }

    remove("Resume.txt");
    update_leaderboard(score, resume_tag);
    cout << "\n";
    menu();
}

bool load_questions(const string& filename, QuizQuestion bank[], int size)
{
    ifstream file(filename, ios::in);
    if (!file) return false;
    for (int i = 0; i < size; i++)
    {
        getline(file, bank[i].text);
        file >> bank[i].opt1 >> bank[i].opt2
             >> bank[i].opt3 >> bank[i].opt4
             >> bank[i].correctAnswer;
        file.ignore();
    }
    file.close();
    return true;
}

void shuffle_indices(int indices[], int n)
{
    for (int i = n - 1; i > 0; i--)
    {
        int r = rand() % (i + 1);
        swap(indices[i], indices[r]);
    }
}

void run_category(const string& file_prefix, const string& display_name)
{
    int difficulty = level();

    string suffix;
    if      (difficulty == 1) suffix = "Easy";
    else if (difficulty == 2) suffix = "Medium";
    else if (difficulty == 3) suffix = "Hard";
    else
    {
        cout << "\n  Invalid choice!\n";
        return;
    }

    string filename   = file_prefix + "_" + suffix + ".txt";
    string resume_tag = display_name + "_" + suffix;

    srand((unsigned)time(0));

    QuizQuestion bank[10];
    if (!load_questions(filename, bank, 10))
    {
        cout << "\n  Error: Could not open \"" << filename << "\"\n\n";
        menu();
        return;
    }

    int indices[10] = {0,1,2,3,4,5,6,7,8,9};
    shuffle_indices(indices, 10);

    run_quiz(bank, indices, 10, 0, 3, resume_tag);
}

void History()     { run_category("Histroy",     "History");     }
void Cars()        { run_category("Cars",        "Cars");        }
void PCs()         { run_category("PCs",         "PCs");         }
void Books()       { run_category("Books",       "Books");       }
void HarryPotter() { run_category("HarryPotter", "HarryPotter"); }

int level()
{
    banner("  SELECT DIFFICULTY  ");
    cout << "    1.  Easy\n";
    cout << "    2.  Medium\n";
    cout << "    3.  Hard\n\n";
    cout << "  Enter your choice: ";
    int diff; cin >> diff;
    return diff;
}

void Resume()
{
    ifstream rf("Resume.txt", ios::in);
    if (!rf)
    {
        cout << "\n  No saved quiz found.\n\n";
        menu();
        return;
    }

    string resume_tag;
    getline(rf, resume_tag);
    int lives = 3;
    rf >> lives;
    rf.ignore();

    size_t sep = resume_tag.find('_');
    string cat_name  = (sep != string::npos) ? resume_tag.substr(0, sep)   : resume_tag;
    string diff_name = (sep != string::npos) ? resume_tag.substr(sep + 1)  : "Easy";

    string file_prefix;
    if      (cat_name == "History")     file_prefix = "Histroy";
    else if (cat_name == "Cars")        file_prefix = "Cars";
    else if (cat_name == "PCs")         file_prefix = "PCs";
    else if (cat_name == "Books")       file_prefix = "Books";
    else if (cat_name == "HarryPotter") file_prefix = "HarryPotter";
    else                                file_prefix = cat_name;

    string filename = file_prefix + "_" + diff_name + ".txt";

    QuizQuestion bank[10];
    if (!load_questions(filename, bank, 10))
    {
        cout << "\n  Error: Could not load question file for resume.\n";
        rf.close();
        menu();
        return;
    }

    int remaining_indices[10];
    int remaining_count = 0;
    int score = 0;
    int q; string label; int s;
    while (rf >> q)
    {
        remaining_indices[remaining_count++] = q;
        rf >> label >> s;
        score = s;
        rf.ignore();
    }
    rf.close();

    if (remaining_count == 0)
    {
        cout << "\n  No remaining questions found.\n\n";
        remove("Resume.txt");
        menu();
        return;
    }

    banner("  RESUMING QUIZ  ");
    cout << "  Category          : " << resume_tag      << "\n";
    cout << "  Questions left     : " << remaining_count << "\n";
    cout << "  Score so far       : " << score           << "\n";
    cout << "  " << lives_bar(lives) << "\n\n";

    srand((unsigned)time(0));
    run_quiz(bank, remaining_indices, remaining_count, score, lives, resume_tag);
}

void Highest_score()
{
    ifstream Score("Score.txt", ios::in);
    if (!Score)
    {
        cout << "\n  No scores recorded yet.\n\n";
        menu();
        return;
    }

    banner("  LEADERBOARD  ");

    string name; int sc; int rank = 1;
    while (rank <= 5 && Score >> name >> sc)
    {
        cout << "  " << rank << ".  "
             << left << setw(20) << name
             << right << setw(5) << sc << " pts\n";
        rank++;
    }
    Score.close();

    cout << "\n";
    line('=');
    cout << "\n";
    menu();
}

void Quick_start()
{
    banner("  SELECT TOPIC  ");
    cout << "    1.  History\n";
    cout << "    2.  Cars\n";
    cout << "    3.  PCs\n";
    cout << "    4.  Books\n";
    cout << "    5.  Harry Potter\n\n";
    cout << "  Enter your choice: ";

    int choice; cin >> choice;

    switch (choice)
    {
    case 1: History();     break;
    case 2: Cars();        break;
    case 3: PCs();         break;
    case 4: Books();       break;
    case 5: HarryPotter(); break;
    default:
        cout << "\n  Invalid input! Try again.\n";
        Quick_start();
        break;
    }
}

void menu()
{
    banner("  QUIZ MASTER  ");
    cout << "    1.  Quick Start\n";
    cout << "    2.  Resume\n";
    cout << "    3.  Leaderboard\n";
    cout << "    4.  Exit\n\n";
    cout << "  Enter your choice: ";

    int choice; cin >> choice;

    switch (choice)
    {
    case 1: Quick_start();   break;
    case 2: Resume();        break;
    case 3: Highest_score(); break;
    case 4:
        cout << "\n  Goodbye! Come back and beat your high score! :)\n\n";
        exit(0);
    default:
        cout << "\n  Invalid input! Try again.\n";
        menu();
        break;
    }
}

int main()
{
    menu();
    return 0;
}