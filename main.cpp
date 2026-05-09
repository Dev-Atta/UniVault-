#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>   
#include <ctime>     
#include <cstring>   
using namespace std;


// Reads num_depts, num_floors, num_rooms from config.txt
void read_config(int &num_depts, int &num_floors, int &num_rooms)
{
    ifstream fin("config.txt");

    if (fin.is_open())
    {
        fin >> num_depts >> num_floors >> num_rooms;
        fin.close();
        cout << "Config loaded: "
             << num_depts  << " Depts, "
             << num_floors << " Floors, "
             << num_rooms  << " Rooms\n";
    }
    else
    {
        cout << "config.txt not found.\n";
        cout << "Enter number of Departments : "; cin >> num_depts;
        cout << "Enter number of Floors      : "; cin >> num_floors;
        cout << "Enter number of Rooms       : "; cin >> num_rooms;

        ofstream fout("config.txt");
        fout << num_depts  << "\n"
             << num_floors << "\n"
             << num_rooms  << "\n";
        fout.close();
        cout << "config.txt created successfully.\n";
    }
}


// Allocates the 3D grid on the heap using new
// Sets every room to 0 (vacant)
int*** init_campus(int num_depts, int num_floors, int num_rooms)
{
    // STEP A: allocate top-level array, one int** per department
    int*** grid = new int**[num_depts];

    for (int d = 0; d < num_depts; d++)
    {
        // STEP B: for each department, allocate an array of int*, one per floor
        *(grid + d) = new int*[num_floors];

        for (int f = 0; f < num_floors; f++)
        {
            // STEP C: for each floor, allocate an array of int, one per room
            *(*(grid + d) + f) = new int[num_rooms];

            for (int r = 0; r < num_rooms; r++)
            {
                // STEP D: set every room to 0 (vacant)
                *(*(*(grid + d) + f) + r) = 0;
            }
        }
    }

    cout << "Campus grid allocated and initialised.\n";
    return grid;
}

// Frees all heap memory in reverse order
void free_campus(int*** grid, int num_depts, int num_floors)
{
    for (int d = 0; d < num_depts; d++)
    {
        for (int f = 0; f < num_floors; f++)
        {
            delete[] *(*(grid + d) + f);   // free each room array
        }
        delete[] *(grid + d);              // free each floor array
    }
    delete[] grid;                         // free the department array
}



// Grows all 4 parallel arrays by 1 slot
void grow_arrays(char** &names, int* &ids, float* &gpas, int* &statuses, int num_students)
{
    int new_size = num_students + 1;  //creates a brand new bigger array on the heap

    //  grow names array 
    char** new_names = new char*[new_size];
    for (int i = 0; i < num_students; i++)
        *(new_names + i) = *(names + i);     //Copy name Array
    delete[] names;
    names = new_names;   //points to new array

    //  grow ids array 
    int* new_ids = new int[new_size];
    for (int i = 0; i < num_students; i++)
        *(new_ids + i) = *(ids + i);
    delete[] ids;
    ids = new_ids;

    //  grow gpas array 
    float* new_gpas = new float[new_size];
    for (int i = 0; i < num_students; i++)
        *(new_gpas + i) = *(gpas + i);
    delete[] gpas;
    gpas = new_gpas;

    // grow statuses array 
    int* new_statuses = new int[new_size];
    for (int i = 0; i < num_students; i++)
        *(new_statuses + i) = *(statuses + i);
    delete[] statuses;
    statuses = new_statuses;
}



// Randomly picks a room, If occupied, does a sequential linear search for next free slot
bool find_free_room(int*** grid, int num_depts, int num_floors, int num_rooms, int &out_d, int &out_f, int &out_r)
{
    int start_d = rand() % num_depts;
    int start_f = rand() % num_floors;
    int start_r = rand() % num_rooms;

    if (*(*(*(grid + start_d) + start_f) + start_r) == 0)
    {
        out_d = start_d;
        out_f = start_f;
        out_r = start_r;
        return true;
    }

    cout << "Room [" << start_d << "][" << start_f << "][" << start_r << "] occupied. Searching...\n";

    int skipped = 0;
    for (int d = 0; d < num_depts; d++)
        for (int f = 0; f < num_floors; f++)
            for (int r = 0; r < num_rooms; r++)
            {
                if (*(*(*(grid + d) + f) + r) == 0)
                {
                    out_d = d; out_f = f; out_r = r;
                    cout << "Free room found after skipping "<< skipped << " slots\n";
                    return true;
                }
                skipped++;
            }

    return false;   // campus completely full
}

// Enroll student
void enroll_student(int*** grid,char** &names,int* &ids,float* &gpas,int* &statuses,int &num_students, int num_depts,int num_floors,int num_rooms)
{
    char temp_name[100];
    cout << "\nEnter student name: ";
    cin.ignore();
    cin.getline(temp_name, 100);

    grow_arrays(names, ids, gpas, statuses, num_students);  //Grow all 4 arrays

    int new_id = 1000 + num_students + 1;
    int name_len = strlen(temp_name);

    *(names + num_students) = new char[name_len + 1];   //+1 for the \0 at end
    strcpy(*(names + num_students), temp_name);
    *(ids + num_students) = new_id;
    *(gpas + num_students) = 2.00;
    *(statuses + num_students) = 0;      // 0 = STUDYING

    int room_d, room_f, room_r;
    bool found = find_free_room(grid, num_depts, num_floors, num_rooms, room_d, room_f, room_r);
    if (!found)
    {
        cout << "ERROR: Campus is full!\n";
        delete[] *(names + num_students);
        return;
    }

    *(*(*(grid + room_d) + room_f) + room_r) = new_id;
    num_students++;

    cout << "\nStudent enrolled!\n";
    cout << " Name : " << *(names   + (num_students - 1)) << "\n";
    cout << " ID   : " << *(ids     + (num_students - 1)) << "\n";
    cout << " Room : Dept " << room_d << " | Floor " << room_f << " | Room "  << room_r << "\n";
}


// Shrinks all 4 parallel arrays by 1 slot when a student graduates
// Opposite of grow_arrays, removes one student at given index
void shrink_arrays(char** &names, int* &ids, float* &gpas, int* &statuses, int &num_students, int index)
{
    int new_size = num_students - 1;   // new size is one less

    // free the graduating student's name memory first
    delete[] *(names + index);

    // shrink names array, copy everyone except the graduating student
    char** new_names = new char*[new_size];
    for (int i = 0, j = 0; i < num_students; i++)
    {
        if (i == index) continue;        // skip graduating student
        *(new_names + j) = *(names + i);
        j++;
    }
    delete[] names;
    names = new_names;   // point to new smaller array

    // shrink ids array
    int* new_ids = new int[new_size];
    for (int i = 0, j = 0; i < num_students; i++)
    {
        if (i == index) continue;        // skip graduating student
        *(new_ids + j) = *(ids + i);
        j++;
    }
    delete[] ids;
    ids = new_ids;

    // shrink gpas array
    float* new_gpas = new float[new_size];
    for (int i = 0, j = 0; i < num_students; i++)
    {
        if (i == index) continue;        // skip graduating student
        *(new_gpas + j) = *(gpas + i);
        j++;
    }
    delete[] gpas;
    gpas = new_gpas;

    // shrink statuses array
    int* new_statuses = new int[new_size];
    for (int i = 0, j = 0; i < num_students; i++)
    {
        if (i == index) continue;        // skip graduating student
        *(new_statuses + j) = *(statuses + i);
        j++;
    }
    delete[] statuses;
    statuses = new_statuses;

    num_students--;   // officially one less student now
}

// Loops every student, rolls dice 1-10, applies the result
void run_timestep(int*** grid,char** &names,int* &ids,float* &gpas,int* &statuses,int &num_students,int num_depts,int num_floors,int num_rooms,int &step)
{
    step++;   // count this as a new time step
    cout << "\n Time Step " << step << " \n";

    int i = 0;
    while (i < num_students)   // use while not for, because num_students changes during graduation
    {
        // roll dice between 1 and 10
        int roll = (rand() % 10) + 1;

        // bitwise flag, tracks what happened to this student this step
        // bit 0 (value 1) = progress
        // bit 1 (value 2) = warning
        // bit 2 (value 4) = graduated
        unsigned char flags = 0;

        if (roll >= 1 && roll <= 5)
        {
            // PROGRESS: GPA goes up 
            *(gpas + i) = *(gpas + i) + 0.1f;

            if (*(gpas + i) > 4.00f)      // cap at 4.00
                *(gpas + i) = 4.00f;

            *(statuses + i) = 0;           // back to STUDYING

            flags = flags | 1;             // set bit 0

            cout << " [PROGRESS]  " << *(names + i) << " | GPA: " << *(gpas + i) << "\n";
        }
        else if (roll >= 6 && roll <= 8)
        {
            // STEADY: nothing changes 
            cout << " [STEADY]    " << *(names + i) << " | GPA: " << *(gpas + i) << "\n";
        }
        else if (roll == 9)
        {
            // WARNING: GPA drops 
            *(gpas + i) = *(gpas + i) - 0.2f;

            if (*(gpas + i) < 0.00f)      // floor at 0.00
                *(gpas + i) = 0.00f;

            *(statuses + i) = 1;           // status = WARNING

            flags = flags | 2;             // set bit 1

            cout << " [WARNING]   " << *(names + i) << " | GPA: " << *(gpas + i) << "\n";
        }
        else if (roll == 10)
        {
            // GRADUATION: student leaves campus 
            *(statuses + i) = 2;           // status = GRADUATING

            flags = flags | 4;             // set bit 2

            // scan entire 3D grid to find this student's room
            // we search by matching student ID in the grid
            for (int d = 0; d < num_depts; d++)
                for (int f = 0; f < num_floors; f++)
                    for (int r = 0; r < num_rooms; r++)
                        if (*(*(*(grid + d) + f) + r) == *(ids + i))
                            *(*(*(grid + d) + f) + r) = 0;   // clear the room 

            // check bit 2 to confirm graduation before printing
            if (flags & 4)
                cout << " [GRADUATED] " << *(names + i) << " | Room cleared. Goodbye!\n";

            // remove this student from all 4 arrays and shrink them
            shrink_arrays(names, ids, gpas, statuses, num_students, i);

            // after shrink, the next student slides into position i automatically
            continue;
        }

        i++;   // only move forward if student did NOT graduate
    }

    cout << " Step " << step << " done. " << num_students << " student remaining.\n";
}


// Displays the live dashboard for the current dept and floor
// Shows every room — occupied or empty — in a formatted table
void display_dashboard(int*** grid,char** names,int* ids,float* gpas,int* statuses,int num_students,int num_depts,int num_floors,int num_rooms,
int* current_dept,int* current_floor,int step)
{
    // clear the screen so dashboard always shows fresh
  //  system("cls");   // use "clear" on Linux/Mac

    int d = *current_dept;    // which dept we are viewing
    int f = *current_floor;   // which floor we are viewing

    cout << "\n";
    cout << "UniVault. LIVE RESOURCE DASHBOARD\n";
    cout << "\n";

    // header line — show current view info
    cout << "[DEPT: "  << d  << "]  "
     << "[FLOOR: " << f  << "]  "
     << "[ROOMS: " << num_rooms << "]  "
     << "[STEP: "  << step  << "]\n";
    
    cout << setfill(' ');   // reset fill back to space after using '0'

 
    cout << left
         << setw(14) << "ROOM ADDR"
         << setw(8)  << "ID"
         << setw(18) << "NAME"
         << setw(8)  << "GPA"
         << setw(12) << "STATUS"
         << "\n";

    // loop through every room on this dept/floor
    for (int r = 0; r < num_rooms; r++)
    {
        // get the value stored in this room (0 = empty, otherwise = student ID)
        int room_val = *(*(*(grid + d) + f) + r);

        // print the actual memory address of this room slot
        cout << left << setw(14) << (void*)(*(*(grid + d) + f) + r);

        if (room_val == 0)
        {
            //  EMPTY ROOM 
            cout << setw(8)  << "----" << setw(18) << "----------" << setw(8)  << "0.00" << setw(12) << "EMPTY" << "\n";
        }
        else
        {
            // OCCUPIED ROOM, find the student in the arrays
            // linear search through ids[] to match room_val
            int found_index = -1;
            for (int s = 0; s < num_students; s++)
            {
                if (*(ids + s) == room_val)
                {
                    found_index = s;
                    break;
                }
            }

            if (found_index != -1)
            {
                // print student info
                // figure out status text
                char status_text[20];
                if      (*(statuses + found_index) == 0) strcpy(status_text, "STUDYING");
                else if (*(statuses + found_index) == 1) strcpy(status_text, "WARNING");
                else                                     strcpy(status_text, "GRADUATING");

                cout << setw(8)  << *(ids   + found_index) << setw(18) << *(names + found_index) << setw(8)  << fixed 
                    << setprecision(2) << *(gpas + found_index) << setw(12) << status_text << "\n";
            }
        }
    }

    cout << "\n";
    cout << "COMMANDS: [ENTER] Step | [E] Enroll | [J] Jump | [F] Find | [S] Save | [X] Exit\n";
    cout << "\n";
}


// Jump command, changes current view to a different dept/floor
// Does NOT run a timestep, just changes what dashboard shows
void jump_view(int num_depts, int num_floors, int* current_dept, int* current_floor)
{
    cout << "\nEnter Department index (0 to " << num_depts  - 1 << "): ";
    cin  >> *current_dept;

    cout << "Enter Floor index     (0 to " << num_floors - 1 << "): ";
    cin  >> *current_floor;

    // clamp values in case user types out of range
    if (*current_dept  < 0) *current_dept  = 0;
    if (*current_dept  >= num_depts)  *current_dept  = num_depts  - 1;
    if (*current_floor < 0) *current_floor = 0;
    if (*current_floor >= num_floors) *current_floor = num_floors - 1;

    cout << "View jumped to Dept " << *current_dept
         << " Floor "              << *current_floor << "\n";
}


// Search for a student by ID
// Linear search through ids[] one by one, then scan grid to find their room
void find_student(int* ids, char** names, float* gpas, int* statuses, int num_students,
                  int*** grid, int num_depts, int num_floors, int num_rooms)
{
    int search_id;
    cout << "\nEnter Student ID to search: ";
    cin >> search_id;

    // linear search through ids[] one by one until match found
    int found_index = -1;
    for (int i = 0; i < num_students; i++)
    {
        if (*(ids + i) == search_id)
        {
            found_index = i;   // save the index where match was found
            break;
        }
    }

    if (found_index == -1)
    {
        // no student matched that ID
        cout << "\n NOT FOUND — No student with ID " << search_id << "\n";
        return;
    }

    // figure out status text to print
    char status_text[20];
    if (*(statuses + found_index) == 0) strcpy(status_text, "STUDYING");
    else if (*(statuses + found_index) == 1) strcpy(status_text, "WARNING");
    else strcpy(status_text, "GRADUATING");

    // print full student record
    cout << "\n Student Found!\n";
    cout << " Name   : " << *(names + found_index) << "\n";
    cout << " ID     : " << *(ids   + found_index) << "\n";
    cout << " GPA    : " << fixed << setprecision(2) << *(gpas + found_index) << "\n";
    cout << " Status : " << status_text << "\n";

    // scan entire 3D grid to find which room belongs to this student
    // search by matching student ID in the grid
    for (int d = 0; d < num_depts; d++)
        for (int f = 0; f < num_floors; f++)
            for (int r = 0; r < num_rooms; r++)
                if (*(*(*(grid + d) + f) + r) == search_id)
                {
                    cout << " Room   : Dept " << d << " | Floor " << f << " | Room " << r << "\n";
                    return;  
                }
}




// Save all students and grid occupancy to database.txt
// Scans 3D grid to find each student's room coordinates
void save_data(int*** grid, char** names, int* ids, float* gpas, int* statuses, int num_students, int num_depts, int num_floors, int num_rooms)
{
    ofstream fout("database.txt");

    if (!fout.is_open())
    {
        cout << "ERROR: Could not open database.txt for saving!\n";
        return;
    }

    // line 1 — write total number of students
    fout << num_students << "\n";

    // lines 2 to N — one line per student
    // format: id name gpa status deptIndex floorIndex roomIndex
    for (int i = 0; i < num_students; i++)
    {
        // scan entire 3D grid to find this student's room coordinates
        int save_d = -1, save_f = -1, save_r = -1;
        for (int d = 0; d < num_depts; d++)
            for (int f = 0; f < num_floors; f++)
                for (int r = 0; r < num_rooms; r++)
                    if (*(*(*(grid + d) + f) + r) == *(ids + i))    //We store the student's ID INTO their assigned room
                                                                    //So the grid doesn't store room numbers — it stores STUDENT IDs!
                    {
                        save_d = d;
                        save_f = f;
                        save_r = r;
                    }

        // write student record to file
       // "Ali Ahmed" becomes "Ali_Ahmed" in the file
       char safe_name[100];
       strcpy(safe_name, *(names + i));
       for (int c = 0; c < (int)strlen(safe_name); c++)
            if (safe_name[c] == ' ') safe_name[c] = '_';   // space → underscore
            
    fout << *(ids + i) << " "<< safe_name << " "<< *(gpas + i) << " "<< *(statuses + i) << " "
     << save_d  << " "
     << save_f  << " "
     << save_r << "\n";
    }

    fout.close();
    cout << "Data saved to database.txt successfully!\n";
}


// Load all students and grid occupancy from database.txt
// Rebuilds all 4 parallel arrays and restores the 3D grid, (Create NEw arrays)
void load_data(int*** grid, char** &names, int* &ids, float* &gpas, int* &statuses, int &num_students, int num_depts, int num_floors, int num_rooms)
{
    ifstream fin("database.txt");

    if (!fin.is_open())
    {
        cout << "No database.txt found. Starting fresh.\n";


         // create fresh empty arrays so pointers are valid
    names    = new char*[0];
    ids      = new int[0];
    gpas     = new float[0];
    statuses = new int[0];
        return;   // no save file exists, just start empty
    }

    // line 1 — read total number of students
    fin >> num_students;
    // allocate all 4 arrays to exact size needed
    names = new char*[num_students];
    ids = new int[num_students];
    gpas = new float[num_students];
    statuses = new int[num_students];

    // read each student line one by one
    for (int i = 0; i < num_students; i++)
    {
        char temp_name[100];
        int load_id, load_status, load_d, load_f, load_r;
        float load_gpa;

        // read: id name gpa status dept floor room
        fin >> load_id >> temp_name >> load_gpa >> load_status >> load_d >> load_f >> load_r;

        // convert underscores back to spaces in the name
        // "Ali_Ahmed" becomes "Ali Ahmed" again
        // ihave done Reverse process in save_file
        for (int c = 0; c < (int)strlen(temp_name); c++)
                if (temp_name[c] == '_') temp_name[c] = ' ';

        // allocate exact memory for name and copy it in
        *(names + i) = new char[strlen(temp_name) + 1];   // +1 for \0 at end
        strcpy(*(names + i), temp_name);
        *(ids + i) = load_id;
        *(gpas + i) = load_gpa;
        *(statuses + i) = load_status;

        // restore this student's ID back into their room in the 3D grid
        *(*(*(grid + load_d) + load_f) + load_r) = load_id;
    }

    fin.close();
    cout << "Data loaded from database.txt. " << num_students << " student(s) restored.\n";
}


int main()
{
    srand(time(0));
    int step = 0;   // tracks how many time steps have run
    int current_dept  = 0;   // dashboard starts showing dept 0
    int current_floor = 0;   // dashboard starts showing floor 0

    int num_depts, num_floors, num_rooms;
    read_config(num_depts, num_floors, num_rooms);

    int*** campus = init_campus(num_depts, num_floors, num_rooms);

    // initialise empty parallel arrays
    char** names  = new char*[0];   //names  ["Ali", "Sara", "Zain"]  each slot = a word thats why 2d array used
    int* ids = new int[0];
    float* gpas = new float[0];
    int* statuses = new int[0];
    int num_students = 0;
     

    // free the empty arrays before load_data replaces them
    delete[] names;
    delete[] ids;
    delete[] gpas;
    delete[] statuses;

    //loading From database.txt
    load_data(campus, names, ids, gpas, statuses, num_students, num_depts, num_floors, num_rooms);

    // show dashboard once on startup
    display_dashboard(campus, names, ids, gpas, statuses, num_students,num_depts, num_floors, num_rooms,&current_dept, &current_floor, step);


    // simple menu loop
    char choice;
    do
    {
        cout << "\n[E] Enroll  [T] TimeStep  [J] Jump  [F] Find  [S] Save  [X] Exit\nChoice: ";
        cin>>choice;

    if (choice == 'E' || choice == 'e')   //Adding a New Student to the University
    {
    enroll_student(campus, names, ids, gpas, statuses, num_students, num_depts, num_floors, num_rooms);
    //it is calling grow_arrays and find_free_room inside itself

    // redraw dashboard so new student appears immediately
    display_dashboard(campus, names, ids, gpas, statuses, num_students, num_depts, num_floors, num_rooms, &current_dept, &current_floor, step);
    }
        
        if (choice == 'T' || choice == 't')
        {
        run_timestep(campus, names, ids, gpas, statuses, num_students,num_depts, num_floors, num_rooms, step); //one semester passes for every student at the same time
        //shrink_array is used in this func
        display_dashboard(campus, names, ids, gpas, statuses, num_students,num_depts, num_floors, num_rooms,&current_dept, &current_floor, step);
        }  
            

        if (choice == 'J' || choice == 'j')
         {
        jump_view(num_depts, num_floors, &current_dept, &current_floor);
         // redraw dashboard after jump
        display_dashboard(campus, names, ids, gpas, statuses, num_students,num_depts, num_floors, num_rooms,&current_dept, &current_floor, step);
         }
        
         if (choice == 'F' || choice == 'f')   // search for a student by ID
         {
           find_student(ids, names, gpas, statuses, num_students, campus, num_depts, num_floors, num_rooms);
           // linear search through ids[] array to find the student
         }

        if (choice == 'S' || choice == 's')   // save everything to database.txt
         {
           save_data(campus, names, ids, gpas, statuses, num_students, num_depts, num_floors, num_rooms);
           // scans 3D grid to find each student's room before saving
        }
  

    } while (choice != 'X' && choice != 'x');

    // cleanup
    for (int i = 0; i < num_students; i++)
        delete[] *(names + i);
    delete[] names;
    delete[] ids;
    delete[] gpas;
    delete[] statuses;

    free_campus(campus, num_depts, num_floors);
    cout << "Goodbye!\n";
    return 0;
}